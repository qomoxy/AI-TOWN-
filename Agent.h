#pragma once
#include "LSTM.h"
#include "monde.h"
#include "SimulationConfig.h"
#include <string>
#include <vector>
#include <utility> 
#include <optional>
#include <fstream>

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
    double fitness = 0.0;

    std::map<unsigned int, int> social_memory;
    std::optional<std::pair<int, int>> last_known_food_pos;

    // Fonctions d'action privées pour une meilleure lisibilité
    void _eat(Map& map);
    void _move(Map& map, std::mt19937& rng);
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
    unsigned int getId() const {return config.id; }
    double getEnergie() const {return config.energie; }
    double getSatisfaction() const {return config.satisfaction; }
    int getSocialMemorySize() const {return social_memory.size(); }
    void setBrain(const LSTM& new_brain) { this->brain = new_brain; }

    double getFitness() const { 
        return (config.energie + 1.0) * (config.satisfaction + 1.0); 
    }

    int getSocialScoreFor(unsigned int agent_id) const;
    void updateSocialScoreFor(unsigned int agent_id, int change);
    void addSatisfaction(double amount);


    // Logique de l'agent
    std::vector<double> perceive(const Map& map, const std::vector<Agent>& all_agents, bool is_day);
    std::vector<double> think(const std::vector<double>& perception_vector);
    void act(const std::vector<double>& decision_vector, Map& map, std::vector<Agent>& all_agents, bool is_day, std::mt19937& rng); 
    void receiveFoodInfo(std::pair<int, int> pos);
    void logSocialMemory(int current_day, std::ofstream& logfile) const;

    // Évolution
    void mutateBrain(double mutationRate);
    Agent breedWith(const Agent& partner, const std::string& childName, unsigned int childId, int startX, int startY) const;

    void saveBrain(const std::string& filename) const {
    brain.saveBrain(filename);
    }
};

