#include "simulation.h"
#include "SimulationConfig.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <set>

// ============================================================
// Constructeur
// ============================================================

Simulation::Simulation(int map_width, int map_height, int num_agents)
    : map(map_width, map_height), day(0) {

    std::random_device rd;
    rng.seed(rd());

    map.generateRandomWorld(rng);

    std::uniform_int_distribution<int> dist_x(0, map.getWidth()  - 1);
    std::uniform_int_distribution<int> dist_y(0, map.getHeight() - 1);
    std::set<std::pair<int, int>> occupied;

    const int MAX_PLACEMENT_TRIES = map_width * map_height * 4;

    for (int i = 0; i < num_agents; ++i) {
        int x, y;
        int tries = 0;
        // CORRECTIF : compteur de sécurité pour éviter la boucle infinie
        // si la carte ne contient pas assez de cases valides.
        do {
            x = dist_x(rng);
            y = dist_y(rng);
            ++tries;
            if (tries > MAX_PLACEMENT_TRIES) {
                std::cerr << "Avertissement : impossible de placer tous les agents "
                             "(carte trop petite ou trop d'eau). Arrêt à "
                          << i << " agents.\n";
                goto done_placing;
            }
        } while (map.getCell(x, y) == CellType::WATER || occupied.count({x, y}));

        occupied.insert({x, y});
        agents.emplace_back("Agent" + std::to_string(i), (unsigned)i,
                            x, y, INPUT_SIZE, HIDDEN_SIZE);
    }
    done_placing:;

    // Fichiers de log
    logfile.open("simulation_log.csv");
    if (logfile.is_open())
        logfile << "jour,fitness_moyen,energie_moyenne,satisfaction_moyenne,population\n";

    social_logfile.open("social_log.csv");
    if (social_logfile.is_open())
        social_logfile << "jour,agent_source_id,agent_cible_id,score_relation\n";
}

// ============================================================
// Helpers de placement sécurisé
// ============================================================

// CORRECTIF : fonction utilitaire pour trouver une case valide
// utilisée dans evolvePopulation() afin de ne pas dupliquer le pattern.
bool Simulation::findValidPosition(int& out_x, int& out_y) {
    std::uniform_int_distribution<int> dist_x(0, map.getWidth()  - 1);
    std::uniform_int_distribution<int> dist_y(0, map.getHeight() - 1);
    const int MAX_TRIES = map.getWidth() * map.getHeight() * 4;

    for (int t = 0; t < MAX_TRIES; ++t) {
        int x = dist_x(rng), y = dist_y(rng);
        if (map.getCell(x, y) != CellType::WATER) {
            out_x = x; out_y = y;
            return true;
        }
    }
    return false; // Carte quasi entièrement eau
}

// ============================================================
// Évolution de la population
// ============================================================

