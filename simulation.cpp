#include "simulation.h" 
#include "SimulationConfig.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h> 
#include <set>

Simulation::Simulation(int map_width, int map_height, int num_agents) 
    : map(map_width, map_height), day(0) {
    
    std::random_device rd;
    rng.seed(rd());

    map.generateRandomWorld(rng);

    // Génération des agents à des positions valides
    std::uniform_int_distribution<int> dist_x(0, map.getWidth() - 1);
    std::uniform_int_distribution<int> dist_y(0, map.getHeight() - 1);
    std::set<std::pair<int, int>> occupied_positions;

    // Utiliser INPUT_SIZE et HIDDEN_SIZE du config
    int input_size = INPUT_SIZE;    // 11 entrées
    int hidden_size = HIDDEN_SIZE;  // 8 neurones cachés

    for (int i = 0; i < num_agents; ++i) {
        int x, y;
        do {
            x = dist_x(rng);
            y = dist_y(rng);
        } while (map.getCell(x, y) == CellType::WATER || occupied_positions.count({x, y}));
        
        occupied_positions.insert({x, y});
        agents.emplace_back("Agent" + std::to_string(i), i, x, y, input_size, hidden_size);
    }
    
    // Ouvrir les fichiers de log
    logfile.open("simulation_log.csv"); 
    if (logfile.is_open()) {
        logfile << "jour,fitness_moyen,energie_moyenne,satisfaction_moyenne,population\n";  
    }
    
    social_logfile.open("social_log.csv");
    if (social_logfile.is_open()) {
        social_logfile << "jour,agent_source_id,agent_cible_id,score_relation\n";
    }

}

void Simulation::evolvePopulation() {
    if (agents.empty()) {
        return;
    }

    const int current_pop_size = (int)agents.size();

    // Calcul de la fitness moyenne (pour mutation adaptative) 
    double avg_fitness = 0.0;
    for (const auto &a : agents) avg_fitness += a.getFitness();
    avg_fitness /= current_pop_size;

    static double prev_avg_fitness = -1e300;
    static int stagnant_generations = 0;

    if (avg_fitness <= prev_avg_fitness + 1e-6) {
        stagnant_generations++;
    } else {
        stagnant_generations = 0;
        prev_avg_fitness = avg_fitness;
    }

    
    double mutation_rate = (stagnant_generations >= STAGNATION_THRESHOLD) 
                          ? MUTATION_RATE_HIGH 
                          : MUTATION_RATE_NORMAL;

    // Paramètres de sélection basés sur les ratios du config
    int elite_count = std::max(1, (int)(current_pop_size * RATIO_ELITE));
    int random_survivors = std::max(1, (int)(current_pop_size * RATIO_RANDOM_SURVIVORS));
    int newcomers_count = std::max(1, (int)(current_pop_size * RATIO_NEWCOMERS));

    // Distributions utiles
    std::uniform_int_distribution<int> dist_idx(0, current_pop_size - 1);
    std::uniform_int_distribution<int> dist_x(0, map.getWidth() - 1);
    std::uniform_int_distribution<int> dist_y(0, map.getHeight() - 1);
    std::uniform_real_distribution<double> dist01(0.0, 1.0);

    // Tri par fitness décroissante
    std::sort(agents.begin(), agents.end(), [](const Agent& a, const Agent& b) {
        return a.getFitness() > b.getFitness();
    });

    std::vector<Agent> next_generation;
    next_generation.reserve(current_pop_size);

    // 1) Élitisme : Conserver les meilleurs
    for (int i = 0; i < elite_count; ++i) {
        next_generation.push_back(agents[i]);
    }

    // 2) Survivants aléatoires (diversité)
    for (int i = 0; i < random_survivors; ++i) {
        next_generation.push_back(agents[dist_idx(rng)]);
    }

    // 3) NOUVEAUTÉ : Immigrants (nouveaux cerveaux aléatoires)
    for (int i = 0; i < newcomers_count; ++i) {
        int startX, startY;
        do {
            startX = dist_x(rng);
            startY = dist_y(rng);
        } while (map.getCell(startX, startY) == CellType::WATER);
        
        unsigned int newId = static_cast<unsigned int>(day * 10000 + 9000 + i);
        Agent newcomer("Immigrant_" + std::to_string(newId), newId, startX, startY, INPUT_SIZE, HIDDEN_SIZE);
        next_generation.push_back(newcomer);
    }

    // 4) Sélection par tournoi
    auto tournament_select = [&](int k = 3) -> const Agent& {
        int best_idx = dist_idx(rng);
        for (int t = 1; t < k; ++t) {
            int idx = dist_idx(rng);
            if (agents[idx].getFitness() > agents[best_idx].getFitness()) {
                best_idx = idx;
            }
        }
        return agents[best_idx];
    };

    // 5) Reproduction jusqu'à remplissage
    while ((int)next_generation.size() < current_pop_size) {
        const Agent& parent1 = tournament_select();
        const Agent& parent2 = tournament_select();

        int startX, startY;
        do {
            startX = dist_x(rng);
            startY = dist_y(rng);
        } while (map.getCell(startX, startY) == CellType::WATER);

        unsigned int childId = static_cast<unsigned int>(day * 10000 + next_generation.size());
        Agent child = parent1.breedWith(parent2, "Agent_gen" + std::to_string(day/EVOLUTION_PERIOD) + "_" + std::to_string(childId), childId, startX, startY);

        //  Mutation sur tous les enfants (pas de condition aléatoire)
        child.mutateBrain(mutation_rate);

        next_generation.push_back(child);
    }

    agents = std::move(next_generation);

    
    if (stagnant_generations >= STAGNATION_THRESHOLD) {
        std::cout << "⚡ Génération " << day/EVOLUTION_PERIOD 
                  << " : Stagnation détectée → Mutation haute (" 
                  << (mutation_rate * 100) << "%)" << std::endl;
    }
}

