#include "Agent.h"
#include <cmath>
#include <fstream>
#include <algorithm>


// Constructeur pour un nouvel agent
Agent::Agent(const std::string& name, unsigned int id, int startX, int startY, int input_size, int hidden_size)
    : brain(input_size, hidden_size, id + std::random_device{}()), // On utilise l'ID + entropy pour varier
      config{name, id, 70.0, 10.0, startX, startY, '@'} 
{}


// L'agent observe son environnement et le transforme en vecteur pour son cerveau
// Dans agent.cpp

std::vector<double> Agent::perceive(const Map& map, const std::vector<Agent>& all_agents, bool is_day) {
    std::vector<double> inputs;
    inputs.reserve(Agent::PERCEPTION_SIZE);

    // --- 1. État Interne (2 neurones) ---
    inputs.push_back(config.energie / 100.0);
    inputs.push_back(config.satisfaction / 100.0);

    // --- 2. Mémoire (2 neurones) ---
    // (Inchangé : on garde la mémoire de la dernière nourriture vue)
    if(last_known_food_pos.has_value()) {
        int dx = last_known_food_pos->first - config.x;
        int dy = last_known_food_pos->second - config.y;
        inputs.push_back(std::sqrt(dx*dx + dy*dy) / MAX_DISTANCE);
        inputs.push_back(std::atan2(dy, dx) / M_PI);
    } else {
        inputs.push_back(0.0);
        inputs.push_back(0.0);
    }

    // --- 3. Vision Relative (Analyse des alentours) ---
    // On cherche la nourriture et l'agent les plus proches dans le champ de vision

    double nearest_food_dist = 1.0; // 1.0 = loin / pas vu
    double nearest_food_angle = 0.0;
    
    double nearest_agent_dist = 1.0;
    double nearest_agent_angle = 0.0;
    double nearest_agent_social = 0.0;

    bool on_food = false;
    bool on_book = false;

    // On scanne le carré de vision (comme avant)
    for (int dy = -VISION_RANGE; dy <= VISION_RANGE; ++dy) {
        for (int dx = -VISION_RANGE; dx <= VISION_RANGE; ++dx) {
            if (dx == 0 && dy == 0) {
                // Analyse de la case sur laquelle on est
                CellType current_cell = map.getCell(config.x, config.y);
                if (current_cell == CellType::APPLE || current_cell == CellType::CHAMPIGNON_LUMINEUX) on_food = true;
                if (current_cell == CellType::BOOK) on_book = true;
                continue; 
            }

            int cx = config.x + dx;
            int cy = config.y + dy;
            
            double dist = std::sqrt(dx*dx + dy*dy) / (double)VISION_RANGE; // Normalisé 0-1
            double angle = std::atan2(dy, dx) / M_PI; // Normalisé -1 à 1

            // A. Chercher nourriture
            CellType cell = map.getCell(cx, cy);
            if (cell == CellType::APPLE || cell == CellType::CHAMPIGNON_LUMINEUX) {
                if (dist < nearest_food_dist) {
                    nearest_food_dist = dist;
                    nearest_food_angle = angle;
                }
            }

            // B. Chercher agents
            for (const auto& other : all_agents) {
                if (other.getId() != this->getId() && other.getX() == cx && other.getY() == cy) {
                    if (dist < nearest_agent_dist) {
                        nearest_agent_dist = dist;
                        nearest_agent_angle = angle;
                        nearest_agent_social = std::tanh(getSocialScoreFor(other.getId()) / 5.0);
                    }
                }
            }
        }
    }

    // --- Remplissage du vecteur (7 neurones) ---
    inputs.push_back(nearest_food_dist);
    inputs.push_back(nearest_food_angle);
    
    inputs.push_back(nearest_agent_dist);
    inputs.push_back(nearest_agent_angle);
    inputs.push_back(nearest_agent_social);

    inputs.push_back(on_food ? 1.0 : 0.0); // "Suis-je sur à manger ?"
    inputs.push_back(on_book ? 1.0 : 0.0); // "Suis-je sur un livre ?"

    return inputs;
}


