#include "Agent.h"
#include "SimulationConfig.h"
#include <cmath>
#include <fstream>
#include <algorithm>
#include <limits>

// Constructeur pour un nouvel agent
Agent::Agent(const std::string& name, unsigned int id, int startX, int startY, int input_size, int hidden_size)
    : brain(input_size, hidden_size),
      config{name, id, 70.0, 10.0, startX, startY, '@'} 
{}


std::pair<double, double> Agent::findClosestFood(const Map& map) const {
    double min_dist = std::numeric_limits<double>::max();
    double angle = 0.0;
    bool found = false;
    
    // Balayer dans un rayon de VISION_RANGE autour de l'agent
    for (int dy = -VISION_RANGE * 2; dy <= VISION_RANGE * 2; ++dy) {
        for (int dx = -VISION_RANGE * 2; dx <= VISION_RANGE * 2; ++dx) {
            int x = config.x + dx;
            int y = config.y + dy;
            
            if (map.isValidPosition(x, y)) {
                CellType cell = map.getCell(x, y);
                if (cell == CellType::APPLE || cell == CellType::CHAMPIGNON_LUMINEUX) {
                    double dist = std::sqrt(dx*dx + dy*dy);
                    if (dist < min_dist && dist > 0) {
                        min_dist = dist;
                        angle = std::atan2(dy, dx);
                        found = true;
                    }
                }
            }
        }
    }
    
    if (!found) {
        return {-1.0, 0.0};  
    }
    return {min_dist, angle};
}

std::pair<double, double> Agent::findClosestAgent(const std::vector<Agent>& all_agents) const {
    double min_dist = std::numeric_limits<double>::max();
    double angle = 0.0;
    bool found = false;
    
    for (const auto& other : all_agents) {
        if (other.getId() == this->getId()) continue;
        
        int dx = other.getX() - config.x;
        int dy = other.getY() - config.y;
        double dist = std::sqrt(dx*dx + dy*dy);
        
        if (dist < min_dist && dist > 0) {
            min_dist = dist;
            angle = std::atan2(dy, dx);
            found = true;
        }
    }
    
    if (!found) {
        return {-1.0, 0.0};  // Pas d'agent trouvé
    }
    return {min_dist, angle};
}

int Agent::countNearbyAgents(const std::vector<Agent>& all_agents, int radius) const {
    int count = 0;
    for (const auto& other : all_agents) {
        if (other.getId() == this->getId()) continue;
        
        int dx = std::abs(other.getX() - config.x);
        int dy = std::abs(other.getY() - config.y);
        
        if (dx <= radius && dy <= radius) {
            count++;
        }
    }
    return count;
}

