#ifndef MONDE_H
#define MONDE_H
#include <vector>
#include <string>
#include <map>
#include <random>

class Agent;


enum class CellType { EMPTY, HOUSE, ROAD, WATER, FOREST, BOOK, APPLE, CHAMPIGNON_LUMINEUX};

// Ajout de métadonnées pour la repousse
struct FoodMeta {
    CellType type;
    int regrowth_time;

    // ** CORRECTION 2 : Ajout d'un constructeur **
    // Cela rend la création d'objets FoodMeta plus simple et corrige l'erreur d'assignation.
    FoodMeta(CellType t = CellType::EMPTY, int time = 0) : type(t), regrowth_time(time) {}
};


class Map {
    private : 
        int width, height; 
        std::vector<std::vector<CellType>> grid;
        // La map de timers contient maintenant aussi le type de ressource à faire repousser
        std::map<std::pair<int, int>, FoodMeta> regrowth_timers; 

    public:
        // Transmet le générateur de nombres aléatoires pour de meilleurs résultats
        void updateWorld(bool is_day, std::mt19937& rng);
        Map(int w, int h);
        void generateRandomWorld(std::mt19937& rng); 


        // Getters
        int getWidth() const { return width; }
        int getHeight() const { return height; }
        CellType getCell(int x, int y) const;
    
        bool isValidPosition(int x, int y) const;
        void setCell(int x, int y, CellType type);
        
        // Lance le minuteur de repousse pour une case
        void startRegrowth(int x, int y, CellType original_food_type);

        // Fonction pour l'affichage
        void display(const std::vector<Agent>& agents);
};

#endif

