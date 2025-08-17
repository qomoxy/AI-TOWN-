<a name="readme-top"></a>

<div align="center">
  <h3 align="center">Société IA</h3>


  <p align="center">
    Étude de l'évolution d'une population au sein d'un milieu social virtuel.<br>
    <br>
    <b>Bellot Alexis & Goursaud Quentin</b>
    <br>
    TIPE 2025 - 2026
    <br>
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table des matières</summary>
  <ol>
    <li><a href="#contexte-et-motivation">Contexte et motivation</a></li>
    <li><a href="#objectif">Objectifs</a></li>
    <li><a href="#choix-du-modèle">Choix du modèle</a></li>
    <li><a href="#lstm">LSTM</a></li>
    <li><a href="#implémentation">Implémentation</a></li>
  </ol>
</details>

## Contexte et motivation 

Dans un monde où le numérique occupe une place centrale, l’intelligence artificielle (IA) suscite un intérêt croissant. Ce projet vise à explorer la façon dont une population d’agents virtuels évolue et interagit dans un environnement social simulé. L’objectif est de comprendre les dynamiques sociales et comportementales qui émergent de ces interactions, tout en abordant les enjeux liés à la modélisation de l’intelligence artificielle.

## Objectifs

- Concevoir un environnement virtuel permettant à des agents d’évoluer librement.
- Développer des agents intelligents capables d’interagir entre eux et avec leur environnement.
- Définir une fonction de calcul de fitness pertinente pour évaluer la performance des agents.
- Étudier l’évolution des agents à travers des mesures fiables et reproductibles.


## Choix du modèle

### Contraintes du projet :
- Réaliser l’ensemble du projet nous-mêmes.
- Utiliser uniquement un CPU (sans NPU ou GPU).
- Finaliser le projet en moins d’un an.

### Besoins identifiés :
- Des agents capables de s’adapter et d’interagir dans un monde primaire et en société.
- Des indicateurs de performance pour mesurer l’évolution individuelle et collective.
- Un système permettant l’évolution des agents et leur acclimatation à leur environnement.
- Un environnement apte à accueillir et faire évoluer la population d’agents.

Afin de rendre nos agents aussi intelligents que possible tout en s’inspirant du fonctionnement humain, nous avons choisi d’utiliser des réseaux de neurones récurrents, et plus précisément les LSTM (Long Short-Term Memory), pour leur capacité à gérer la mémoire et le contexte temporel.

## LSTM

Les LSTM, introduits par Hochreiter & Schmidhuber et popularisés par Christopher Olah, sont des réseaux de neurones récurrents dotés de plusieurs portes internes :
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

---

> Pour toute suggestion ou question, n’hésitez pas à ouvrir une issue !

<a href="#readme-top">Retour en haut</a>



