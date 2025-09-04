#include "Agent.h"
#include <cmath>
#include <fstream>


// Constructeur pour un nouvel agent
Agent::Agent(const std::string& name, unsigned int id, int startX, int startY, int input_size, int hidden_size)
    : brain(input_size, hidden_size),
      config{name, id, 50.0, 25.0, startX, startY, '@'} // Initialisation de la struct
{}


// L'agent observe son environnement et le transforme en vecteur pour son cerveau
std::vector<double> Agent::perceive(const Map& map, const std::vector<Agent>& all_agents, bool is_day) {
    std::vector<double> perception_vector;
    
    perception_vector.push_back(config.energie / 100.0);
    perception_vector.push_back(config.satisfaction / 100.0);

    int vision_range = is_day ? 4 : 2;

    if(last_known_food_pos.has_value()) {
        int dx = last_known_food_pos->first - config.x;
        int dy = last_known_food_pos->second - config.y;
        perception_vector.push_back(std::sqrt(dx*dx + dy*dy) / 10.0); // Distance normalisée
        perception_vector.push_back(std::atan2(dy, dx) / M_PI); // Angle normalisé (-1 à 1)
    } else {
        perception_vector.push_back(0.0); // Pas d'info
        perception_vector.push_back(0.0); // Pas d'info
    }

    for (int i = -vision_range; i <= vision_range; ++i) {
        for (int j = -vision_range; j <= vision_range; ++j) {
            int check_x = config.x + j;
            int check_y = config.y + i;

            if (!map.isValidPosition(check_x, check_y)) {
                perception_vector.push_back(-1.0);
            } else {
                perception_vector.push_back(static_cast<double>(map.getCell(check_x, check_y)) / 5.0);
            }

            bool agent_found = false;
            for(const auto& other_agent : all_agents) {
                if (this->config.id == other_agent.config.id) continue; // Ne pas se voir soi-même
                if (other_agent.getPosition().first == check_x && other_agent.getPosition().second == check_y) {
                    agent_found = true;
                    int relation_score = 0;
                    if (social_memory.count(other_agent.getId())) {
                        relation_score = social_memory.at(other_agent.getId());
                    }
                    perception_vector.push_back(relation_score / 10.0);
                    break;
                }
            }
        }
    }

    return perception_vector; 
}

// L'agent utilise son cerveau pour prendre une décision
std::vector<double> Agent::think(const std::vector<double>& perception_vector) {
    return brain.forward(perception_vector);
}
// Divisé act en sous fonction privée *
Agent* Agent::_findNearbyAgent(std::vector<Agent>& all_agents) {
    for (auto& other_agent : all_agents) {
        if (config.id == other_agent.config.id) continue;

        int dist_x = abs(config.x - other_agent.getX());
        int dist_y = abs(config.y - other_agent.getY());

        if (dist_x <= 1 && dist_y <= 1) { 
            return &other_agent; 
        }
    }
    return nullptr; // Personne à proximité
}

void Agent::_interact(Map& map) {
    for(int i = -1; i <= 1; i++) {
        for(int j = - 1; j <= 1; j++) { 
            int check_x = config.x + i;
            int check_y = config.y + j;
            if(map.getCell(check_x,check_y) == CellType::BOOK && map.isValidPosition(check_x,check_y)) {
                config.satisfaction += 2;
                last_known_food_pos = {check_x,check_y};
                return;
            }//Checker si il y a quelque chose avec quoi interagire si oui interagir sinon rien faire 
        }
    }
}

void Agent::_move(Map& map, std::mt19937& rng) { 

    std::uniform_int_distribution<int> distrib(0, 4);

    int move = distrib(rng);
    int newX = config.x, newY = config.y;
    if (move == 0) newY--;
    else if (move == 1) newX++;
    else if (move == 2) newY++;
    else if (move == 3) newX--;
    
    if (map.isValidPosition(newX, newY) && map.getCell(newX, newY) != CellType::WATER) {
        config.x = newX;
        config.y = newY;
    }
}


// Retourne le score social envers un agent. Retourne 0 si inconnu.
int Agent::getSocialScoreFor(unsigned int agent_id) const {
    auto it = social_memory.find(agent_id);
    if (it != social_memory.end()) {
        return it->second;
    }
    return 0; // Opinion neutre par défaut
}

