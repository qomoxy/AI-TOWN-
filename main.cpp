#include "simulation.h" 
#include "SimulationConfig.h"

int main() {
    Simulation sim(MAP_WIDTH, MAP_HEIGHT, INITIAL_AGENT_COUNT);
    // Choix du mode : run() pour voir, fast_run() pour entraîner. 
    sim.fast_run(); // Lancement de la boucle principale
    return 0;
}