void Simulation::evolvePopulation() {
    if (agents.empty()) return;

    const int current_pop_size = (int)agents.size();

    // Fitness moyenne
    double avg_fitness = 0.0;
    for (const auto& a : agents) avg_fitness += a.getFitness();
    avg_fitness /= current_pop_size;

    // CORRECTIF : variables membres au lieu de static locaux,
    // ce qui permettrait de réinitialiser entre deux simulations.
    if (avg_fitness <= prev_avg_fitness + 1e-6)
        ++stagnant_generations;
    else {
        stagnant_generations = 0;
        prev_avg_fitness = avg_fitness;
    }

    double mutation_rate = (stagnant_generations >= STAGNATION_THRESHOLD)
                         ? MUTATION_RATE_HIGH
                         : MUTATION_RATE_NORMAL;

    // Paramètres de sélection
    int elite_count      = std::max(1, (int)(current_pop_size * RATIO_ELITE));
    int random_survivors = std::max(1, (int)(current_pop_size * RATIO_RANDOM_SURVIVORS));
    int newcomers_count  = std::max(1, (int)(current_pop_size * RATIO_NEWCOMERS));

    std::uniform_int_distribution<int> dist_idx(0, current_pop_size - 1);

    // Tri par fitness décroissante
    std::sort(agents.begin(), agents.end(),
              [](const Agent& a, const Agent& b) { return a.getFitness() > b.getFitness(); });

    std::vector<Agent> next_gen;
    next_gen.reserve(current_pop_size);

    // 1) Élitisme — les meilleurs conservent leur cerveau
    //    CORRECTIF : on les réinitialise à énergie/satisfaction de départ
    //    pour éviter un avantage injuste vis-à-vis des enfants naissants.
    for (int i = 0; i < elite_count; ++i) {
        int x, y;
        if (!findValidPosition(x, y)) { x = 0; y = 0; }
        Agent elite = agents[i]; // copie (conserve le cerveau)
        // Réinitialisation des stats uniquement (pas du cerveau ni de la mémoire sociale)
        // On crée un nouvel agent avec le même cerveau
        Agent reset_elite("Elite_" + std::to_string(i),
                          agents[i].getId(), x, y, INPUT_SIZE, HIDDEN_SIZE);
        reset_elite.setBrain(agents[i].getBrain());
        next_gen.push_back(reset_elite);
    }

    // 2) Survivants aléatoires (diversité génétique)
    for (int i = 0; i < random_survivors; ++i) {
        int idx = dist_idx(rng);
        int x, y;
        if (!findValidPosition(x, y)) { x = 0; y = 0; }
        Agent survivor("Survivor_" + std::to_string(i),
                       agents[idx].getId(), x, y, INPUT_SIZE, HIDDEN_SIZE);
        survivor.setBrain(agents[idx].getBrain());
        next_gen.push_back(survivor);
    }

    // 3) Immigrants — cerveaux aléatoires neufs
    for (int i = 0; i < newcomers_count; ++i) {
        int x, y;
        if (!findValidPosition(x, y)) { x = 0; y = 0; }
        unsigned int newId = (unsigned)(day * 10000 + 9000 + i);
        next_gen.emplace_back("Immigrant_" + std::to_string(newId),
                              newId, x, y, INPUT_SIZE, HIDDEN_SIZE);
    }

    // 4) Sélection par tournoi (k=3)
    auto tournament_select = [&](int k = 3) -> const Agent& {
        int best = dist_idx(rng);
        for (int t = 1; t < k; ++t) {
            int idx = dist_idx(rng);
            if (agents[idx].getFitness() > agents[best].getFitness())
                best = idx;
        }
        return agents[best];
    };

    // 5) Crossover + mutation jusqu'au remplissage
    while ((int)next_gen.size() < current_pop_size) {
        const Agent& p1 = tournament_select();
        const Agent& p2 = tournament_select();

        int x, y;
        if (!findValidPosition(x, y)) { x = 0; y = 0; }

        unsigned int childId = (unsigned)(day * 10000 + next_gen.size());
        std::string childName = "Agent_gen"
                              + std::to_string(day / EVOLUTION_PERIOD)
                              + "_" + std::to_string(childId);

        // CORRECTIF : breed reçoit rng (crossover uniforme)
        Agent child = p1.breedWith(p2, childName, childId, x, y, rng);
        // CORRECTIF : mutate reçoit rng + amplitude adaptative (dans LSTM)
        child.mutateBrain(mutation_rate, rng);
        next_gen.push_back(child);
    }

    agents = std::move(next_gen);

    if (stagnant_generations >= STAGNATION_THRESHOLD) {
        std::cout << "⚡ Génération " << day / EVOLUTION_PERIOD
                  << " : Stagnation → Mutation haute ("
                  << (mutation_rate * 100) << "%)\n";
    }
}

// ============================================================
// fast_run — simulation sans affichage
// ============================================================

void Simulation::fast_run() {
    int last_evolution_day = 0;
    int last_report_day    = 0;

    std::cout << "Démarrage simulation rapide...\n";

    // CORRECTIF : réinitialisation des variables de stagnation pour pouvoir
    // appeler fast_run() plusieurs fois sans état résiduel.
    prev_avg_fitness    = -1e300;
    stagnant_generations = 0;

    while (day < MAX_DAYS) {
        if (agents.empty()) {
            std::cerr << "\n💀 EXTINCTION au jour " << day << " !\n";
            break;
        }

        for (auto& agent : agents) {
            auto perception = agent.perceive(map, agents, is_day);
            auto decision   = agent.think(perception);
            agent.act(decision, map, agents, is_day, rng);
        }

        map.updateWorld(is_day, rng);
        ++time_of_day;

        if (time_of_day >= DAY_DURATION * 2) {
            time_of_day = 0;
            ++day;
            logDailyStats();

            if (day - last_report_day >= 1000)
                last_report_day = day;

            if (day % 10 == 0)
                logSocialNetworkSnapshot();

            if (day % EVOLUTION_PERIOD == 0 && day != last_evolution_day) {
                evolvePopulation();
                last_evolution_day = day;
            }
        }

        is_day = (time_of_day < DAY_DURATION);
    }

    if (!agents.empty()) {
        auto best_it = std::max_element(agents.begin(), agents.end(),
            [](const Agent& a, const Agent& b) { return a.getFitness() < b.getFitness(); });
        std::cout << "\nSauvegarde du meilleur agent...\n";
        std::cout << "  Fitness      : " << best_it->getFitness()      << "\n";
        std::cout << "  Énergie      : " << best_it->getEnergie()      << "\n";
        std::cout << "  Satisfaction : " << best_it->getSatisfaction() << "\n";
        best_it->saveBrain("best_brain.txt");
    }

    logfile.close();
    social_logfile.close();
    std::cout << "\nSimulation terminée au jour " << day << "\n";
    displayFinalStats();
}