void Simulation::fast_run() {
    int last_evolution_day = 0;
    int last_report_day = 0;
    
    std::cout << " Démarrage simulation rapide..." << std::endl;
    
    while(day < MAX_DAYS) {
        
       
        if (agents.empty()) {
            std::cerr << "\n EXTINCTION au jour " << day << " !" << std::endl;
            break;
        }

        // Tour de jeu pour chaque agent
        for (auto& agent : agents) {
            std::vector<double> perception = agent.perceive(map, agents, is_day);
            std::vector<double> decision = agent.think(perception);
            agent.act(decision, map, agents, is_day, rng);
        }

        // Mise à jour du monde
        map.updateWorld(is_day, rng);
        time_of_day++;
        
        if (time_of_day >= DAY_DURATION * 2) {
            time_of_day = 0;
            day++;
            logDailyStats();

            
            if (day - last_report_day >= 1000) {
                last_report_day = day;
            }

            // Log social tous les 10 jours
            if (day % 10 == 0) {
                logSocialNetworkSnapshot();
            }

            // Évolution tous les EVOLUTION_PERIOD jours
            if(day % EVOLUTION_PERIOD == 0 && day != last_evolution_day) {
                evolvePopulation();
                last_evolution_day = day;
            }
        }
        is_day = (time_of_day < DAY_DURATION);
    }

    
    if (!agents.empty()) {
        auto best_it = std::max_element(agents.begin(), agents.end(), 
            [](const Agent& a, const Agent& b) { return a.getFitness() < b.getFitness(); });
        
        std::cout << "\n Sauvegarde du meilleur agent..." << std::endl;
        std::cout << "   Fitness: " << best_it->getFitness() << std::endl;
        std::cout << "   Énergie: " << best_it->getEnergie() << std::endl;
        std::cout << "   Satisfaction: " << best_it->getSatisfaction() << std::endl;
        
        best_it->saveBrain("best_brain.txt");
    }

    logfile.close();
    social_logfile.close();
    
    std::cout << "\n Simulation terminée au jour " << day << std::endl;
    displayFinalStats();
}

