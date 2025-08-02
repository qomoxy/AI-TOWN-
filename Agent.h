#pragma once
#include "LSTM.h"
#include "monde.h"
#include <string>
#include <vector>
#include <utility> 

struct AgentConfig { 
    std::string name;
    unsigned int id;
    
    // Stats
    double energie;
    double satisfaction;

    // Position & Apparence
    int x, y;
    char symbol;
    Direction direction;
};

class Agent {
private:

    LSTM brain;
    
    AgentConfig config;

    void _eat(int nutrition_score);
    void _move(Map& map);
    void _interact(Map& map);


public:
    // Constructeur 
    Agent(const std::string& name, unsigned int id, int startX, int startY, int input_size, int hidden_size);

    // Getters 
    std::string getName() const { return config.name; }
    std::pair<int, int> getPosition() const { return {config.x, config.y}; }
    char getSymbol() const { return config.symbol; }
    const LSTM& getBrain() const { return brain; } // Pour le breeding
    int getX() const { return config.x; }
    int getY() const { return config.y; }

    // Logique de l'agent
    std::vector<double> perceive(const Map& map, const std::vector<Agent>& all_agents);
    std::vector<double> think(const std::vector<double>& perception_vector);
    void act(const std::vector<double>& decision_vector, Map& map); 

    // Évolution
    void mutateBrain(double mutationRate);
    Agent breedWith(const Agent& partner, const std::string& childName, unsigned int childId, int startX, int startY) const;
};