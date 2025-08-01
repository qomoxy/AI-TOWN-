#include "Agent.h"
#include "monde.cpp"

// Constructeur pour nouvel agent (cerveau aléatoire)
Agent::Agent(const std::string& name, unsigned int id, unsigned int personnalite,
             double vitesse, double energie, double satisfaction,
             int input_size, int hidden_size, int x, int y, char symbol, Direction direction)
    : name(name), id(id), personnalite(personnalite),
      vitesse(vitesse), energie(energie), satisfaction(satisfaction),
      brain(input_size, hidden_size), x(x), y(y), symbol(symbol) {}

// Constructeur pour breeding (cerveau existant)
Agent::Agent(const std::string& name, unsigned int id, unsigned int personnalite,
             double vitesse, double energie, double satisfaction,
             const LSTM& brain, int x, int y, Direction direction, char symbol)
    : name(name), id(id), personnalite(personnalite),
      vitesse(vitesse), energie(energie), satisfaction(satisfaction),
      brain(brain), x(x), y(y), direction(direction), symbol(symbol) {}

// Propagation avant du cerveau
std::vector<double> Agent::think(const std::vector<double>& input) {
    return brain.forward(input);
}

// Mutation du cerveau
void Agent::mutateBrain(double mutationRate) {
    brain.mutate(mutationRate);
}

// Reproduction avec un autre agent
Agent Agent::breedWith(const Agent& partner, const std::string& childName, unsigned int childId) const {
    // Calcul des traits moyens
    unsigned int childPersonnalite = (personnalite + partner.personnalite) / 2;
    double childVitesse = (vitesse + partner.vitesse) / 2.0;
    double childEnergie = (energie + partner.energie) / 2.0;
    double childSatisfaction = (satisfaction + partner.satisfaction) / 2.0;

    int childX = (x + partner.x) / 2;
    int childY = (Y + partner.Y) / 2;


    // Création du cerveau enfant
    LSTM childBrain = brain.breed(partner.brain);

    return Agent(childName, childId, childPersonnalite, childVitesse, 
                childEnergie, childSatisfaction, childBrain, childX, childY);
}

void Agent::interactWith(Resource& resource) { // a refaire 
    if (resource.getType() == FOOD) {
        satisfaction += resource.getValue();
        energie += resource.getEnergyBoost();
    }
}

void Agent::act(const std::vector<double>& decision, Map& map) {
    // Exemple basé sur la sortie du LSTM
    if (decision[0] > 0.8) moveRandom(map);
    if (decision[1] > 0.7 && findFood(map)) eat();
    if (decision[2] > 0.6) communicate();
}