// Fichier: monde.cpp
#include "monde.h"
#include "Agent.h" // On a besoin de connaître l'Agent pour l'affichage
#include <iostream>
#include <cstdlib> // Pour rand()
#include <ctime>   // Pour time()

Map::Map(int w, int h) : width(w), height(h) {
    grid.resize(height, std::vector<CellType>(width, EMPTY));
    srand(time(0));
}

bool Map::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

CellType Map::getCell(int x, int y) const {
    if (isValidPosition(x, y)) {
        return grid[y][x];
    }
    return WATER; // Retourner une case "infranchissable" si hors map
}

void Map::generateRandomWorld() {
    // Exemple simple de génération
    for (int i = 0; i < (width * height) / 10; ++i) {
        grid[rand() % height][rand() % width] = FOREST;
        grid[rand() % height][rand() % width] = WATER;
    }
}

void Map::display(const std::vector<Agent>& agents) {
    system("clear"); // ou "cls" sur Windows
    std::vector<std::vector<char>> displayGrid(height, std::vector<char>(width));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            switch (grid[y][x]) {
                case EMPTY:  displayGrid[y][x] = '.'; break;
                case HOUSE:  displayGrid[y][x] = 'H'; break;
                case WATER:  displayGrid[y][x] = '~'; break;
                case FOREST: displayGrid[y][x] = 'T'; break;
                case ROAD:   displayGrid[y][x] = '#'; break;
            }
        }
    }

    // Placer les agents par-dessus
    for (const auto& agent : agents) {
        std::pair<int, int> pos = agent.getPosition();
        if (isValidPosition(pos.first, pos.second)) {
            displayGrid[pos.second][pos.first] = agent.getSymbol();
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