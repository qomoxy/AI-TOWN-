#include "LSTM.h"
#include <string>

class Agent {
private:
    std::string name;
    unsigned int id;
    unsigned int personnalite;
    double vitesse;
    double energie;
    double satisfaction;
    LSTM brain;

public:
    // Constructeur principal
    Agent(const std::string& name, 
          unsigned int id, 
          unsigned int personnalite,
          double vitesse, 
          double energie, 
          double satisfaction,
          int input_size, 
          int hidden_size);

    // Constructeur pour le breeding
    Agent(const std::string& name, 
          unsigned int id, 
          unsigned int personnalite,
          double vitesse, 
          double energie, 
          double satisfaction,
          const LSTM& brain);

    // Getters
    std::string getName() const { return name; }

    LSTM& getBrain() { return brain; }

    unsigned int getId() const {return id; }

    unsigned int getPersonnalite() const { return personnalite; }

    double getVitesse() const  { return vitesse; }

    double getEnergie() const { return satisfaction; }

    //Setters
    void setName(std::string new_name) { name = new_name; } 

    void setBrain(LSTM& new_brain) {brain = new_brain; }

    void setPersonnalite(unsigned int new_personnalite) { personnalite = new_personnalite; }

    void setVitesse(double new_vitesse) { vitesse = new_vitesse;}

    void setEnergie(double new_energie) {energie = new_energie; }
    

    // Méthodes fonctionnelles
    std::vector<double> think(const std::vector<double>& input);
    void mutateBrain(double mutationRate);
    Agent breedWith(const Agent& partner, const std::string& childName, unsigned int childId) const;
};