double Agent::getAverageSocialScore() const {
    if (social_memory.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (const auto& [id, score] : social_memory) {
        sum += score;
    }
    return sum / social_memory.size();
}



std::vector<double> Agent::perceive(const Map& map, const std::vector<Agent>& all_agents, bool is_day) {
    std::vector<double> perception_vector;
    perception_vector.reserve(INPUT_SIZE);  // INPUT_SIZE = 11
    
    // 1-2. États internes (énergie, satisfaction) - normalisés [0, 1]
    perception_vector.push_back(config.energie / MAX_STAT);
    perception_vector.push_back(config.satisfaction / MAX_STAT);
    
    // 3-4. Nourriture la plus proche (distance + angle)
    auto [food_dist, food_angle] = findClosestFood(map);
    if (food_dist > 0) {
        perception_vector.push_back(food_dist / MAX_DISTANCE);  // Normalisé [0, 1]
        perception_vector.push_back(food_angle / M_PI);         // Normalisé [-1, 1]
    } else {
        perception_vector.push_back(0.0);  // Pas de nourriture visible
        perception_vector.push_back(0.0);
    }
    
    // 5-6. Agent le plus proche (distance + angle)
    auto [agent_dist, agent_angle] = findClosestAgent(all_agents);
    if (agent_dist > 0) {
        perception_vector.push_back(agent_dist / MAX_DISTANCE);
        perception_vector.push_back(agent_angle / M_PI);
    } else {
        perception_vector.push_back(0.0);  // Pas d'agent visible
        perception_vector.push_back(0.0);
    }
    
    // 7. Jour/Nuit (booléen)
    perception_vector.push_back(is_day ? 1.0 : 0.0);
    
    // 8. Nombre d'agents proches (dans un rayon de 3) - normalisé
    int nearby_count = countNearbyAgents(all_agents, 3);
    perception_vector.push_back(nearby_count / 10.0);  // Max ~10 agents proches
    
    // 9. Type de case actuelle (normalisé)
    CellType current_cell = map.getCell(config.x, config.y);
    perception_vector.push_back(static_cast<double>(current_cell) / 10.0);
    
    // 10. Moyenne des relations sociales (normalisé)
    double avg_social = getAverageSocialScore();
    perception_vector.push_back(std::tanh(avg_social / 10.0));  // Normalisé [-1, 1]
    
    // 11. Niveau d'urgence (combinaison énergie/satisfaction)
    double urgency = (100.0 - config.energie) / 100.0;  // Plus l'énergie est basse, plus c'est urgent
    perception_vector.push_back(urgency);
    
    return perception_vector;
}



std::vector<double> Agent::think(const std::vector<double>& perception_vector) {
    return brain.forward(perception_vector);
}

// Fonctions d'action privées
void Agent::_eat(Map& map) {
    CellType cell_type = map.getCell(config.x, config.y);
    int nutrition = 0;
    if (cell_type == CellType::APPLE) {
        nutrition = GAIN_ENERGY_APPLE;
    } else if (cell_type == CellType::CHAMPIGNON_LUMINEUX) {
        nutrition = GAIN_ENERGY_CHAMP;
    }

    if (nutrition > 0) {
        config.satisfaction = std::min(MAX_STAT, config.satisfaction + GAIN_SATISFACTION_EAT);
        config.energie = std::min(MAX_STAT, config.energie + nutrition);
        map.startRegrowth(config.x, config.y, cell_type);
    }
}

void Agent::_move(Map& map, std::mt19937& rng) {
    std::uniform_int_distribution<int> distrib(-1, 1);
    int dx = distrib(rng);
    int dy = distrib(rng);
    int newX = config.x + dx;
    int newY = config.y + dy;
    if (map.isValidPosition(newX, newY) && map.getCell(newX, newY) != CellType::WATER) {
        config.x = newX;
        config.y = newY;
    }
}

Agent* Agent::_findNearbyAgent(std::vector<Agent>& all_agents) {
    for (auto& other : all_agents) {
        if (other.getId() != this->getId() && other.getEnergie() > 15) {
            int dx = std::abs(other.getX() - config.x);
            int dy = std::abs(other.getY() - config.y);
            if (dx <= 1 && dy <= 1) {
                return &other;
            }
        }
    }
    return nullptr;
}

void Agent::_interact(Map& map) {
    if (map.getCell(config.x, config.y) == CellType::BOOK) {
        addSatisfaction(GAIN_SATISFACTION_BOOK);
        map.setCell(config.x, config.y, CellType::EMPTY);
    }
}

// Exécution des actions
void Agent::act(const std::vector<double>& decision_vector, Map& map, std::vector<Agent>& all_agents, bool is_day, std::mt19937& rng) {
    
    // Si l'agent n'a plus d'énergie, il ne peut rien faire
    if (config.energie <= 0) {
        return;
    }

    // Sélectionner l'action avec la plus grande valeur de sortie
    auto max_it = std::max_element(decision_vector.begin(), decision_vector.end());
    int action_choice = std::distance(decision_vector.begin(), max_it);

    // Exécuter l'action choisie
    switch (action_choice) {
        case 0: // Manger
            _eat(map);
            break;

        case 1: // Parler
            {
                Agent* target = _findNearbyAgent(all_agents);
                if (target) {
                    addSatisfaction(GAIN_SATISFACTION_TALK_BASE);
                    target->addSatisfaction(GAIN_SATISFACTION_TALK_BASE);
                    updateSocialScoreFor(target->getId(), 1);
                    target->updateSocialScoreFor(this->getId(), 1);
                }
            }
            break;

        case 2: // Dormir
            if (!is_day) {
                config.energie = std::min(MAX_STAT, config.energie + GAIN_ENERGY_SLEEP);
            }
            // Pas de gain le jour (géré par le coût de vie)
            break;

        case 3: // Interagir avec objet (livre)
            _interact(map);
            break;

        // Toutes les autres décisions (4, 5, 6, 7...) mènent à un mouvement
        default: 
            _move(map, rng);
            break;
    }

    // Coût de vie unique à la fin 
    double living_cost = is_day ? 1.0 : 1.5;
    config.energie = std::max(0.0, config.energie - living_cost);

    config.satisfaction = std::max(0.0, config.satisfaction - 0.5);
}



void Agent::addSatisfaction(double amount) {
    config.satisfaction = std::min(MAX_STAT, config.satisfaction + amount);
}

int Agent::getSocialScoreFor(unsigned int agent_id) const {
    auto it = social_memory.find(agent_id);
    if (it != social_memory.end()) {
        return it->second;
    }
    return 0;
}

void Agent::updateSocialScoreFor(unsigned int agent_id, int change) {
    social_memory[agent_id] += change;
}

void Agent::receiveFoodInfo(std::pair<int, int> pos) {
    last_known_food_pos = pos;
}

// Évolution génétique
void Agent::mutateBrain(double mutationRate) {
    brain.mutate(mutationRate);
}

Agent Agent::breedWith(const Agent& partner, const std::string& childName, unsigned int childId, int startX, int startY) const {
    Agent child(childName, childId, startX, startY, brain.getInputSize(), brain.getHiddenSize());
    LSTM childBrain = this->brain.breed(partner.getBrain());
    child.setBrain(childBrain);
    return child;
}

void Agent::logSocialMemory(int current_day, std::ofstream& logfile) const {
    if (social_memory.empty()) return;

    for (const auto& pair : social_memory) {
        unsigned int target_id = pair.first;
        int score = pair.second;
        logfile << current_day << "," << config.id << "," << target_id << "," << score << "\n";
    }
}