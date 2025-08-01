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
    FOREST
}

class Agent_graphique {
    public:
    int x, y; //position
    char symbol; 
    int id;

    Agent(int id, int startX, int StartY, char sym)
        :id(id), x(StartX), y(StartY), symbol(sym) {}
    
    void move(const Map& map) { 
        int newX = x, newY = y;
        
        switch(direction) {
            case 0: newY--; break; // Haut
            case 1: newX++; break; // Droite
            case 2: newY++; break; // Bas
            case 3: newX--; break; // Gauche
        }
        
        // Vérifier si le déplacement est valide
        if (map.isValidPosition(newX, newY) && map.getCell(newX, newY) != WATER) {
            x = newX;
            y = newY;
        }
    }

    //getters 
    int getPosition() {
        return x, y;
    }

    char getSymbol() {
        return symbol;
    }
    
    int getId() {
        return id;
    }

    //setters
    int setPositon(int newX, int newY) {
        x = newX;
        y = newY;

    }
};

class Map {
    private:
    int with, height;
    vector<vector<CellType>> grid;
    vector<Agent> agents;

    public:

    Map(int w, int h) : width(w), height(h) {
        grid.resize(height, vector<CellType>(width, EMPTY));
        srand(time(0)); // Initialiser le générateur aléatoire
    }

    bool isValidPosition(int x, int y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
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
            int x = rand() % width;
            int y = rand() % height;
            if (grid[y][x] == EMPTY) {
                grid[y][x] = type;
                count--;
            }
        }
    }

    void addAgent(int x, int y, char sym) {
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
        vector<vector<char>> displayGrid(height, vector<char>(width, '.'));
        
        // Remplir avec le terrain
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                switch(grid[y][x]) {
                    case EMPTY: displayGrid[y][x] = '.'; break;
                    case HOUSE: displayGrid[y][x] = 'H'; break;
                    case FARM: displayGrid[y][x] = 'F'; break;
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
        system("clear"); // Effacer la console (Linux/macOS)
        
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                cout << displayGrid[y][x] << " ";
            }
            cout << endl;
        }
    }

    const vector<Agent>& getAgents() const { return agents; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

}


