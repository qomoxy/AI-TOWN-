#include <iostream>
#include <vector>
#include "Agent.h"
#include "monde.h"
#include <unistd.h> 

class Simulation {
public:
    Map map;
    std::vector<Agent> agents;
    int day;

    Simulation(int map_width, int map_height, int num_agents) 
        : map(map_width, map_height), day(0) {
        
        // Crée un monde avec des obstacles
        map.generateRandomWorld();

        // Crée les agents initiaux
        int input_size = 2 + 81 * 2;
        int hidden_size = 30; // Taille du "cerveau"
        for (int i = 0; i < num_agents; ++i) {
            agents.emplace_back("Agent" + std::to_string(i), i, 5 + i, 5, input_size, hidden_size);
        }
    }

    int getDay() {
        return day;
    }

    void run() {
        while (day < 100) {
            std::cout << "----------- JOUR " << day << " -----------" << std::endl;
            
            
            for (auto& agent : agents) {
                
                std::vector<double> perception = agent.perceive(map, agents);

                
                std::vector<double> decision = agent.think(perception);
                
                
                agent.act(decision, map);
            }
            
            
            map.display(agents);

            
            usleep(200000); // 0.2 secondes

            evolvePopulation();
            day++;
            
        }

        int fitness_agents[num_agents];

        for(auto& agent : agents ) { 
           fitness_agents[agent.config.id] = agent.getFitness();
        }


    }
};

void Simulation::evolvePopulation() {
    if(agent.empty()) return;

    std::sort(agent.begin(), agent.end(), [](const Agent& a, const Agent& b) { 
        return a.getFitness() > b.getFitness();
    });

    std::vector<Agent> reproduction_pool;
    int elite_count = agent.size / 8;
    if(elite_count = 0) elite_count = 1;

    for(int i =0; i < elite_count; ++i) { 
        reproduction_pool.push_back(agent[i]);
    };

    std::vector<Agent> next_generation;
    int current_pop_size = agents.size();
    
    while (next_generation.size() < current_pop_size) {
        
        const Agent& parent1 = reproduction_pool[rand() % elite_count];
        const Agent& parent2 = reproduction_pool[rand() % elite_count];

        
        int startX = rand() % map.getWidth();
        int startY = rand() % map.getHeight();
        Agent child = parent1.breedWith(parent2, "Agent" + std::to_string(next_generation.size()), next_generation.size(), startX, startY);

        
        if ((rand() % 100) < 10) {
            child.mutateBrain(0.05);
        }
        
        next_generation.push_back(child);
    }
    
    // Remplacer l'ancienne génération par la nouvelle
    agents = next_generation;

}