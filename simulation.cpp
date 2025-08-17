#include "simulation.h" 
#include <iostream>
#include <vector>
#include <algorithm> // Pour std::sort
#include <unistd.h> 


Simulation::Simulation(int map_width, int map_height, int num_agents) 
    : map(map_width, map_height), day(0), rng(std::random_device{}()) {
    map.generateRandomWorld();
    int input_size = 52;
    int hidden_size = 8;
    for (int i = 0; i < num_agents; ++i) {
        agents.emplace_back("Agent" + std::to_string(i), i, 5 + i, 5, input_size, hidden_size);
    }
    logfile.open("simulation_log.csv"); // Ouvre (ou crée) le fichier
    if (logfile.is_open()) {
        // Écrit l'en-tête du fichier CSV
        logfile << "jour,population,fitness_moyen,energie_moyenne,satisfaction_moyenne\n";
    }
}

void Simulation::evolvePopulation() {
    if (agents.empty()) return;

    const int current_pop_size = (int)agents.size();

    // --- Calcul de la fitness moyenne (pour mutation adaptative) ---
    double avg_fitness = 0.0;
    for (const auto &a : agents) avg_fitness += a.getFitness();
    avg_fitness /= current_pop_size;

    // variable statique interne pour mémoriser la dernière meilleure moyenne
    static double prev_avg_fitness = -1e300;
    static int stagnant_generations = 0;

    if (prev_avg_fitness < -1e200) { // première exécution
        prev_avg_fitness = avg_fitness;
        stagnant_generations = 0;
    } else {
        if (avg_fitness <= prev_avg_fitness + 1e-6) {
            stagnant_generations++;
        } else {
            stagnant_generations = 0;
            prev_avg_fitness = avg_fitness;
        }
    }

    // mutation adaptative
    double base_mutation_rate = 0.05; // mutation par défaut
    double mutation_rate = base_mutation_rate;
    if (stagnant_generations >= 5) {
        mutation_rate = 0.25; // secouer la population si stagnation
    }

    // --- paramètres de la sélection ---
    int elite_count = std::max(1, current_pop_size / 10);       // top 10%
    int random_survivors = std::max(1, current_pop_size / 20); // ~5%
    int newcomers_count = std::max(1, current_pop_size / 10);  // 10% newcomers

    // distributions utiles
    std::uniform_int_distribution<int> dist_idx(0, current_pop_size - 1);
    std::uniform_int_distribution<int> dist_x(0, map.getWidth() - 1);
    std::uniform_int_distribution<int> dist_y(0, map.getHeight() - 1);
    std::uniform_real_distribution<double> dist01(0.0, 1.0);

    // --- Trier par fitness décroissante ---
    std::sort(agents.begin(), agents.end(), [](const Agent& a, const Agent& b) {
        return a.getFitness() > b.getFitness();
    });

    std::vector<Agent> next_generation;
    next_generation.reserve(current_pop_size);

    // 1) Garder les élites (copies directes)
    for (int i = 0; i < elite_count && (int)next_generation.size() < current_pop_size; ++i) {
        next_generation.push_back(agents[i]);
    }

    // 2) Garder quelques survivants aléatoires (préserver diversité)
    for (int i = 0; i < random_survivors && (int)next_generation.size() < current_pop_size; ++i) {
        int idx = dist_idx(rng);
        next_generation.push_back(agents[idx]);
    }

    // 3) Immigration mixte : moitié totalement aléatoires, moitié clones mutés d'agents moyens
    int half_new = newcomers_count / 2;
    for (int i = 0; i < half_new && (int)next_generation.size() < current_pop_size; ++i) {
        // newcomer totalement aléatoire (nouveau cerveau, position aléatoire)
        int nx = dist_x(rng), ny = dist_y(rng);
        unsigned int newId = static_cast<unsigned int>(day * 10000 + next_generation.size());
        // brain sizes taken from an existing agent (safe because agents non-empty)
        int in_size = agents[0].getBrain().getInputSize();
        int hid_size = agents[0].getBrain().getHiddenSize();
        next_generation.emplace_back("Newcomer_rand_" + std::to_string(newId), newId, nx, ny, in_size, hid_size);
    }
    for (int i = 0; i < newcomers_count - half_new && (int)next_generation.size() < current_pop_size; ++i) {
        // newcomer = clone d'un agent moyen + forte mutation (exploration guidée)
        int idx = dist_idx(rng);
        Agent clone = agents[idx];
        clone.mutateBrain(0.35); // mutation forte
        // repositionne aléatoirement
        // (on suppose Agent a setters via constructeur copie; sinon on reconstruit)
        next_generation.push_back(clone);
    }

    // --- fonction de sélection de parents : tournoi (k = 3) ---
    auto tournament_select = [&](int k = 3) -> const Agent& {
        int best_idx = dist_idx(rng);
        double best_fit = agents[best_idx].getFitness();
        for (int t = 1; t < k; ++t) {
            int idx = dist_idx(rng);
            if (agents[idx].getFitness() > best_fit) {
                best_idx = idx;
                best_fit = agents[idx].getFitness();
            }
        }
        return agents[best_idx];
    };

    // 4) Reproduction jusqu'à remplissage
    while ((int)next_generation.size() < current_pop_size) {
        const Agent& parent1 = tournament_select(3);
        const Agent& parent2 = tournament_select(3);

        int startX = dist_x(rng);
        int startY = dist_y(rng);

        unsigned int childId = static_cast<unsigned int>(day * 10000 + next_generation.size());
        Agent child = parent1.breedWith(parent2, "Agent_child_" + std::to_string(childId), childId, startX, startY);

        // mutation probabiliste basée sur mutation_rate (déterministe si dist01)
        if (dist01(rng) < mutation_rate) {
            // mutation proportionnelle (valeur aléatoire autour de mutation_rate)
            double mrate = mutation_rate * (0.5 + dist01(rng)); 
            child.mutateBrain(mrate);
        }

        next_generation.push_back(child);
    }

    // Remplacer population
    agents = std::move(next_generation);
}
// le but de cette fonction est d'avoir une simulation la plus rapide possible. En contre partie de cette vitesse, on perd : l'affichage et moins de data. 
void Simulation::fast_run() {
    while (day < 50000) {

        for (auto& agent : agents) {
            std::vector<double> perception = agent.perceive(map, agents, is_day);
            std::vector<double> decision = agent.think(perception);
            agent.act(decision, map, agents, is_day, rng);
        }
        
        map.updateWorld(is_day);

        time_of_day++;
        if (time_of_day >= DAY_DURATION * 2) {
            time_of_day = 0;
            day++;
            logDailyStats();
        }
        is_day = (time_of_day < DAY_DURATION);

        if((day %5) == 0 && day != 0) {
            evolvePopulation();
        }
    }

    logfile.close();
    
}
// simulation avec affichage 
void Simulation::run() {
    while (day < 25) {
        std::cout << "----------- JOUR " << day << " -----------" << std::endl;

        
        
        for (auto& agent : agents) {
            std::vector<double> perception = agent.perceive(map, agents, is_day);
            std::vector<double> decision = agent.think(perception);
            agent.act(decision, map, agents, is_day, rng);
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

// gestion des données dans un fichier csv

void Simulation::logDailyStats() {
    if (!logfile.is_open() || agents.empty()) {
        return; // Ne rien faire si le fichier n'est pas ouvert ou s'il n'y a pas d'agents
    }

    // --- CALCUL DES STATISTIQUES ---
    double total_fitness = 0.0;
    double total_energie = 0.0;
    double total_satisfaction = 0.0;

    for (const auto& agent : agents) {
        total_fitness += agent.getFitness();
        total_energie += agent.getEnergie(); // Ajoute un getter pour energie
        total_satisfaction += agent.getSatisfaction(); // Ajoute un getter pour satisfaction
    }

    int taille_de_la_pop = agents.size();
    double average_fitness = total_fitness / taille_de_la_pop;
    double average_energie = total_energie / taille_de_la_pop;
    double average_satisfaction = total_satisfaction / taille_de_la_pop;

    // --- ÉCRITURE DANS LE FICHIER ---
    logfile << day << ","
            << taille_de_la_pop << ","
            << average_fitness << ","
            << average_energie << ","
            << average_satisfaction << "\n";
}
