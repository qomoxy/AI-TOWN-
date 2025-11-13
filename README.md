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

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table des matières</summary>
  <ol>
    <li><a href="#contexte-et-motivation">Contexte et motivation</a></li>
      <li><a href="#objectifs">Objectifs</a></li>
    <li><a href="#choix-du-modèle">Choix du modèle</a></li>
    <li><a href="#lstm">LSTM</a></li>
    <li><a href="#implémentation">Implémentation</a></li>
    <li><a href="#fitness-et-itérations">Fitness et Itérations</a></li>
    <li><a href="#cycle-d'évolution-et-sélection">Cycle d'Évolution et Sélection</a></li>
    <li><a href="#résultats-et-observations-clés">Résultats et Observations Clés</a></li>
    <li><a href="#pour-commencer">Pour commencer</a></li>
    <li><a href="#documentation">Documentation</a></li>
  </ol>
</details>

## Contexte et motivation

Dans un monde où le numérique occupe une place centrale, l’intelligence artificielle (IA) suscite un intérêt croissant. Ce projet vise à explorer comment une population d’agents virtuels évolue et interagit dans un environnement social simulé. L’objectif est de comprendre les dynamiques sociales et comportementales qui émergent de ces interactions, tout en abordant les enjeux liés à la modélisation de l’intelligence artificielle. Ce projet aborde le thème 'Cycle et boucles' à travers le cycle d'évolution darwinienne (sélection, reproduction) et les boucles de rétroaction du réseau de neurones récurrents (LSTM) qui gèrent la mémoire et la prise de décision des agents.

## Objectifs

- **Concevoir un environnement virtuel** où des agents évoluent librement.
- **Développer des agents intelligents** capables d’interactions complexes entre eux et avec leur environnement.
- **Définir une fonction de fitness** pertinente pour évaluer la performance des agents.
- **Étudier l’évolution des agents** à travers des mesures fiables et reproductibles.

## Choix du modèle

### Contraintes du projet :

- **Autonomie** : Réaliser l’ensemble du projet nous-mêmes.
- **Ressources** : Utiliser uniquement un CPU (sans NPU ou GPU).
- **Temporalité** : Finaliser le projet en moins d’une année scolaire.

### Besoins identifiés :

- Des agents capables de s’adapter, d’interagir et d’évoluer en société.
- Des indicateurs de performance pour mesurer leur évolution individuelle et collective.
- Un système permettant l’évolution et l’acclimatation des agents à leur environnement.
- Un environnement apte à accueillir et faire évoluer cette population d’agents.

Pour permettre à nos agents de simuler des comportements proches de l’humain, nous avons choisi d’utiliser des **réseaux de neurones récurrents**, et plus précisément les **LSTM** (Long Short-Term Memory), afin de gérer la mémoire et le contexte temporel.

## LSTM

Les LSTM, introduits par **Hochreiter & Schmidhuber** et popularisés par **Christopher Olah**, sont des réseaux de neurones récurrents dotés de plusieurs portes internes :
- **Porte d’oubli** : décide quelles informations supprimer de la mémoire.
- **Porte d’entrée** : sélectionne les nouvelles informations à mémoriser.
- **Porte de sortie** : détermine l’état de sortie du neurone.

Ce mécanisme permet de modéliser des comportements séquentiels et d’intégrer une mémoire à long terme, parfaitement adaptés à notre problématique : simuler des agents capables d’apprentissage et d’évolution dans un environnement social.

## Implémentation

L’implémentation est réalisée en C++ pour garantir rapidité et maîtrise de la mémoire.
- Traduction des équations mathématiques des LSTM en code C++ optimisé.
- Création d’agents virtuels intégrant un réseau LSTM pour la prise de décision et l’apprentissage.
- Développement d’un environnement simulé permettant l’observation et l’analyse des interactions sociales.
- Tests et validations pour assurer la robustesse et la fiabilité du modèle.

## Fitness

Les agents sont évalués à l'aide d'un score appelé "*fitness*", , qui prend en compte deux paramètres essentiels :
- **Énergie** : Représente la capacité à effectuer des actions.
- **Satisfaction** : Reflète le bien-être de l’agent (besoins sociaux, intellectuels).