// ============================================================
// run — simulation avec affichage terminal
// ============================================================

void Simulation::run() {
    int last_evolution_day = 0;

    std::cout << "Démarrage simulation avec affichage...\n";

    // CORRECTIF : réinitialisation de la stagnation
    prev_avg_fitness     = -1e300;
    stagnant_generations = 0;

    // CORRECTIF : utilise MAX_DAYS (depuis config) au lieu du hardcode "500"
    while (day < MAX_DAYS) {
        if (agents.empty()) {
            std::cerr << "\n💀 EXTINCTION au jour " << day << " !\n";
            break;
        }

        for (auto& agent : agents) {
            auto perception = agent.perceive(map, agents, is_day);
            auto decision   = agent.think(perception);
            agent.act(decision, map, agents, is_day, rng);
        }

        map.updateWorld(is_day, rng);
        map.display(agents);

        std::cout << "Jour " << day
                  << " | " << (is_day ? "☀️  JOUR" : "🌙 NUIT")
                  << " | Pop: " << agents.size()
                  << " | Tour: " << time_of_day << "/" << (DAY_DURATION * 2) << "\n";

        usleep(100000); // 0.1 s

        ++time_of_day;

        if (time_of_day >= DAY_DURATION * 2) {
            time_of_day = 0;
            ++day;
            logDailyStats();

            if (day % 10 == 0)
                logSocialNetworkSnapshot();

            if (day % EVOLUTION_PERIOD == 0 && day != last_evolution_day) {
                evolvePopulation();
                last_evolution_day = day;
                usleep(1000000); // pause 1 s lors d'une évolution
            }
        }

        is_day = (time_of_day < DAY_DURATION);
    }

    if (!agents.empty()) {
        auto best_it = std::max_element(agents.begin(), agents.end(),
            [](const Agent& a, const Agent& b) { return a.getFitness() < b.getFitness(); });
        std::cout << "\nSauvegarde du meilleur agent...\n";
        best_it->saveBrain("best_brain.txt");
    }

    logfile.close();
    social_logfile.close();
    std::cout << "\nSimulation terminée\n";
    displayFinalStats();
}

// ============================================================
// Logging
// ============================================================

void Simulation::logDailyStats() {
    if (!logfile.is_open() || agents.empty()) return;

    double total_fitness      = 0.0;
    double total_energie      = 0.0;
    double total_satisfaction = 0.0;

    for (const auto& agent : agents) {
        total_fitness      += agent.getFitness();
        total_energie      += agent.getEnergie();
        total_satisfaction += agent.getSatisfaction();
    }

    int pop_size = (int)agents.size();
    logfile << day                               << ","
            << total_fitness      / pop_size     << ","
            << total_energie      / pop_size     << ","
            << total_satisfaction / pop_size     << ","
            << pop_size                          << "\n";
}

void Simulation::logSocialNetworkSnapshot() {
    if (!social_logfile.is_open()) return;
    for (const auto& agent : agents)
        agent.logSocialMemory(day, social_logfile);
}

void Simulation::displayFinalStats() {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "  STATISTIQUES FINALES\n";
    std::cout << std::string(50, '=') << "\n";
    std::cout << "Jours simulés   : " << day << "\n";
    std::cout << "Population finale: " << agents.size() << " agents\n";

    if (!agents.empty()) {
        double avg_fitness = 0.0, max_fitness = 0.0;
        for (const auto& a : agents) {
            double f = a.getFitness();
            avg_fitness += f;
            if (f > max_fitness) max_fitness = f;
        }
        avg_fitness /= (double)agents.size();
        std::cout << "Fitness moyenne : " << avg_fitness << "\n";
        std::cout << "Fitness max     : " << max_fitness << "\n";
        std::cout << "Générations     : " << day / EVOLUTION_PERIOD << "\n";
    }

    std::cout << "\nFichiers générés :\n";
    std::cout << "  📄 simulation_log.csv\n";
    std::cout << "  📄 social_log.csv\n";
    std::cout << "  📄 best_brain.txt\n";
    std::cout << std::string(50, '=') << "\n";
}
