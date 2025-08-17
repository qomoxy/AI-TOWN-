<a name="readme-top"></a>

<div align="center">
  <h3 align="center">Sociét IA — TIPE 205 (2026)</h3>

  <p align="center">
    Étude de l'évolution d'une population au sein d'un milieu social virtuel.<br>
    <br>
    <b>Bellot Alexis & Goursaud Quentin</b>
    <br>
    TIPE 205 - 2026
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
    <li><a href="#fitness">Fitness</a></li>
    <li><a href="#cycle-d'Évolution-et-sélection">Cycle d'Évolution et Sélection</a></li>
    <li><a href="#installation">Installation</a></li>
    <li><a href="#documentation">Documentation</a></li>
  </ol>
</details>

## Contexte et motivation

Dans un monde où le numérique occupe une place centrale, l’intelligence artificielle (IA) suscite un intérêt croissant. Ce projet vise à explorer comment une population d’agents virtuels évolue et interagit dans un environnement social simulé. L’objectif est de comprendre les dynamiques sociales et comportementales qui émergent de ces interactions, tout en abordant les enjeux liés à la modélisation de l’intelligence artificielle.

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

Les agents sont évalués à l'aide d'un score appelé "*fitness*", qui prend en compte deux paramètres essentiels à leur survie :
- **Énergie** : Représente la capacité à effectuer des actions. Elle diminue lors des déplacements et augmente pendant le repos.
- **Satisfaction** : Reflète le bien-être de l’agent dans la simulation. Elle augmente, par exemple, lorsqu’il mange.

La formule utilisée est :
<br>
<p align='center'><b>Fitness = énergie × w₁ + satisfaction × w₂</b></p>
<br>
où w₁ = 0.4 et w₂ = 0.6.

## Cycle d'Évolution et Sélection

Pour simuler une évolution darwinienne, un cycle de sélection a lieu tous les 5 jours (temps simulé) pour créer une nouvelle génération :
- **Élitisme (10%)** : Les 10% des agents ayant le meilleur score de fitness sont conservés tels quels pour la génération suivante.
- **Aléatoire (5%)** : 5% des agents sont sélectionnés au hasard pour préserver la diversité génétique.
- **Renouvellement (85%)** : Le reste de la population est renouvelé par reproduction et mutation des agents sélectionnés.

Ce mécanisme permet de garantir à la fois une amélioration progressive des performances et une diversité suffisante pour éviter les optima locaux.

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
    git clone https://github.com/qomoxy/AI-TOWN-
    cd AI-TOWN-
    ```

2. Compilez le projet et lancez l'exécutable :
    ```shell
    g++ -std=c++17 -o ai-town main.cpp agent.cpp monde.cpp simulation.cpp
    ./ai-town
    ```

## Documentation

### Structure du projet

Voici la structure des fichiers sources principaux

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


---

> Pour toute suggestion ou question, n’hésitez pas à ouvrir une issue !

<a href="#readme-top">Retour en haut</a>