// L'agent utilise son cerveau pour prendre une décision à partir de ses perceptions
std::vector<double> Agent::think(const std::vector<double>& perception_vector) {
    return brain.forward(perception_vector);
}


// Fonctions d'action privées
void Agent::_eat(Map& map) {
    CellType cell_type = map.getCell(config.x, config.y);
    int nutrition = 0;
    if (cell_type == CellType::APPLE) {
        nutrition = 40;
        
    } else if (cell_type == CellType::CHAMPIGNON_LUMINEUX) {
        nutrition = 60;
    }

    if (nutrition > 0) {
        config.satisfaction = std::min(100.0, config.satisfaction + 2.0);
        config.energie = std::min(100.0, config.energie + nutrition);
        map.startRegrowth(config.x, config.y, cell_type); // La map gère la repousse
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
        addSatisfaction(15.0);
        map.setCell(config.x, config.y, CellType::EMPTY);
    }
}


// L'agent exécute une action en fonction de la sortie de son cerveau
void Agent::act(const std::vector<double>& decision_vector, Map& map, std::vector<Agent>& all_agents, bool is_day, std::mt19937& rng) {
    
    // Si l'agent n'a plus d'énergie, il ne peut rien faire.
    if (config.energie <= 0) {
        return;
    }

    auto max_it = std::max_element(decision_vector.begin(), decision_vector.end());
    int action_choice = std::distance(decision_vector.begin(), max_it);

    // Coût de base des actions, pour ne pas avoir de "magie" dans le code
    const double EAT_COST = 0.0;
    const double TALK_COST = 1.0;
    const double DAY_SLEEP_PENALTY = 1.0;
    const double INTERACT_COST = 1.0;
    const double MOVE_COST = 1.0;
    const double LIVING_COST = 0.5;


    switch (action_choice) {
        case 0: // Manger
            _eat(map);
            config.energie = std::max(0.0, config.energie - EAT_COST);
            break;

        case 1: // Parler
            {
                Agent* target = _findNearbyAgent(all_agents);
                if (target) {
                    int score_with_target = getSocialScoreFor(target->getId());
                    double gain_satisfaction = std::max(1.0, 3.0 + (score_with_target / 2.0));
                    addSatisfaction(gain_satisfaction);
                    target->addSatisfaction(gain_satisfaction);
                    updateSocialScoreFor(target->getId(), 1);
                    target->updateSocialScoreFor(this->getId(), 1);
                }
            }
            config.energie = std::max(0.0, config.energie - TALK_COST);
            break;

        case 2: // Dormir
            if (!is_day) {
                config.energie = std::min(100.0, config.energie + 25);
            } else {
                config.energie = std::max(0.0, config.energie - DAY_SLEEP_PENALTY);
            }
            break;

        case 3: // Interagir avec objet (livre)
            _interact(map);
            config.energie = std::max(0.0, config.energie - INTERACT_COST);
            break;

        // Toutes les autres décisions (4, 5, 6, 7...) mènent à un mouvement
        default: 
            _move(map, rng);
            config.energie = std::max(0.0, config.energie - MOVE_COST);
            break;
    }

    // Coût de la vie (appliqué à chaque tour, quelle que soit l'action)
    config.energie = std::max(0.0, config.energie - LIVING_COST);
}


// --- Le reste des fonctions reste inchangé ---

void Agent::addSatisfaction(double amount) {
    config.satisfaction = std::min(100.0, config.satisfaction + amount);
}

int Agent::getSocialScoreFor(unsigned int agent_id) const {
    auto it = social_memory.find(agent_id);
    if (it != social_memory.end()) {
        return it->second;
    }
    return 0; // Score par défaut pour un inconnu
}

void Agent::updateSocialScoreFor(unsigned int agent_id, int change) {
    social_memory[agent_id] += change;
}

void Agent::receiveFoodInfo(std::pair<int, int> pos) {
    last_known_food_pos = pos;
}
// Fonctions pour l'évolution
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

