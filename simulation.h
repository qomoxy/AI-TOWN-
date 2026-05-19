#pragma once

#include "Agent.h"
#include "monde.h"
#include <vector>
#include <fstream>
#include <random>

class Simulation {
private:
    Map               map;
    std::vector<Agent> agents;
    std::mt19937      rng;

    int  day         = 0;
    int  time_of_day = 0;
    bool is_day      = true;

    // CORRECTIF : variables membres (au lieu de static locaux dans evolvePopulation)
    // → permet de réinitialiser l'état de stagnation entre deux appels à run()/fast_run().
    double prev_avg_fitness     = -1e300;
    int    stagnant_generations = 0;

    std::ofstream logfile;
    std::ofstream social_logfile;

    // --- Méthodes internes ---
    void evolvePopulation();
    void logDailyStats();
    void logSocialNetworkSnapshot();
    void displayFinalStats();

    // CORRECTIF : placement sécurisé avec compteur de tentatives max
    bool findValidPosition(int& out_x, int& out_y);

public:
    Simulation(int map_width, int map_height, int num_agents);

    // Simulation rapide (sans affichage) — jusqu'à MAX_DAYS
    void fast_run();

    // Simulation avec affichage terminal — jusqu'à MAX_DAYS
    void run();
};
