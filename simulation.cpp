#include "simulation.h" 
#include <iostream>
#include <vector>
#include <algorithm> // Pour std::sort
#include <unistd.h> 
#include <set>


Simulation::Simulation(int map_width, int map_height, int num_agents) 
    : map(map_width, map_height), day(0) {
    
    std::random_device rd;
    rng.seed(rd());

    map.generateRandomWorld(rng);

    
    std::uniform_int_distribution<int> dist_x(0, map.getWidth() - 1);
    std::uniform_int_distribution<int> dist_y(0, map.getHeight() - 1);
    std::set<std::pair<int, int>> occupied_positions; // Pour éviter les superpositions

    // Taille du vecteur de perception. DOIT correspondre à la logique dans Agent::perceive

    int input_size = Agent::PERCEPTION_SIZE;
    int hidden_size = 8; // La taille de la couche cachée/sortie du LSTM

    for (int i = 0; i < num_agents; ++i) {
        int x, y;
        do {
            x = dist_x(rng);
            y = dist_y(rng);
        } while (map.getCell(x, y) == CellType::WATER || occupied_positions.count({x, y}));
        
        occupied_positions.insert({x, y});
        agents.emplace_back("Agent" + std::to_string(i), i, x, y, input_size, hidden_size);
    }
    
    logfile.open("simulation_log.csv"); 
    if (logfile.is_open()) {
        logfile << "fitness_moyen,energie_moyenne,satisfaction_moyenne\n";
    }
    social_logfile.open("social_log.csv");
    if (social_logfile.is_open()) {
        social_logfile << "jour,agent_source_id,agent_cible_id,score_relation\n";
    }
}

void Simulation::evolvePopulation() {
    if (agents.empty()) {
        
        std::cout << "Extinction totale. Fin de la simulation ou repeuplement d'urgence." << std::endl;
        return; 
    }

    const int current_pop_size = (int)agents.size();

    //Calcul de la fitness moyenne (pour mutation adaptative) 
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

    double mutation_rate = (stagnant_generations >= STAGNATION_THRESHOLD) ? MUTATION_RATE_HIGH : MUTATION_RATE_NORMAL;

    // --- paramètres de la sélection ---
    int elite_count = std::max(1, current_pop_size * RATIO_ELITE);
    int random_survivors = std::max(1, current_pop_size * RATIO_RANDOM_SURVIVORS);
    int newcomers_count = std::max(1, current_pop_size * RATIO_NEWCOMERS);

    // distributions utiles
    std::uniform_int_distribution<int> dist_idx(0, current_pop_size - 1);
    std::uniform_int_distribution<int> dist_x(0, map.getWidth() - 1);
    std::uniform_int_distribution<int> dist_y(0, map.getHeight() - 1);
    std::uniform_real_distribution<double> dist01(0.0, 1.0);

    std::sort(agents.begin(), agents.end(), [](const Agent& a, const Agent& b) {
        return a.getFitness() > b.getFitness();
    });

    std::vector<Agent> next_generation;
    next_generation.reserve(current_pop_size);

    // 1) Élitisme
    for (int i = 0; i < elite_count; ++i) {
        next_generation.push_back(agents[i]);
    }

    // 2) Survivants aléatoires
    for (int i = 0; i < random_survivors; ++i) {
        next_generation.push_back(agents[dist_idx(rng)]);
    }

    // 3) Immigration (Nouveaux agents aléatoires)
    // On récupère les tailles du cerveau depuis un agent existant
    int input_size = agents[0].getBrain().getInputSize();
    int hidden_size = agents[0].getBrain().getHiddenSize();

    for (int i = 0; i < newcomers_count; ++i) {
        int startX, startY;
        // On s'assure qu'ils n'apparaissent pas dans l'eau
        do {
            startX = dist_x(rng);
            startY = dist_y(rng);
        } while (map.getCell(startX, startY) == CellType::WATER);

        unsigned int newId = static_cast<unsigned int>(day * 10000 + next_generation.size());
        
        // On crée un nouvel agent avec un cerveau initialisé aléatoirement
        next_generation.emplace_back(
            "Agent_New_" + std::to_string(newId), 
            newId, 
            startX, 
            startY, 
            input_size, 
            hidden_size
        );
    }

    // --- fonction de sélection de parents : tournoi (k = 3) ---
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

    // 4) Reproduction jusqu'à remplissage
    while ((int)next_generation.size() < current_pop_size) {
        const Agent& parent1 = tournament_select();
        const Agent& parent2 = tournament_select();

        int startX = dist_x(rng);
        int startY = dist_y(rng);

        unsigned int childId = static_cast<unsigned int>(day * 10000 + next_generation.size());
        Agent child = parent1.breedWith(parent2, "Agent_child_" + std::to_string(childId), childId, startX, startY);

        if (dist01(rng) < mutation_rate) {
            child.mutateBrain(mutation_rate * (0.5 + dist01(rng)));
        }

        next_generation.push_back(child);
    }
    agents = std::move(next_generation);
}


