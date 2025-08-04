#pragma once
#include "LSTM.h"
#include "monde.h"
#include <string>
#include <vector>
#include <utility> 
#include <optional>

struct AgentConfig { 
    std::string name;
    unsigned int id;
    
    // Stats
    double energie;
    double satisfaction;

    // Position & Apparence
    int x, y;
    char symbol;
};

class Agent {
private:

    LSTM brain;
    
    AgentConfig config;

    std::map<unsigned int, int> social_memory;
    std::optional<std::pair<int, int>> last_known_food_pos;
    void _eat(int nutrition_score);
    void _move(Map& map);
    void _interact(Map& map);
    Agent* _findNearbyAgent(std::vector<Agent>& all_agents);

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
    int getId() const {return config.id; }


    // Logique de l'agent
    std::vector<double> perceive(const Map& map, const std::vector<Agent>& all_agents, bool is_day);
    std::vector<double> think(const std::vector<double>& perception_vector);
    void act(const std::vector<double>& decision_vector, Map& map, std::vector<Agent>& all_agents, bool is_day); 
    void receiveFoodInfo(std::pair<int, int> pos);

    // Évolution
    double getFitness() const;
    void mutateBrain(double mutationRate);
    Agent breedWith(const Agent& partner, const std::string& childName, unsigned int childId, int startX, int startY) const;
};