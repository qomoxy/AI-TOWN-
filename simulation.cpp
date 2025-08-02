#include <iostream>
#include <vector>
#include "Agent.h"
#include "monde.h"
#include <unistd.h> // Pour sleep()

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
        // Le vecteur de perception aura 2 (infos agent) + 9*9 (vision) * 2 (type+agent) = 164 entrées
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
        while (day < 100) { // La simulation tourne pour 100 jours
            std::cout << "----------- JOUR " << day << " -----------" << std::endl;
            
            // Boucle de tour pour chaque agent
            for (auto& agent : agents) {
                // 1. PERCEIVE
                std::vector<double> perception = agent.perceive(map, agents);

                // 2. THINK
                std::vector<double> decision = agent.think(perception);
                
                // 3. ACT
                agent.act(decision, map);
            }
            
            // Affiche l'état du monde
            map.display(agents);

            // Fais une pause pour qu'on puisse voir ce qui se passe
            usleep(200000); // 0.2 secondes

            day++;
            // Ici, tu pourras ajouter la logique de reproduction / sélection
        }
    }
};

// Il faudra aussi ajouter les définitions des fonctions de Map dans un fichier monde.cpp
// Et un main.cpp pour lancer la simulation.
// int main() {
//     Simulation sim(30, 20, 5);
//     sim.run();
//     return 0;
// }