#pragma once 
#include <vector>
#include "Agent.h"
#include <string>

class Agent;

enum CellType { EMPTY, HOUSE, ROAD, WATER, FOREST };
enum Direction { UP, RIGHT, DOWN, LEFT };

class Map {
    private : 
    int width, height; // "width" corrigé
    std::vector<std::vector<CellType>> grid;
    std::vector<Agent> agents;
public:
    Map(int w, int h);

    void generateRandomWorld(); // Une fonction pour créer un monde
    
    bool isValidPosition(int x, int y) const;
    CellType getCell(int x, int y) const;
    void setCell(int x, int y, CellType type);

    // Fonction pour l'affichage
    void display(const std::vector<Agent>& agents);
};

};