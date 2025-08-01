#include "Agent.h"

// Constructeur pour nouvel agent (cerveau aléatoire)
Agent::Agent(const std::string& name, unsigned int id, unsigned int personnalite,
             double vitesse, double energie, double satisfaction,
             int input_size, int hidden_size)
    : name(name), id(id), personnalite(personnalite),
      vitesse(vitesse), energie(energie), satisfaction(satisfaction),
      brain(input_size, hidden_size) {}

// Constructeur pour breeding (cerveau existant)
Agent::Agent(const std::string& name, unsigned int id, unsigned int personnalite,
             double vitesse, double energie, double satisfaction,
             const LSTM& brain)
    : name(name), id(id), personnalite(personnalite),
      vitesse(vitesse), energie(energie), satisfaction(satisfaction),
      brain(brain) {}

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

    // Création du cerveau enfant
    LSTM childBrain = brain.breed(partner.brain);

    return Agent(childName, childId, childPersonnalite, childVitesse, 
                childEnergie, childSatisfaction, childBrain);
}