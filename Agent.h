#pragma once
#include "LSTM.h"
#include "monde.h"
#include <string>
#include <vector>

class Agent {
private:
    std::string name;
    unsigned int id;
    unsigned int personnalite;
    double vitesse;
    double energie;
    double sociable;
    double hunger;
    double satisfaction;
    LSTM brain;

    // Position et Apparence 
    Direction direction;
    int x,  y; 
    char symbol; 

public:
    // Constructeur principal
    Agent(const std::string& name, 
          unsigned int id, 
          unsigned int personnalite,
          double vitesse, 
          double energie, 
          double satisfaction,
          int input_size, 
          int hidden_size, 
          int x, 
          int y, 
          char symbol,
          Direction direction,
          double hunger,
          double sociable);

    // Constructeur pour le breeding
    Agent(const std::string& name, 
          unsigned int id, 
          unsigned int personnalite,
          double vitesse, 
          double energie, 
          double satisfaction,
          const LSTM& brain,
          int x,
          int y,
          char symbol,
          Direction direction,
          double hunger,
          double sociable);

    // Getters
    std::string getName() const { return name; }

    LSTM& getBrain() { return brain; }

    unsigned int getId() const {return id; }

    unsigned int getPersonnalite() const { return personnalite; }

    double getVitesse() const  { return vitesse; }

    double getEnergie() const { return energie; }

    std::pair<int, int> getPosition() const {
        return {x, y};
    }

    char getSymbol() const {
        return symbol;
    }

    Direction getDirection() const { 
        return Direction;
    }



    //Setters
    void setName(std::string new_name) { name = new_name; } 

    void setBrain(LSTM& new_brain) {brain = new_brain; }

    void setPersonnalite(unsigned int new_personnalite) { personnalite = new_personnalite; }

    void setVitesse(double new_vitesse) { vitesse = new_vitesse;}

    void setEnergie(double new_energie) {energie = new_energie; }

    void setPosition(int newX, int newY) {
        x = newX;
        y = newY;

    }

    void move(const Map& map);
    void perceiveEnvironment(const Map& map);
    

    // Méthodes fonctionnelles
    std::vector<double> think(const std::vector<double>& input);
    void mutateBrain(double mutationRate);
    Agent breedWith(const Agent& partner, const std::string& childName, unsigned int childId) const;

    // Ancien agent_graphique

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

    void updateNeeds(double timePassed) {
        hunger += 0.1 * timePassed;
        energy -= 0.05 * timePassed;
        social -= 0.03 * timePassed;
    }

    void sendMessage(const std::string& msg, Map& map) {
        for (auto& other : map.getAgents()) {
            if (distanceTo(other) < 5) {
                other.receiveMessage(msg);
            }
        }
    }

};