Au cours de nos expériences, le choix de la fonction de fitness s'est avéré être le paramètre le plus critique. Une première approche additive a mené à un échec : l'évolution favorisait des agents inactifs avec une satisfaction élevée, mais une énergie nulle, des "cadavres satisfaits" :
<br>
<p align='center'><b>Fitness = énergie × w₁ + satisfaction × w₂</b></p>
<br>
où w₁ = 0.4 et w₂ = 0.6. 

Nous avons donc opté pour une **formule multiplicative** qui lie la survie et le bien-être de manière indissociable :

<p align='center'><b>Fitness = (énergie + 1) × (satisfaction + 1)</b></p>

Avec cette fonction, un agent doit obligatoirement maintenir ses deux statistiques à un niveau élevé pour être considéré comme performant, ce qui a permis de résoudre le problème de la mortalité de la population.
Le <b>+1</b> a été ajouté à chaque paramètre pour garantir que la fitness reste positive ou nulle, et pour éviter qu'une énergie ou une satisfaction nulle ne coupe instantanément toute chance de survie, forçant ainsi l'agent à maintenir un niveau minimal dans les deux statistiques.



## Cycle d'Évolution et Sélection

Pour simuler une évolution darwinienne, un cycle de sélection a lieu tous les cinq jours (temps simulé) pour créer une nouvelle génération. Les pourcentages sont ajustés pour maximiser à la fois la performance et la diversité génétique :
- **Élitisme (10%)** : Les meilleurs agents sont conservés tels quels.
- **Immigration (10%)** : Pour injecter de la nouveauté, 10% de la population est composée d'agents avec un cerveau totalement aléatoire.
- **Survivants aléatoires (5%)** : Quelques agents non-élites sont conservés pour préserver des gènes potentiellement utiles.
- **Renouvellement (75%)** : Le reste de la population est généré par croisement et mutation des agents sélectionnés.   

Ce mécanisme permet de garantir à la fois une amélioration progressive des performances et une diversité suffisante pour éviter les optima locaux.
La mutation est adaptative : son taux augmente si la fitness moyenne de la population stagne, afin de "secouer" l'évolution et d'éviter les optima locaux.

## Résultats et Observations Clés

