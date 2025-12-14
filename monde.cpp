#include "monde.h"
#include "Agent.h" 
#include <iostream>
#include <vector>

Map::Map(int w, int h) : width(w), height(h) {
    grid.resize(height, std::vector<CellType>(width, CellType::EMPTY));
}

bool Map::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

CellType Map::getCell(int x, int y) const {
    if (isValidPosition(x, y)) {
        return grid[y][x];
    }
    return CellType::WATER; // Retourner une case "infranchissable" si hors map
}

void Map::setCell(int x, int y, CellType type) {
    if (isValidPosition(x, y)) {
        grid[y][x] = type;
    }
}

// Démarre un minuteur de repousse pour une case donnée
void Map::startRegrowth(int x, int y, CellType original_food_type) {
    if (isValidPosition(x, y)) {
        int time = 0;
        CellType becomes = CellType::EMPTY;

        if (original_food_type == CellType::APPLE) {
            time = TIME_REGROWTH_APPLE ; // Repousse plus vite
            becomes = CellType::EMPTY;
        } else if (original_food_type == CellType::CHAMPIGNON_LUMINEUX) {
            time = TIME_REGROWTH_CHAMP ; // Plus long à repousser
            becomes = CellType::FOREST;
        }

        grid[y][x] = becomes;
        if (time > 0) {
            regrowth_timers[{x, y}] = FoodMeta(original_food_type, time);
        }
    }
}

// Met à jour le monde en gérant la repousse des ressources
void Map::updateWorld(bool is_day, std::mt19937& rng) {
    // 1. Gérer la repousse
    for (auto it = regrowth_timers.begin(); it != regrowth_timers.end(); ) {
        it->second.regrowth_time--;
        if (it->second.regrowth_time <= 0) {
            // Le temps est écoulé, la ressource réapparaît
            setCell(it->first.first, it->first.second, it->second.type);
            it = regrowth_timers.erase(it); // On supprime le minuteur
        } else {
            ++it;
        }
    }

    // 2. Gérer le cycle jour/nuit des champignons et autres logiques
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Un champignon lumineux n'apparaît que la nuit sur une case de forêt
            if (grid[y][x] == CellType::FOREST && !is_day) {
                 std::uniform_int_distribution<int> distrib(0, CHANCE_MUSHROOM_SPAWN);
                 if(distrib(rng) == 0) {
                    grid[y][x] = CellType::CHAMPIGNON_LUMINEUX;
                 }
            // S'il fait jour, un champignon lumineux redevient une simple forêt
            } else if (is_day && grid[y][x] == CellType::CHAMPIGNON_LUMINEUX) {
                grid[y][x] = CellType::FOREST;
            }
        }
    }
}

// Génère le monde aléatoirement en utilisant le générateur mt19937
void Map::generateRandomWorld(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist_w(0, width - 1);
    std::uniform_int_distribution<int> dist_h(0, height - 1);

    for (int i = 0; i < (width * height) * RATIO_FOREST ; ++i) {
        grid[dist_h(rng)][dist_w(rng)] = CellType::FOREST;
        grid[dist_h(rng)][dist_w(rng)] = CellType::WATER;
    }
    for (int i = 0; i < (width * height) * RATIO_APPLE ; ++i) {
        int x = dist_w(rng);
        int y = dist_h(rng);
        if (grid[y][x] == CellType::EMPTY) {
            grid[y][x] = CellType::APPLE;
        }
    }
     for (int i = 0; i < (width * height) * RATIO_BOOK ; ++i) {
        int x = dist_w(rng);
        int y = dist_h(rng);
        if (grid[y][x] == CellType::EMPTY) {
            grid[y][x] = CellType::BOOK;
        }
    }
}

void Map::display(const std::vector<Agent>& agents) {
    std::cout << "\033[2J\033[1;1H";
    std::vector<std::vector<char>> displayGrid(height, std::vector<char>(width));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            switch (grid[y][x]) {
                case CellType::EMPTY:  displayGrid[y][x] = '.'; break;
                case CellType::HOUSE:  displayGrid[y][x] = 'H'; break;
                case CellType::APPLE:  displayGrid[y][x] = 'A'; break;
                case CellType::WATER:  displayGrid[y][x] = '~'; break;
                case CellType::FOREST: displayGrid[y][x] = 'T'; break;
                case CellType::ROAD:   displayGrid[y][x] = '#'; break;
                case CellType::BOOK:   displayGrid[y][x] = 'B'; break;
                case CellType::CHAMPIGNON_LUMINEUX: displayGrid[y][x] = 'C'; break;
            }
        }
    }

    // Placer les agents par-dessus
    for (const auto& agent : agents) {
        if (isValidPosition(agent.getX(), agent.getY())) {
            displayGrid[agent.getY()][agent.getX()] = agent.getSymbol();
        }
    }

    // Afficher la grille finale
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::cout << displayGrid[y][x] << ' ';
        }
        std::cout << std::endl;
    }
}

