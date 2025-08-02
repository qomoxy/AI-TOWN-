#pragma once 
#include <vector>
#include <string>

class Agent;

enum CellType { EMPTY, HOUSE, ROAD, WATER, FOREST, BOOK, APPLE};


class Map {
    private : 
        int width, height; 
        std::vector<std::vector<CellType>> grid;

    public:
        Map(int w, int h);
        void generateRandomWorld(); 


        // Getters
        int getWidth() const { return width; }
        int getHeight() const { return height; }
        CellType getCell(int x, int y) const;
    
        bool isValidPosition(int x, int y) const;
        void setCell(int x, int y, CellType type);
        

        // Fonction pour l'affichage
        void display(const std::vector<Agent>& agents);
};

