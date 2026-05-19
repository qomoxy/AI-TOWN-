#include "Agent.h"
#include "SimulationConfig.h"
#include <cmath>
#include <fstream>
#include <algorithm>
#include <limits>

// ============================================================
// Constructeur
// ============================================================

Agent::Agent(const std::string& name, unsigned int id, int startX, int startY,
             int input_size, int hidden_size)
    : brain(input_size, hidden_size),
      config{name, id, 70.0, 10.0, startX, startY, '@'}
{}

// ============================================================
// Perception
// ============================================================

std::pair<double, double> Agent::findClosestFood(const Map& map) const {
    double min_dist = std::numeric_limits<double>::max();
    double angle    = 0.0;
    bool   found    = false;

    for (int dy = -VISION_RANGE * 2; dy <= VISION_RANGE * 2; ++dy) {
        for (int dx = -VISION_RANGE * 2; dx <= VISION_RANGE * 2; ++dx) {
            int x = config.x + dx;
            int y = config.y + dy;
            if (map.isValidPosition(x, y)) {
                CellType cell = map.getCell(x, y);
                if (cell == CellType::APPLE || cell == CellType::CHAMPIGNON_LUMINEUX) {
                    double dist = std::sqrt(dx * dx + dy * dy);
                    if (dist < min_dist && dist > 0) {
                        min_dist = dist;
                        angle    = std::atan2(dy, dx);
                        found    = true;
                    }
                }
            }
        }
    }

    return found ? std::make_pair(min_dist, angle)
                 : std::make_pair(-1.0, 0.0);
}

std::pair<double, double> Agent::findClosestAgent(const std::vector<Agent>& all_agents) const {
    double min_dist = std::numeric_limits<double>::max();
    double angle    = 0.0;
    bool   found    = false;

    for (const auto& other : all_agents) {
        if (other.getId() == this->getId()) continue;
        int    dx   = other.getX() - config.x;
        int    dy   = other.getY() - config.y;
        double dist = std::sqrt(dx * dx + dy * dy);
        if (dist < min_dist && dist > 0) {
            min_dist = dist;
            angle    = std::atan2(dy, dx);
            found    = true;
        }
    }

    return found ? std::make_pair(min_dist, angle)
                 : std::make_pair(-1.0, 0.0);
}

int Agent::countNearbyAgents(const std::vector<Agent>& all_agents, int radius) const {
    int count = 0;
    for (const auto& other : all_agents) {
        if (other.getId() == this->getId()) continue;
        if (std::abs(other.getX() - config.x) <= radius &&
            std::abs(other.getY() - config.y) <= radius)
            ++count;
    }
    return count;
}

double Agent::getAverageSocialScore() const {
    if (social_memory.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& [id, score] : social_memory) sum += score;
    return sum / (double)social_memory.size();
}

// ============================================================
// perceive / think
// ============================================================

std::vector<double> Agent::perceive(const Map& map, const std::vector<Agent>& all_agents,
                                    bool is_day) {
    std::vector<double> pv;
    pv.reserve(INPUT_SIZE); // 11 entrées

    // 1-2. États internes normalisés [0, 1]
    pv.push_back(config.energie     / MAX_STAT);
    pv.push_back(config.satisfaction / MAX_STAT);

    // 3-4. Nourriture la plus proche
    auto [food_dist, food_angle] = findClosestFood(map);
    pv.push_back(food_dist > 0 ? food_dist / MAX_DISTANCE : 0.0);
    pv.push_back(food_dist > 0 ? food_angle / M_PI        : 0.0);

    // 5-6. Agent le plus proche
    auto [agent_dist, agent_angle] = findClosestAgent(all_agents);
    pv.push_back(agent_dist > 0 ? agent_dist / MAX_DISTANCE : 0.0);
    pv.push_back(agent_dist > 0 ? agent_angle / M_PI        : 0.0);

    // 7. Jour/Nuit
    pv.push_back(is_day ? 1.0 : 0.0);

    // 8. Agents proches (rayon 3), normalisé
    pv.push_back(countNearbyAgents(all_agents, 3) / 10.0);

    // 9. Type de case actuelle, normalisé
    pv.push_back(static_cast<double>(map.getCell(config.x, config.y)) / 10.0);

    // 10. Moyenne sociale normalisée [-1, 1]
    pv.push_back(std::tanh(getAverageSocialScore() / 10.0));

    // 11. Niveau d'urgence [0, 1]
    pv.push_back((100.0 - config.energie) / 100.0);

    return pv;
}

