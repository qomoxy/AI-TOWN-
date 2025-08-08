#pragma once
#include "Agent.h"
#include "monde.h"
#include <fstream> 
#include <vector>
#include <random>

class Simulation {
public:
    Map map;
    std::vector<Agent> agents;
    int day;

    bool is_day = true;
    int time_of_day = 0;
    const int DAY_DURATION = 25;

    Simulation(int map_width, int map_height, int num_agents);
    void run();
    void fast_run();
    
    std::mt19937 rng;
private:
    void evolvePopulation();

    std::ofstream logfile; 
    void logDailyStats();
};
