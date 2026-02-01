#include "simulation.h" 
#include "SimulationConfig.h"
#include <iostream>
#include <string>


int main(int argc, char* argv[]) {
    std::string mode = "fast";  
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════╗\n";
    std::cout << "║          AI TOWN - TIPE 2025-2026            ║\n";
    std::cout << "║    Évolution d'une société d'agents IA       ║\n";
    std::cout << "║                                              ║\n";
    std::cout << "║      Alexis Bellot & Quentin Goursaud        ║\n";
    std::cout << "╚══════════════════════════════════════════════╝\n";
    std::cout << std::endl;

    Simulation sim(MAP_WIDTH, MAP_HEIGHT, INITIAL_AGENT_COUNT);

    if (mode == "display" || mode == "d" || mode == "visual" || mode == "v") {
        sim.run();
    } else if (mode == "fast" || mode == "f" || mode == "quick" || mode == "q") {
        sim.fast_run();
    } else {
        std::cerr << "Mode inconnu: '" << mode << "'. Veillez Utiliser le mode fast ou display" << std::endl; 
        return 1;
    }

    return 0;
}