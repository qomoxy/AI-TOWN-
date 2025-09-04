#ifndef MONDE_H
#define MONDE_H
#include <vector>
#include <string>
#include <map>

class Agent;

enum class CellType { EMPTY, HOUSE, ROAD, WATER, FOREST, BOOK, APPLE, CHAMPIGNON_LUMINEUX};


class Map {
    private : 
        int width, height; 
        std::vector<std::vector<CellType>> grid;
        std::map<std::pair<int, int>, int> regrowth_timers; 

    public:
        void updateWorld(bool is_day);
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

#endif 