std::vector<double> Agent::think(const std::vector<double>& perception_vector) {
    return brain.forward(perception_vector);
}

// ============================================================
// Actions privées
// ============================================================

void Agent::_eat(Map& map) {
    CellType cell_type = map.getCell(config.x, config.y);
    int      nutrition = 0;

    if      (cell_type == CellType::APPLE)             nutrition = GAIN_ENERGY_APPLE;
    else if (cell_type == CellType::CHAMPIGNON_LUMINEUX) nutrition = GAIN_ENERGY_CHAMP;

    if (nutrition > 0) {
        config.satisfaction = std::min(MAX_STAT, config.satisfaction + GAIN_SATISFACTION_EAT);
        config.energie      = std::min(MAX_STAT, config.energie      + (double)nutrition);
        map.startRegrowth(config.x, config.y, cell_type);
    }
}

void Agent::_move(Map& map, std::mt19937& rng) {
    std::uniform_int_distribution<int> distrib(-1, 1);
    int dx   = distrib(rng);
    int dy   = distrib(rng);
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
            if (std::abs(other.getX() - config.x) <= 1 &&
                std::abs(other.getY() - config.y) <= 1)
                return &other;
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

// ============================================================
// act
// ============================================================

void Agent::act(const std::vector<double>& decision_vector, Map& map,
                std::vector<Agent>& all_agents, bool is_day, std::mt19937& rng) {
    if (config.energie <= 0) return;

    auto max_it     = std::max_element(decision_vector.begin(), decision_vector.end());
    int  action_idx = (int)std::distance(decision_vector.begin(), max_it);

    switch (action_idx) {
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
            break;
        }
        case 2: // Dormir
            if (!is_day)
                config.energie = std::min(MAX_STAT, config.energie + (double)GAIN_ENERGY_SLEEP);
            break;
        case 3: // Lire un livre
            _interact(map);
            break;
        default: // Bouger
            _move(map, rng);
            break;
    }

    // Coût de vie : 1.0 le jour, 1.5 la nuit
    double living_cost = is_day ? 1.0 : 1.5;
    config.energie      = std::max(0.0, config.energie      - living_cost);
    config.satisfaction = std::max(0.0, config.satisfaction - 0.5);
}

// ============================================================
// Utilitaires sociaux
// ============================================================

void Agent::addSatisfaction(double amount) {
    config.satisfaction = std::min(MAX_STAT, config.satisfaction + amount);
}

int Agent::getSocialScoreFor(unsigned int agent_id) const {
    auto it = social_memory.find(agent_id);
    return (it != social_memory.end()) ? it->second : 0;
}

void Agent::updateSocialScoreFor(unsigned int agent_id, int change) {
    social_memory[agent_id] += change;
}

void Agent::receiveFoodInfo(std::pair<int, int> pos) {
    last_known_food_pos = pos;
}

void Agent::logSocialMemory(int current_day, std::ofstream& logfile) const {
    for (const auto& [target_id, score] : social_memory)
        logfile << current_day << "," << config.id << "," << target_id << "," << score << "\n";
}

// ============================================================
// Évolution génétique
// ============================================================

// CORRECTIF : rng passé en paramètre, plus de générateur interne non contrôlé
void Agent::mutateBrain(double mutationRate, std::mt19937& rng) {
    brain.mutate(mutationRate, rng);
}

Agent Agent::breedWith(const Agent& partner, const std::string& childName,
                       unsigned int childId, int startX, int startY,
                       std::mt19937& rng) const {
    Agent child(childName, childId, startX, startY,
                brain.getInputSize(), brain.getHiddenSize());
    LSTM childBrain = this->brain.breed(partner.getBrain(), rng);
    child.setBrain(childBrain);
    return child;
}