// Met à jour le score social envers un agent.
void Agent::updateSocialScoreFor(unsigned int agent_id, int change) {
    // Si on ne connaît pas l'agent, on l'ajoute avec un score initial de 0.
    if (social_memory.find(agent_id) == social_memory.end()) {
        social_memory[agent_id] = 0;
    }
    // Applique le changement et s'assure que le score reste dans des limites (-20 à 20 par ex.)
    social_memory[agent_id] = std::max(-20, std::min(20, social_memory[agent_id] + change));
}

// Ajoute un montant à la satisfaction de l'agent.
void Agent::addSatisfaction(double amount) {
    config.satisfaction = std::min(100.0, config.satisfaction + amount);
}

// ACTION PRINCIPALE
void Agent::act(const std::vector<double>& decision_vector, Map& map, std::vector<Agent>& all_agents, bool is_day, std::mt19937& rng) {

    int best_action_index = 0;
    for (size_t i = 1; i < decision_vector.size(); ++i) {
        if (decision_vector[i] > decision_vector[best_action_index]) {
            best_action_index = i;
        }
    }

    switch(best_action_index) {
        case 0: // Manger
            if (map.getCell(config.x, config.y) == CellType::APPLE) {
                map.setCell(config.x, config.y, CellType::EMPTY);
            } else if (map.getCell(config.x, config.y) == CellType::CHAMPIGNON_LUMINEUX) {
                map.setCell(config.x, config.y, CellType::FOREST);
            }
            config.energie = std::min(100.0, config.energie + 20);
            last_known_food_pos = {config.x, config.y};
            break;

        case 1: // Parler
            { 
                Agent* target = _findNearbyAgent(all_agents);
                
                if (target && config.energie > 15) {
                    
                    // --- Étape 1 : Calculer la force de la relation ---
                    int score_A_to_B = this->getSocialScoreFor(target->getId());
                    int score_B_to_A = target->getSocialScoreFor(this->getId());
                    // La force de la relation est la moyenne de leurs opinions mutuelles
                    double relationship_strength = (score_A_to_B + score_B_to_A) / 2.0;

                    // --- Étape 2 : Calculer le gain de satisfaction basé sur la relation ---
                    // Un score de 0 donne 5 points. Un score max (+20) donne 15 points. Un score min (-20) donne 0 point.
                    double satisfaction_gain = std::max(0.0, 5.0 + (relationship_strength / 2.0));
                    
                    // --- Étape 3 : Appliquer le gain aux DEUX agents ---
                    this->addSatisfaction(satisfaction_gain);
                    target->addSatisfaction(satisfaction_gain);

                    // --- Étape 4 : Mettre à jour la relation de manière synchronisée ---
                    std::uniform_int_distribution<int> affinity_dist(-1, 2); // Petite variation aléatoire
                    int affinity_change = affinity_dist(rng);

                    this->updateSocialScoreFor(target->getId(), affinity_change);
                    target->updateSocialScoreFor(this->getId(), affinity_change);

                    // --- Étape 5 : Échanger des informations (comme avant) ---
                    if (this->last_known_food_pos.has_value()) {
                        target->receiveFoodInfo(this->last_known_food_pos.value());
                    }
                }
                // Le coût de l'action est maintenant ici, pour s'appliquer même si l'interaction échoue
                config.energie -= 2.0;
            }
            break;

        case 2: // Dormir
            if (!is_day) {
                config.energie = std::min(100.0, config.energie + 15);
            } // On peut ajouter une pénaliter pour leur dire d'éviter de dormir le jour.
            break;

        case 3: // Interagir avec objet (livre)
            _interact(map);
            break;

        default: // Bouger
            _move(map, rng);
            break;
    }

    // Coût de la vie (appliqué à chaque tour, quelle que soit l'action)
    if(is_day) {
        config.energie = std::max(0.0, config.energie - 1.0);
    } else { 
        config.energie = std::max(0.0, config.energie - 1.5);
    }
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
    child.brain.setWeights(childBrain.getWeights());
    return child;
}

void Agent::logSocialMemory(int current_day, std::ofstream& logfile) const {
    if (social_memory.empty()) return;

    for (const auto& pair : social_memory) {
        unsigned int target_id = pair.first;
        int score = pair.second;
        logfile << current_day << ","
         << this->getId() << "," 
         << target_id << "," 
         << score << "\n";
    }
}