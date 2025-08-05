#include "monde.h"
#include "Agent.h" 
#include <iostream>
#include <cstdlib> 
#include <ctime>  

Map::Map(int w, int h) : width(w), height(h) {
    grid.resize(height, std::vector<CellType>(width, CellType::EMPTY));
    srand(time(0));
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

void Map::updateWorld(bool is_day) {
    // Pour chaque case vide, il y a une petite chance qu'un buisson repousse
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (grid[y][x] == CellType::EMPTY && (rand() % 1000) < 1) { // 0.1% de chance par tour
                grid[y][x] = CellType::APPLE;
            } else if(grid[y][x] == CellType::FOREST && (rand() % 1000) < 1 && !is_day) {
                grid[y][x] = CellType::CHAMPIGNON_LUMINEUX;
            } else if (is_day && grid[y][x] == CellType::CHAMPIGNON_LUMINEUX) {
                grid[y][x] = CellType::FOREST;
            }

        }
    }
}

void Map::generateRandomWorld() {
    for (int i = 0; i < (width * height) / 10; ++i) {
        grid[rand() % height][rand() % width] = CellType::FOREST;
        grid[rand() % height][rand() % width] = CellType::WATER;
    }
    for (int i = 0; i < (width * height) / 20; ++i) {
        int x = rand() % width;
        int y = rand() % height;
        if (grid[y][x] == CellType::EMPTY) {
            grid[y][x] = CellType::APPLE;
        }
    }
}

void Map::display(const std::vector<Agent>& agents) {
    system("clear"); // ou "cls" sur Windows
    std::vector<std::vector<char>> displayGrid(height, std::vector<char>(width));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            switch (grid[y][x]) {
                case CellType::EMPTY:  displayGrid[y][x] = '.'; break;
                case CellType::HOUSE:  displayGrid[y][x] = 'H'; break;
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