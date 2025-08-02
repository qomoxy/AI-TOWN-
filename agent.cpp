#include "Agent.h"

// Constructeur pour un nouvel agent
Agent::Agent(const std::string& name, unsigned int id, int x, int y, int input_size, int hidden_size)
    : name(name),
      id(id),
      personnalite(50), 
      vitesse(1.0),
      energie(100.0),
      satisfaction(100.0),
      brain(input_size, hidden_size),
      x(x),
      y(y),
      direction(UP), // Direction par défaut
      symbol('@') {}


// L'agent observe son environnement et le transforme en vecteur pour son cerveau
std::vector<double> Agent::perceive(const Map& map, const std::vector<Agent>& all_agents) {
    std::vector<double> perception_vector;
    
    // 1. Infos sur soi-même
    perception_vector.push_back(this->energie / 100.0); // Normalisé entre 0 et 1
    perception_vector.push_back(this->satisfaction / 100.0);

    // 2. Infos sur l'environnement (vision à 4 cases) (render distance)
    for (int i = -4; i <= 4; ++i) {
        for (int j = -4; j <= 4; ++j) {
            int check_x = this->x + j;
            int check_y = this->y + i;

            // Info sur le type de case
            if (!map.isValidPosition(check_x, check_y)) {
                perception_vector.push_back(-1.0); // Mur / Hors map
            } else {
                perception_vector.push_back(map.getCell(check_x, check_y) / 5.0); // Normalisé
            }

            // Info sur la présence d'un agent
            bool agent_found = false;
            for(const auto& other_agent : all_agents) {
                if (other_agent.getX() == check_x && other_agent.getY() == check_y) {
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

// L'agent exécute une action en fonction de la décision de son cerveau
void Agent::act(const std::vector<double>& decision_vector, const Map& map) {
    // La sortie du LSTM est un vecteur de probabilités. Trouvons la meilleure action.
    int best_action_index = 0;
    for (size_t i = 1; i < decision_vector.size(); ++i) {
        if (decision_vector[i] > decision_vector[best_action_index]) {
            best_action_index = i;
        }
    }

    // Traduire l'index en action (EXEMPLE SIMPLE)
    // 0: Haut, 1: Droite, 2: Bas, 3: Gauche, 4: Ne rien faire
    if (best_action_index <= 3) {
        this->direction = static_cast<Direction>(best_action_index);
        int newX = x, newY = y;
        switch(direction) {
            case UP: newY--; break;
            case RIGHT: newX++; break;
            case DOWN: newY++; break;
            case LEFT: newX--; break;
        }
        if (map.isValidPosition(newX, newY) && map.getCell(newX, newY) != WATER) {
            x = newX;
            y = newY;
        }
    }
    // else: ne rien faire

    this->energie -= 0.5; // Chaque action coûte de l'énergie
}


// Fonctions pour l'évolution
void Agent::mutateBrain(double mutationRate) {
    brain.mutate(mutationRate);
}

Agent Agent::breedWith(const Agent& partner, const std::string& childName, unsigned int childId) const {
    // Le cerveau de l'enfant est un croisement des cerveaux des parents
    LSTM childBrain = brain.breed(partner.getBrain());

    // Le nouvel agent est créé à une position aléatoire (à améliorer)
    Agent child(childName, childId, 5, 5, childBrain.input_size, childBrain.hidden_size); // Position de départ à définir
    
    // Les autres stats peuvent être moyennées
    // ...
    
    return child;
}