Nos simulations sur de longues durées (jusqu'à 50 000 jours) nous ont permis d'observer plusieurs comportements émergents fascinants :

  - La Crise de Survie Initiale : La première version de la fonction de fitness a conduit à l'extinction systématique de la population, démontrant l'importance cruciale de bien définir les objectifs de l'évolution.

   - La Société Éphémère : Avec la fonction de fitness corrigée, la population est devenue viable. Cependant, l'analyse du réseau social a révélé une très forte mortalité et des liens sociaux faibles et rares. Les agents formaient des "paires de survie" opportunistes plutôt qu'une société structurée.

  - L'Émergence de la Stabilité : En ajustant les paramètres de l'environnement (coût de la vie réduit, nourriture plus abondante, gain de satisfaction en mangeant), nous avons finalement obtenu une population stable sur 50 000 jours, avec une énergie et une satisfaction moyennes positives, et un réseau social actif.

Ces étapes illustrent une démarche itérative où l'analyse des résultats permet de poser de nouvelles hypothèses et d'affiner le modèle pour faire émerger des comportements plus complexes. Les fichiers ```.csv``` sont dispo dans test.

<img width="1253" height="930" alt="Screenshot from 2025-09-27 19-09-44" src="https://github.com/user-attachments/assets/a3c6c088-7408-446e-abb0-487b92e423a0" />

Figure 1: Évolution des relation (ligne bleue) entre les agents sur 50 000 jours, illustrant leur liens.


## Pour commencer

### Prérequis

- Un compilateur C++ supportant la norme C++17 (ex: g++, clang++) :
  ```shell
  # Sur Debian/Ubuntu
  sudo apt-get install build-essential
  ```

### Installation

1. Clonez ce dépôt sur votre machine locale :
    ```shell
    git clone https://github.com/qomoxy/AI-TOWN-.git
    cd AI-TOWN-
    ```

2. Compilez le projet et lancez l'exécutable :
    ```shell
    g++ -std=c++17 -o ai-town main.cpp agent.cpp monde.cpp simulation.cpp
    ./ai-town
    ```

    Avant de lancer le projet, il est conseiller de lire le ```main.cpp``` : 

    - ```run()``` : Pour une simulation avec affichage graphique.
    - ```fast_run()``` : Pour une simulation sans affichage. 

## Documentation

### Structure du projet

Voici la structure des fichiers sources principaux :

```
  .
  ├── Agent.h
  ├── agent.cpp
  ├── LSTM.h
  ├── main.cpp
  ├── monde.cpp
  ├── monde.h 
  ├── simulation.cpp
  ├── simulation.h
  └── README.md           # Vous êtes ici !
```

- **main.cpp** : Point d’entrée du programme.
- **Agent.h / agent.cpp** : Définition et implémentation des agents.
- **LSTM.h** : Implémentation du réseau de neurones LSTM.
- **monde.h / monde.cpp** : Gestion de l’environnement virtuel.
- **simulation.h / simulation.cpp** : Logique de la simulation.

### Affichage 

Avec la fonction ```run()```, vous pouvez accéder à un affichage de la simulation dans votre Terminal. Voici les symboles :

- **@** : Agent 
- **.** : Case vide
- **~** : Eau (infranchissable)
- **T** : Forêt
- **A** : Pomme (comestible)
- **C** : Champignon lumineux (comestible, apparaît la nuit)
- **B** : Livre (augmente la satisfaction)

### Actions et Récompenses

L'agent perçoit son environnement et choisit une action à chaque tour.

<ol>
  <li> <b>Manger</b> : Consomme une ressource pour gagner de l'énergie et de la satisfaction.</li>
  <li> <b>Parler</b> : Interagit avec un autre agent pour un gain de satisfaction mutuel, basé sur leur relation.</li>
  <li> <b>Dormir</b> : Permet de regagner de l'énergie, principalement la nuit.</li>
  <li> <b>Lire</b> : Interagir avec un livre pour un gain de satisfaction important.</li>
  <li> <b>Se déplacer</b> : Explore la carte.</li>
</ol>

**Satisfaction** : un score qui commence à 10, qui ne peut pas dépasser 100. <br>
**Énergie** : un score qui commence à 75, qui ne peut dépasser 100 et qui peut être négatif. A la fin de chaque tour, l'agent perd 1 point d'énergie si c'est le jour, sinon 1.5.

### Données 

À la fin de chaque simulation, deux fichiers ```.csv``` sont générés :

- ```simulation_log.csv```. : Contient les moyennes de fitness, d'énergie et de satisfaction pour chaque jour.

- ```social_log.csv``` : Enregistre toutes les interactions sociales, permettant de reconstruire et d'analyser le réseau social.

### Info complémentaire 

L'ensemble de ces informations sont contenues dans le code. Voici une liste non-exhaustive de détails.

#### Mécanique 

<ol>
  <li><b>Pomme</b> : Donne 40 d'énergie et 2 de satisfaction. Prends 50 tours à repousser, une fois mangée la casse devient vide (Empty : <b>.</b>) Puis redeviens une pomme.</li> 
  <li><b>Champignon Lumineux</b> : Pousse uniquement la nuit, donne 60 d'énergie et 2 de satisfaction. Prends 150 tours à repousser, une fois mangée la case redevient un arbre.</li>
  <li><b>Communication</b> : On a opté pour une version simplifiée qui donne 5 de satisfaction à chaque agent, et augmente leur score social de 1. On a fait le choix d'enlever toute la partie aléatoire car elle nous permet de mesurer le nombre d'interactions, donc on a simplifié. Et il ne partage plus le last_food_postion car souvent il a été consommé et l'agent en a déjà trouvé. </li>
  <li> <b>Dormir</b>: Lui faire passer le tour en échange de 25 d'énergie la nuit, mais le jour il perd 1 de pénalité sur son énergie. (La procrastination est bannie). </li>
  <li><b>Déplacment</b> : Lui permet de bouger de 3 cases. </li>
</ol>

#### Map

La map fait du ```nxn``` est entièrement remplie de casse, puis les agents sont positionnés sur la map, ils sont placés aléatoirement sans être surperposés. La map est mise à jour chaque tour. Il y a 50 tours par jour, 25 de jour et 25 de nuit. 
Le monde est généré aléatoirement.



### Sources Utilisée

Blog de Colah : https://colah.github.io/posts/2015-08-Understanding-LSTMs/

## A venir :
L'ajout d'un fichier configuration pour stocker l'ensemble des paramètres bruts, pour une meilleure lisibilité et plus simple à modifier.

**Peut-être** :
Cycle de vie ou de mort avec reproduction intégrée.

---

> Pour toute suggestion ou question, n’hésitez pas à ouvrir une issue !

<a href="#readme-top">Retour en haut</a>



