#pragma once
#include "Agent.h"
#include "monde.h"
#include "SimulationConfig.h" 
#include <fstream> 
#include <vector>
#include <map>
#include <random>

class Simulation {
public:
    Map map;
    std::vector<Agent> agents;
    int day;

    bool is_day = true;
    int time_of_day = 0;
    const int DAY_DURATION = ::DAY_DURATION;  
    
    Simulation(int map_width, int map_height, int num_agents);
    
    // Méthodes de simulation
    void run();       // Mode avec affichage graphique (500 jours max)
    void fast_run();  // Mode rapide sans affichage (MAX_DAYS)

    std::mt19937 rng;

private:
    // Évolution génétique
    void evolvePopulation();

    // Logs et statistiques
    std::ofstream logfile; 
    std::ofstream social_logfile;
    void logSocialNetworkSnapshot();
    void logDailyStats();
    
    void displayProgress();      // Affiche les stats courantes
    void displayFinalStats();    // Affiche le résumé final
};