const int apple_nutrition = 40;
const int champ_nutrition = 60;

// Coût de base des actions, pour ne pas avoir de "magie" dans le code
const double EAT_COST = 0.0;
const double TALK_COST = 1.0;
const double DAY_SLEEP_PENALTY = 1.0;
const double INTERACT_COST = 1.0;
const double MOVE_COST = 1.0;
const double LIVING_COST = 0.5;
static const int VISION_RANGE = 3;
static const int PERCEPTION_SIZE = 151;
const double MAX_DISTANCE = 44.72; // sqrt(40*40 + 20*20)
int regrowth_apple = 50;
int regrowth_champ = 150;
const int DAY_DURATION = 50;
const nb_of_days = 50000;
