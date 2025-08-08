#include "simulation.h" 

int main() {
    // Création de la simulation : map de 40x20 avec 10 agents
    Simulation sim(40, 20, 10);
    sim.fast_run(); // Lancement de la boucle principale
    return 0;
}
