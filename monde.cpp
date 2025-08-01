#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// Type de casses 
enum CellType { 
    EMPTY,
    HOUSE,
    ROAD,
    WATER,
    FOREST,
    
};


class Map {
    private:
    int with, height;
    vector<vector<CellType>> grid;
    vector<Agent> agents;

    public:

    Map(int w, int h) : with(w), height(h) {
        grid.resize(height, vector<CellType>(with, EMPTY));
        srand(time(0)); // Initialiser le générateur aléatoire
    }

    bool isValidPosition(int x, int y) const {
        return x >= 0 && x < with && y >= 0 && y < height;
    }

    CellType getCell(int x, int y) const {
        if (isValidPosition(x, y)) {
            return grid[y][x];
        }
        return EMPTY;
    }

    void setCell(int x, int y, CellType type) {
        if (isValidPosition(x, y)) {
            grid[y][x] = type;
        }
    }

    void placeRandom(CellType type, int count) {
        while (count > 0) {
            int x = rand() % with;
            int y = rand() % height;
            if (grid[y][x] == EMPTY) {
                grid[y][x] = type;
                count--;
            }
        }
    }

    void addAgent(int x, int y, char sym) { // à refaire 
        if (isValidPosition(x, y)) {
            agents.emplace_back(agents.size(), x, y, sym);
        }
    }

    void moveAgents() {
        for (auto& agent : agents) {
            agent.moveRandom(*this);
        }
    }

    void display() {
        // Créer un buffer d'affichage
        vector<vector<char>> displayGrid(height, vector<char>(with, '.'));
        
        // Remplir avec le terrain
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < with; ++x) {
                switch(grid[y][x]) {
                    case EMPTY: displayGrid[y][x] = '.'; break;
                    case HOUSE: displayGrid[y][x] = 'H'; break;

                    case WATER: displayGrid[y][x] = 'W'; break;
                    case FOREST: displayGrid[y][x] = 'T'; break;
                    case ROAD: displayGrid[y][x] = 'R'; break;
                }
            }
        }
        
        // Placer les agents
        for (const auto& agent : agents) {
            if (isValidPosition(agent.x, agent.y)) {
                displayGrid[agent.y][agent.x] = agent.symbol;
            }
        }
        
        // Afficher
        system("clear"); 
        
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < with; ++x) {
                cout << displayGrid[y][x] << " ";
            }
            cout << endl;
        }
    }

    const vector<Agent>& getAgents() const { return agents; }
    int getWith() const { return with; }
    int getHeight() const { return height; }

    bool Map::isPositionFree(int x, int y ) { 
        if(!isValidPosition(x,y)) { return false;}

        for(const auto&  agent : agents ) { 
            if(agent.x == x and agent.y == y ) {
                return false;
            }
        }
        return getCell(x,y) != WATER;
    }

}


