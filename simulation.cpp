#include "simulation.h" 
#include <iostream>
#include <vector>
#include <algorithm> // Pour std::sort
#include <unistd.h> 


Simulation::Simulation(int map_width, int map_height, int num_agents) 
    : map(map_width, map_height), day(0) {
    map.generateRandomWorld();
    int input_size = 2 + 81 * 2;
    int hidden_size = 30;
    for (int i = 0; i < num_agents; ++i) {
        agents.emplace_back("Agent" + std::to_string(i), i, 5 + i, 5, input_size, hidden_size);
    }
}

void Simulation::evolvePopulation() {
    if (agents.empty()) return;
    std::sort(agents.begin(), agents.end(), [](const Agent& a, const Agent& b) {
        return a.getFitness() > b.getFitness();
    });
    std::vector<Agent> reproduction_pool;
    int elite_count = agents.size() / 8;
    if(elite_count == 0) elite_count = 1;

    for(int i =0; i < elite_count; ++i) { 
        reproduction_pool.push_back(agents[i]);
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

void Simulation::run() {
    while (day < 100) {
        std::cout << "----------- JOUR " << day << " -----------" << std::endl;

        time_of_day++;
        if (time_of_day >= DAY_DURATION * 2) {
            time_of_day = 0;
            day++;
        }
        is_day = (time_of_day < DAY_DURATION);
        
        for (auto& agent : agents) {
            std::vector<double> perception = agent.perceive(map, agents, is_day);
            std::vector<double> decision = agent.think(perception);
            agent.act(decision, map, agents, is_day);
        }
        
        map.updateWorld(is_day);
        map.display(agents);
        usleep(200000);

        evolvePopulation();
    }
    
}