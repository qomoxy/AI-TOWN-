<a name="readme-top"></a>

<div align="center">
  <h3 align="center">Société IA — TIPE 2025-2026</h3>

  <p align="center">
    Étude de l'évolution d'une population au sein d'un milieu social virtuel.<br>
    <br>
    <b>Bellot Alexis & Goursaud Quentin</b>
    <br>
    TIPE Cycle et boucles
    <br>
  </p>
</div>

## Table des matières

1. [Contexte et motivation](#contexte-et-motivation)
2. [Objectifs](#objectifs)
3. [Choix du modèle](#choix-du-modèle)
4. [LSTM](#lstm)
5. [Implémentation](#implémentation)
6. [Fitness et Itérations](#fitness-et-itérations)
7. [Cycle d'Évolution et Sélection](#cycle-dévolution-et-sélection)
8. [Résultats et Observations Clés](#résultats-et-observations-clés)
9. [Pour commencer](#pour-commencer)
10. [Documentation](#documentation)


## Contexte et motivation

Dans un monde où le numérique occupe une place centrale, l'intelligence artificielle suscite un intérêt croissant. Ce projet explore comment une population d'agents virtuels évolue et interagit dans un environnement social simulé. L'objectif est de comprendre les dynamiques sociales et comportementales qui émergent de ces interactions, tout en abordant les enjeux liés à la modélisation de l'IA.

Le thème **Cycle et boucles** est abordé à deux niveaux :
- le **cycle d'évolution darwinienne** (sélection, reproduction, mutation) ;
- les **boucles de rétroaction** du réseau LSTM, qui gèrent la mémoire et la prise de décision des agents.

---

## Objectifs

- **Concevoir un environnement virtuel** où des agents évoluent librement.
- **Développer des agents intelligents** capables d'interactions complexes entre eux et avec leur environnement.
- **Définir une fonction de fitness** pertinente pour évaluer la performance des agents.
- **Étudier l'évolution des agents** à travers des mesures fiables et reproductibles.

---

## Choix du modèle

### Contraintes du projet

- **Autonomie** : réaliser l'ensemble du projet nous-mêmes.
- **Ressources** : utiliser uniquement un CPU (sans GPU ni NPU).
- **Temporalité** : finaliser le projet en moins d'une année scolaire.

### Besoins identifiés

- Des agents capables de s'adapter, d'interagir et d'évoluer en société.
- Des indicateurs de performance pour mesurer leur évolution individuelle et collective.
- Un système permettant l'acclimatation progressive des agents à leur environnement.
- Un environnement apte à accueillir et faire évoluer cette population.

Pour simuler des comportements proches de l'humain, nous avons choisi les **LSTM** (Long Short-Term Memory), un type de réseau de neurones récurrents capable de gérer la mémoire et le contexte temporel.


## LSTM

Les LSTM, introduits par **Hochreiter & Schmidhuber** et popularisés par **Christopher Olah**, sont dotés de trois portes internes :

| Porte | Rôle |
|---|---|
| **Oubli** | Décide quelles informations supprimer de la mémoire |
| **Entrée** | Sélectionne les nouvelles informations à mémoriser |
| **Sortie** | Détermine l'état de sortie du neurone |

Ce mécanisme permet de modéliser des comportements séquentiels avec une mémoire à long terme — parfaitement adapté à des agents qui doivent apprendre et évoluer dans un environnement social.

### Optimisation de la vision

Contrairement à une approche classique « pixel par pixel » (151 entrées, testée et abandonnée car non fonctionnelle), nous avons opté pour une **vision relative simplifiée à 11 entrées**, inspirée de la biologie :

| # | Entrée | Plage |
|---|---|---|
| 1 | Énergie normalisée | [0, 1] |
| 2 | Satisfaction normalisée | [0, 1] |
| 3 | Distance à la nourriture la plus proche | [0, 1] |
| 4 | Angle vers la nourriture | [-1, 1] |
| 5 | Distance à l'agent le plus proche | [0, 1] |
| 6 | Angle vers l'agent | [-1, 1] |
| 7 | Jour / Nuit | {0, 1} |
| 8 | Nombre d'agents proches (rayon 3) | [0, 1] |
| 9 | Type de case actuelle | normalisé |
| 10 | Moyenne des scores sociaux | [-1, 1] |
| 11 | Niveau d'urgence (100 − énergie) / 100 | [0, 1] |

Cette réduction de dimensionnalité permet une convergence beaucoup plus rapide de l'apprentissage.

---

## Fitness et Itérations

Les agents sont évalués par un score de **fitness** combinant deux paramètres :

- **Énergie** : capacité à effectuer des actions (manger, bouger).
- **Satisfaction** : bien-être social et intellectuel.

### Première approche (abandonnée) — fitness additive

```
Fitness = énergie × 0.4 + satisfaction × 0.6
```

Cette formule a mené à un échec : l'évolution favorisait des agents inactifs avec une satisfaction élevée mais une énergie nulle — des **« cadavres satisfaits »**.

### Formule retenue — fitness multiplicative

```
Fitness = (énergie + 1) × (satisfaction + 1)
```

Avec cette formule, un agent doit obligatoirement maintenir ses deux statistiques à un niveau élevé pour être performant. Le **+1** garantit que la fitness reste strictement positive même quand une stat tombe à zéro, évitant toute extinction immédiate.

---

## Cycle d'Évolution et Sélection

Un cycle de sélection a lieu tous les **5 jours simulés**. La stratégie de reproduction combine :

| Mécanisme | Proportion | Description |
|---|---|---|
| **Élitisme** | 10 % | Conservation des meilleurs cerveaux |
| **Survivants aléatoires** | 5 % | Maintien de la diversité génétique |
| **Immigrants** | 10 % | Injection de cerveaux entièrement nouveaux |
| **Crossover + Mutation** | 75 % | Reproduction par tournoi (k=3), crossover uniforme |

### Crossover uniforme

Chaque poids synaptique de l'enfant est tiré aléatoirement chez l'un ou l'autre parent (50/50), préservant la diversité mieux qu'une simple moyenne arithmétique.

### Mutation adaptative

Le taux et l'amplitude de mutation augmentent automatiquement si la fitness moyenne stagne sur plusieurs générations, permettant de sortir des optima locaux.

```
Taux normal  : 15 %   Amplitude : ~0.05
Taux élevé   : 45 %   Amplitude : ~0.15  (déclenché après stagnation)
```

---

## Résultats et Observations Clés

Nos simulations (jusqu'à 50 000 jours) ont permis d'observer trois phénomènes majeurs :

1. **La Crise de Survie** : extinction rapide lors des premiers tests, soulignant la difficulté de l'environnement et l'importance du choix de la fitness.
2. **L'Émergence Sociale** : avec la fitness multiplicative, les agents ont spontanément appris à former des **« paires de survie »** pour maximiser leur satisfaction mutuelle.
3. **La Visualisation du Cerveau** : grâce à `visu_cerveau.py`, nous générons des *heatmaps* de décision montrant comment l'agent arbitre entre « Manger » et « Dormir » selon son niveau d'énergie.

![Graphe Social](https://github.com/user-attachments/assets/a3c6c088-7408-446e-abb0-487b92e423a0)
*Figure 1 : Évolution des relations sociales entre les agents sur 50 000 jours.*

Les données brutes (fichiers `.csv`) sont disponibles dans le dossier `test_stable/`.

---

## Pour commencer

### Prérequis

- **C++17** : compilateur `g++` ou `clang++`
- **Python 3** : bibliothèques `pandas`, `matplotlib`, `networkx`, `seaborn`
- **Make** (optionnel, recommandé)

```bash
# Sur Debian/Ubuntu
sudo apt-get install build-essential

# Dépendances Python
pip install pandas matplotlib networkx seaborn
```

### Installation et compilation

```bash
# 1. Cloner le dépôt
git clone https://github.com/qomoxy/AI-TOWN-.git
cd AI-TOWN-

# 2a. Compiler avec Make (recommandé)
make

# 2b. Ou compiler manuellement
g++ -std=c++17 -O2 -o ai-town main.cpp agent.cpp monde.cpp simulation.cpp
```

### Lancement

```bash
# Simulation rapide (sans affichage, jusqu'à MAX_DAYS)
make fast
# ou : ./ai-town fast

# Simulation avec affichage terminal
make run
# ou : ./ai-town
```

> **Note** : lire `main.cpp` avant de lancer — `run()` affiche la simulation en temps réel, `fast_run()` tourne sans affichage pour de grandes simulations.

### Analyse des résultats

```bash
python3 analyse.py      # Génère courbes_stats.png + graphe du réseau social
python3 visu_cerveau.py # Génère la heatmap de décision (nécessite best_brain.txt)
```

---

## Documentation

### Structure du projet

```
AI-TOWN-/
├── main.cpp            # Point d'entrée (run / fast_run)
├── Agent.h / agent.cpp # Cerveau, corps, perception et actions des agents
├── LSTM.h              # Réseau de neurones LSTM (forward, breed, mutate)
├── monde.h / monde.cpp # Carte, génération du monde, mise à jour des cellules
├── simulation.h / simulation.cpp  # Boucle de simulation et évolution
├── SimulationConfig.h  # Tous les paramètres centralisés
├── analyse.py          # Analyse statistique et réseau social
├── visu_cerveau.py     # Heatmap de décision du meilleur agent
├── Makefile            # Compilation simplifiée
└── test_stable/        # Fichiers CSV de résultats de référence
```

### Paramétrage (`SimulationConfig.h`)

Tous les paramètres de l'univers sont centralisés dans ce fichier, organisés par catégorie :

- **PARAMÈTRES GLOBAUX** : dimensions de la carte, durée de simulation, durée du jour.
- **RÉSEAU DE NEURONES** : taille de l'entrée (11), taille cachée (8), taille de sortie (8).
- **AGENTS** : coûts des actions, gains d'énergie et de satisfaction.
- **MONDE** : probabilités d'apparition des ressources, ratios de génération.
- **ÉVOLUTION** : taux de mutation, seuils de stagnation, ratios de sélection.

### Affichage terminal (`run()`)

| Symbole | Élément |
|---|---|
| `@` | Agent |
| `.` | Case vide |
| `~` | Eau (infranchissable) |
| `T` | Forêt |
| `A` | Pomme (comestible) |
| `C` | Champignon lumineux (nuit uniquement) |
| `B` | Livre (augmente la satisfaction) |

### Actions des agents

| # | Action | Effet |
|---|---|---|
| 0 | **Manger** | Consomme la ressource de la case courante |
| 1 | **Parler** | +5 satisfaction à chaque agent adjacent, +1 score social |
| 2 | **Dormir** | +25 énergie la nuit ; pénalité le jour |
| 3 | **Lire** | +15 satisfaction (si un livre est sur la case) |
| 4–7 | **Se déplacer** | Déplace l'agent d'une case |

**Énergie** : démarre à 70, max 100. Coût : −1/tour le jour, −1.5/tour la nuit.  
**Satisfaction** : démarre à 10, max 100. Décroît de −0.5/tour.

### Mécanique des ressources

| Ressource | Énergie | Satisfaction | Repousse |
|---|---|---|---|
| Pomme (`A`) | +40 | +2 | 50 tours → case vide |
| Champignon (`C`) | +60 | +2 | 150 tours → redevient forêt |
| Livre (`B`) | — | +15 | disparaît après lecture |

### Données générées

À la fin de chaque simulation, trois fichiers sont produits :

- **`simulation_log.csv`** : fitness moyenne, énergie moyenne, satisfaction moyenne et taille de la population, jour par jour.
- **`social_log.csv`** : snapshot des scores de relation entre chaque paire d'agents, tous les 10 jours.
- **`best_brain.txt`** : poids LSTM du meilleur agent, rechargeable pour analyse.

### Carte

La carte de `MAP_HEIGHT × MAP_WIDTH` cases est générée aléatoirement à chaque lancement. Un jour dure **100 tours** (50 de jour + 50 de nuit). Les agents sont placés aléatoirement sur des cases non-aquatiques, sans superposition.

---

## Sources

- Hochreiter & Schmidhuber, *Long Short-Term Memory*, Neural Computation, 1997.
- Christopher Olah, [*Understanding LSTMs*](https://colah.github.io/posts/2015-08-Understanding-LSTMs/), 2015.

---

## À venir

- Cycle de vie complet avec reproduction intégrée et mort naturelle.
- Spécialisation des agents (rôles sociaux émergents).

---

> Pour toute suggestion ou question, n'hésitez pas à ouvrir une [issue](https://github.com/qomoxy/AI-TOWN-/issues) !

[↑ Retour en haut](#readme-top)