void Simulation::fast_run() {
    int last_evolution_day = 0;
    while(day < MAX_DAYS) {
        
        for (auto& agent : agents) {
            std::vector<double> perception = agent.perceive(map, agents, is_day);
            std::vector<double> decision = agent.think(perception);
            agent.act(decision, map, agents, is_day, rng);
        }

        agents.erase(
            std::remove_if(agents.begin(), agents.end(), 
            [](const Agent& a) { return a.getEnergie() <= 0; }),
        agents.end());    


        map.updateWorld(is_day, rng);
        time_of_day++;
        if (time_of_day >= DAY_DURATION * 2) {
            time_of_day = 0;
            day++;
            logDailyStats();

            if (day % 10 == 0) {
                logSocialNetworkSnapshot();
            }

            // L'évolution a lieu tous les 5 jours
            if(day % EVOLUTION_PERIOD == 0 && day != last_evolution_day) {
                evolvePopulation();
                last_evolution_day = day;
            }
        }
        is_day = (time_of_day < DAY_DURATION);
    }
    logfile.close();
    social_logfile.close();
}



void Simulation::run() {
    int last_evolution_day = 0;
    while (day < MAX_DAYS) {
        
        for (auto& agent : agents) {
            std::vector<double> perception = agent.perceive(map, agents, is_day);
            std::vector<double> decision = agent.think(perception);
            agent.act(decision, map, agents, is_day, rng);
        }

         agents.erase(
            std::remove_if(agents.begin(), agents.end(), 
            [](const Agent& a) { return a.getEnergie() <= 0; }),
         agents.end());  
        
        map.updateWorld(is_day, rng);
        map.display(agents);
        usleep(100000); // 0.1 secondes

        time_of_day++;
        if (time_of_day >= DAY_DURATION * 2) {
            time_of_day = 0;
            day++;
            std::cout << "----------- JOUR " << day << " -----------" << std::endl;
            logDailyStats();
            
            if (day % 10 == 0) {
                logSocialNetworkSnapshot();
            }
            
            // L'évolution a lieu tous les 5 jours
            if(day % EVOLUTION_PERIOD == 0 && day != last_evolution_day) {
                evolvePopulation();
                last_evolution_day = day; // Empêche de multiples évolutions le même jour
                 std::cout << "----------- EVOLUTION DE LA POPULATION -----------" << std::endl;
                 usleep(1000000); // Pause d'une seconde pour voir le message
            }
        }
        is_day = (time_of_day < DAY_DURATION);
    }

   if (!agents.empty()) {
        auto best_agent = std::max_element(agents.begin(), agents.end(), 
            [](const Agent& a, const Agent& b) { return a.getFitness() < b.getFitness(); });
        
        best_agent->saveBrain("best_brain.txt");
        std::cout << "Cerveau du meilleur agent (ID: " << best_agent->getId() << ") sauvegardé dans 'best_brain.txt'" << std::endl;
    }

    logfile.close();
    social_logfile.close();
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

    logfile << average_fitness << ","
            << average_energie << ","
            << average_satisfaction << "\n";
}

void Simulation::logSocialNetworkSnapshot() {
    if (!social_logfile.is_open()) return;

    for (const auto& agent : agents) {
        agent.logSocialMemory(day, social_logfile);
    }
}
