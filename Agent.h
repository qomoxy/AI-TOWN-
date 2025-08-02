#pragma once
#include "LSTM.h"
#include "monde.h"
#include <string>
#include <vector>
#include <utility> // Pour std::pair

class Agent {
private:
    std::string name;
    unsigned int id;
    LSTM brain;

    // Stats
    double energie;
    double satisfaction;

    // Position & Apparence
    int x, y;
    char symbol;

public:
    // Constructeur 
    Agent(const std::string& name, unsigned int id, int startX, int startY, int input_size, int hidden_size);

    // Getters 
    std::string getName() const { return name; }
    std::pair<int, int> getPosition() const { return {x, y}; }
    char getSymbol() const { return symbol; }
    const LSTM& getBrain() const { return brain; } // Pour le breeding

    // Logique de l'agent
    std::vector<double> perceive(const Map& map, const std::vector<Agent>& all_agents);
    std::vector<double> think(const std::vector<double>& perception_vector);
    void act(const std::vector<double>& decision_vector, Map& map); 

    // Évolution
    void mutateBrain(double mutationRate);
    Agent breedWith(const Agent& partner, const std::string& childName, unsigned int childId, int startX, int startY) const;
};