#include "Agent.h"

// Constructeur pour un nouvel agent
Agent::Agent(const std::string& name, unsigned int id, int startX, int startY, int input_size, int hidden_size)
    : brain(input_size, hidden_size),
      config{name, id, 100.0, 100.0, startX, startY, '@', UP} // Initialisation de la struct
{}


// L'agent observe son environnement et le transforme en vecteur pour son cerveau
std::vector<double> Agent::perceive(const Map& map, const std::vector<Agent>& all_agents) {
    std::vector<double> perception_vector;
    
    perception_vector.push_back(config.energie / 100.0);
    perception_vector.push_back(config.satisfaction / 100.0);

    for (int i = -4; i <= 4; ++i) {
        for (int j = -4; j <= 4; ++j) {
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
                    break;
                }
            }
            perception_vector.push_back(agent_found ? 1.0 : 0.0);
        }
    }
    return perception_vector; 
}

// L'agent utilise son cerveau pour prendre une décision
std::vector<double> Agent::think(const std::vector<double>& perception_vector) {
    return brain.forward(perception_vector);
}
// Divisé act en sous fonction privée 

void Agent::_move(Map& map) { 
    int move = rand() % 5;
    int newX = config.x, newY = config.y;
    if (move == 0) newY--;
    else if (move == 1) newX++;
    else if (move == 2) newY++;
    else if (move == 3) newX--;
    
    if (map.isValidPosition(newX, newY) && map.getCell(newX, newY) != WATER) {
        config.x = newX;
        config.y = newY;
    }
}

// L'agent exécute une action en fonction de la décision de son cerveau
void Agent::act(const std::vector<double>& decision_vector, const Map& map) {
    // La sortie du LSTM est un vecteur de probabilités. Trouvons la meilleure action.
    int best_action_index = 0;
    for (size_t i = 1; i < decision_vector.size(); ++i) {
        if (decision_vector[i] > decision_vector[best_action_index]) {
            best_action_index = i;
        }
    }

    _move(map);

    config.energie -= 0.5; // Chaque action coûte de l'énergie
}

double Agent::getFitness() const {
    
    return config.energie * 0.2 + config.satisfaction 0.5;
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