void Simulation::run() {
    int last_evolution_day = 0;
    
    std::cout << " Démarrage simulation avec affichage..." << std::endl;
    
    while (day < 500) {
        
        
        if (agents.empty()) {
            std::cerr << "\n💀 EXTINCTION au jour " << day << " !" << std::endl;
            break;
        }
        
        for (auto& agent : agents) {
            std::vector<double> perception = agent.perceive(map, agents, is_day);
            std::vector<double> decision = agent.think(perception);
            agent.act(decision, map, agents, is_day, rng);
        }
        
        map.updateWorld(is_day, rng);
        map.display(agents);
        
       
        std::cout << "Jour " << day << " | " << (is_day ? "☀️ JOUR" : "🌙 NUIT") 
                  << " | Pop: " << agents.size() << " | Tour: " << time_of_day << "/" << (DAY_DURATION*2) << std::endl;
        
        usleep(100000); // 0.1 secondes

        time_of_day++;
        if (time_of_day >= DAY_DURATION * 2) {
            time_of_day = 0;
            day++;
            logDailyStats();

            
            if (day % 10 == 0) {
                logSocialNetworkSnapshot();
            }
            
            if(day % EVOLUTION_PERIOD == 0 && day != last_evolution_day) {
                evolvePopulation();
                last_evolution_day = day;
                usleep(1000000); // Pause d'une seconde
            }
        }
        is_day = (time_of_day < DAY_DURATION);
    }

    
    if (!agents.empty()) {
        auto best_it = std::max_element(agents.begin(), agents.end(), 
            [](const Agent& a, const Agent& b) { return a.getFitness() < b.getFitness(); });
        
        std::cout << "\n Sauvegarde du meilleur agent..." << std::endl;
        best_it->saveBrain("best_brain.txt");
    }

    logfile.close();
    social_logfile.close();
    
    std::cout << "\n Simulation terminée" << std::endl;
    displayFinalStats();
}

void Simulation::logDailyStats() {
    if (!logfile.is_open() || agents.empty()) {
        return;
    }

    double total_fitness = 0.0;
    double total_energie = 0.0;
    double total_satisfaction = 0.0;

    for (const auto& agent : agents) {
        total_fitness += agent.getFitness();
        total_energie += agent.getEnergie(); 
        total_satisfaction += agent.getSatisfaction();
    }

    int pop_size = agents.size();
    double average_fitness = total_fitness / pop_size;
    double average_energie = total_energie / pop_size;
    double average_satisfaction = total_satisfaction / pop_size;

   
    logfile << day << ","
            << average_fitness << ","
            << average_energie << ","
            << average_satisfaction << ","
            << pop_size << "\n";
}

void Simulation::logSocialNetworkSnapshot() {
    if (!social_logfile.is_open()) return;

    for (const auto& agent : agents) {
        agent.logSocialMemory(day, social_logfile);
    }
}

void Simulation::displayFinalStats() {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "       STATISTIQUES FINALES" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "Jours simulés: " << day << std::endl;
    std::cout << "Population finale: " << agents.size() << " agents" << std::endl;
    
    if (!agents.empty()) {
        double avg_fitness = 0.0;
        double max_fitness = 0.0;
        
        for (const auto& agent : agents) {
            double f = agent.getFitness();
            avg_fitness += f;
            if (f > max_fitness) max_fitness = f;
        }
        avg_fitness /= agents.size();
        
        std::cout << "Fitness moyenne: " << avg_fitness << std::endl;
        std::cout << "Fitness maximale: " << max_fitness << std::endl;
        std::cout << "Nombre de générations: " << day / EVOLUTION_PERIOD << std::endl;
    }
    
    std::cout << "\nFichiers générés:" << std::endl;
    std::cout << "  📄 simulation_log.csv" << std::endl;
    std::cout << "  📄 social_log.csv" << std::endl;
    std::cout << "  📄 best_brain.txt" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}