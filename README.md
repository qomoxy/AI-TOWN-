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
    <li><a href="#fitness">Fitness</a></li>
    <li><a href="#cycle-d'évolution-et-sélection">Cycle d'Évolution et Sélection</a></li>
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
- **Élitisme (10%)** : Les meilleurs agents sont conservés.
- **Immigration (10%)** : Pour injecter de la diversité, 10% de la nouvelle population est composée d'agents totalement nouveaux avec un cerveau aléatoire.
- **Survivants aléatoires (5%)** : Quelques agents non-élites sont conservés pour préserver le patrimoine génétique
- **Renouvellement (85%)** : Le reste de la population est renouvelé par reproduction et mutation des agents élite sélectionnés.

Ce mécanisme permet de garantir à la fois une amélioration progressive des performances et une diversité suffisante pour éviter les optima locaux.
La mutation est évolutive, qui a pour but de secouer la population si sa fitness moyenne reste stable.

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

Avec la fonction run(n), vous pouvez accéder à un affichage rudimentaire de la simulation au sein de votre Terminal. Voici les différences symboles : 

- **@** : Agent (personnage)
- **.** : Case vide
- **~** : Mur (non franchissable)
- **T** : Forêt
- **A** : Pomme (comestible donne 20 d'énergie, 0.1% de chance de remplacer une case vide par tour)
- **C** : Champignon lumineux (comestible donne 20 d'énergie, apparaît que la nuit, même tôt de spawn que les pommes)

Le jour est affiché à chaque début d'un nouveau jour. La map est un carré de nxn.

### Actions et Récompenses

L'agent a le choix de faire différentes actions en fonction de l'environnement qui l'entoure, il a un champ de vision de 4x4 autour de lui. Une action par tour.


<ol>
  <li> <b>Manger</b> : Si une case avec un comestible est non loin de lui, il peut la consommer pour obtenir 20 points d'énergie.</li>
  <li> <b>Parler</b> : Si un agent est proche, il peut lui parler pour gagner de la satisfaction. Ce gain est mutuel et dépend de leur score social réciproque : une bonne entente résulte en un gain élevé pour les deux agents, tandis qu'une mauvaise entente ne donne que peu ou pas de la satisfaction.</li>
  <li> <b>Dormir</b> : Donne 5 énergies la nuit.</li>
  <li> <b>Déplacement</b> : Se déplace de trois cases au max.</li>
</ol>

**Satisfaction** : un score qui commence à 25, qui ne peut pas dépasser 100. <br>
**Énergie** : un score qui commence à 50, qui ne peut dépasser 100 et qui peut être négatif. A la fin de chaque tour, l'agent perd 1 point d'énergie si c'est le jour, sinon 1.5.

### Données 

A la fin de la simulation, un fichier .csv sera présent sous le nom de **simulation_log.csv** avec dedans des données comme la fitness moyenne, l'énergie et la satisfaction moyenne durant les jours de la simulation.

### Sources Utilisée

Blog de Colah : https://colah.github.io/posts/2015-08-Understanding-LSTMs/

## A venir :
Le résultat de nos observations avec des données précises et intéressantes d'une simulation.
L'ajout d'un fichier configuration pour stocker l'ensemble des paramètres bruts, pour une meilleure lisibilité et plus simple à modifier.
Meullier gestion de la nourriture et cases.

**Peut-être** : interaction sociale négative quand elles sont ratées ou que le score est bas, et des actions antisociales comme manger la pomme d'un autre. 
Meullier gestion de last food position
Cycle de vie ou de mort avec reproduction intégrée.

---

> Pour toute suggestion ou question, n’hésitez pas à ouvrir une issue !

<a href="#readme-top">Retour en haut</a>



