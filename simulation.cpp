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
    logfile.open("simulation_log.csv"); // Ouvre (ou crée) le fichier
    if (logfile.is_open()) {
        // Écrit l'en-tête du fichier CSV
        logfile << "jour,population,fitness_moyen,energie_moyenne,satisfaction_moyenne,interactions_sociales,taille_memoire_sociale_moyenne\n";
    }
}

void Simulation::evolvePopulation() {
    if (agents.empty()) return;

    // Trier par fitness cumulative
    std::sort(agents.begin(), agents.end(), [](const Agent& a, const Agent& b) {
        return a.getFitness() > b.getFitness();
    });

    std::vector<Agent> next_generation;
    int elite_count = std::max(1, (int)(agents.size() / 5)); // Top 20%
    
    // Garder les élites tels quels
    for (int i = 0; i < elite_count; i++) {
        next_generation.push_back(agents[i]);
    }

    // Reproduire jusqu'à remplir
    while (next_generation.size() < agents.size()) {
        const Agent& parent1 = agents[rand() % elite_count];
        const Agent& parent2 = agents[rand() % elite_count];

        int startX = rand() % map.getWidth();
        int startY = rand() % map.getHeight();

        Agent child = parent1.breedWith(parent2, "Agent" + std::to_string(next_generation.size()),
                                        next_generation.size(), startX, startY);

        // Mutation plus fréquente
        if ((rand() % 100) < 10) {
            child.mutateBrain(0.01);
        }

        next_generation.push_back(child);
    }

    agents = next_generation;
}


void Simulation::run() {
    while (day < 25) {
        std::cout << "----------- JOUR " << day << " -----------" << std::endl;

        
        
        for (auto& agent : agents) {
            std::vector<double> perception = agent.perceive(map, agents, is_day);
            std::vector<double> decision = agent.think(perception);
            agent.act(decision, map, agents, is_day);
        }
        
        map.updateWorld(is_day);
        map.display(agents);
        usleep(200000);
        logDailyStats();

        time_of_day++;
        if (time_of_day >= DAY_DURATION * 2) {
            time_of_day = 0;
            day++;
        }
        is_day = (time_of_day < DAY_DURATION);

        if((day %5) == 0 && day != 0) {
            evolvePopulation();
        }
    }

    logfile.close();
    
}

void Simulation::logDailyStats() {
    if (!logfile.is_open() || agents.empty()) {
        return; // Ne rien faire si le fichier n'est pas ouvert ou s'il n'y a pas d'agents
    }

    // --- CALCUL DES STATISTIQUES ---
    double total_fitness = 0.0;
    double total_energie = 0.0;
    double total_satisfaction = 0.0;
    int total_interactions = 0; // Tu devras ajouter un moyen de compter ça*
    double total_memory_size = 0.0;

    for (const auto& agent : agents) {
        total_fitness += agent.getFitness();
        total_energie += agent.getEnergie(); // Ajoute un getter pour energie
        total_satisfaction += agent.getSatisfaction(); // Ajoute un getter pour satisfaction
        total_memory_size += agent.getSocialMemorySize(); // Ajoute un getter pour la taille de la map social_memory
    }

    int taille_de_la_pop = agents.size();
    double average_fitness = total_fitness / taille_de_la_pop;
    double average_energie = total_energie / taille_de_la_pop;
    double average_satisfaction = total_satisfaction / taille_de_la_pop;
    double average_memory_size = total_memory_size / taille_de_la_pop;

    // --- ÉCRITURE DANS LE FICHIER ---
    logfile << day << ","
            << taille_de_la_pop << ","
            << average_fitness << ","
            << average_energie << ","
            << average_satisfaction << ","
            << total_interactions << "," // Remplace par ta variable
            << average_memory_size << "\n";
}