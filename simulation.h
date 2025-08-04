#pragma once
#include "Agent.h"
#include "monde.h"
#include <vector>

class Simulation {
public:
    Map map;
    std::vector<Agent> agents;
    int day;

    bool is_day = true;
    int time_of_day = 0;
    const int DAY_DURATION = 100;

    Simulation(int map_width, int map_height, int num_agents);
    void run();

private:
    void evolvePopulation();
};