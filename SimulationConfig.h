#pragma once
#include <cmath>

// ==========================================
// PARAMÈTRES GLOBAUX DE LA SIMULATION
// ==========================================

// --- Dimensions & Durée ---
const int MAP_WIDTH = 40;
const int MAP_HEIGHT = 20;
const int INITIAL_AGENT_COUNT = 10;
const int MAX_DAYS = 50000;
const int DAY_DURATION = 50; // Nombre de ticks par phase (jour/nuit) = 100 ticks total/jour

// --- Calculs automatiques ---
const double MAX_DISTANCE = std::sqrt(MAP_WIDTH * MAP_WIDTH + MAP_HEIGHT * MAP_HEIGHT);

// ==========================================
// PARAMÈTRES DU RÉSEAU DE NEURONES (LSTM)
// ==========================================
const int INPUT_SIZE = 11;  // Vision(7) + État(2) + Mémoire(2)
const int HIDDEN_SIZE = 8;  // Taille du cerveau

// ==========================================
// PARAMÈTRES DES AGENTS
// ==========================================
const int VISION_RANGE = 3;
const double MAX_STAT = 100.0; // Énergie & Satisfaction Max

// --- Coûts des actions (Energie) ---
const double COST_EAT = 0.0;
const double COST_TALK = 1.0;
const double COST_INTERACT = 1.0;
const double COST_MOVE = 1.0;
const double COST_LIVING = 0.5; // Coût par tour (métabolisme)
const double COST_SLEEP_PENALTY_DAY = 1.0; // Pénalité si on dort le jour

// --- Gains (Energie & Satisfaction) ---
const int GAIN_ENERGY_APPLE = 40;
const int GAIN_ENERGY_CHAMP = 60;
const int GAIN_ENERGY_SLEEP = 25;

const double GAIN_SATISFACTION_EAT = 2.0;
const double GAIN_SATISFACTION_BOOK = 15.0;

// Communication : Gain = BASE + (ScoreSocial * FACTOR)
const double GAIN_SATISFACTION_TALK_BASE = 3.0;
const double GAIN_SATISFACTION_TALK_FACTOR = 0.5; 

// ==========================================
// PARAMÈTRES DU MONDE (Génération & Repousse)
// ==========================================
const int TIME_REGROWTH_APPLE = 50;
const int TIME_REGROWTH_CHAMP = 150;

// Probabilité d'apparition (1 chance sur X)
const int CHANCE_MUSHROOM_SPAWN = 2000; 

// Ratios de génération (0.1 = 10% de la carte)
const double RATIO_FOREST = 0.1;
const double RATIO_APPLE = 0.05;
const double RATIO_BOOK = 0.02;

// ==========================================
// PARAMÈTRES D'ÉVOLUTION (Génétique)
// ==========================================
const int EVOLUTION_PERIOD = 5; // Évolution tous les X jours

// Mutation
const double MUTATION_RATE_NORMAL = 0.1;
const double MUTATION_RATE_HIGH = 0.25;
const int STAGNATION_THRESHOLD = 5; // Nb de gén. sans progrès avant mutation forte

// Sélection (Doit sommer < 1.0 pour laisser place aux enfants)
const double RATIO_ELITE = 0.1;             // Les meilleurs restent
const double RATIO_RANDOM_SURVIVORS = 0.05; // Chanceux
const double RATIO_NEWCOMERS = 0.1;         // Nouveaux immigrants
