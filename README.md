<a name="readme-top"></a>

<div align="center">
  <h3 align="center">Sociét IA</h3>

  <p align="center">
    L'étude de l'évolution d'une population dans un milieux sociale. 
    <br/>
  </p>
  <p align="center">
    Bellot Alexis & Goursaud Quentin
    <br/>
  </p>

  <p align="center">
    TIPE 205 - 2026
    <br/>
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Tables des matières</summary>
  <ol>
    <li>
      <a href="#contexte-et-motivation">Context et motivation</a>
    </li>
    <li><a href="#objectif">Objectif</a></li>
    <li><a href="#rprérequis">Prérequis</a></li>
    <li><a href="#LSTM">LSTM</a></li>
    <li><a href="#implémentation">Implmentation</a></li>
  </ol>
</details>

## Contexte et motivation 

  Dans un monde où le numérique est l'outil et domaine d'étude. Certain domaine et recherche sucité l'atteret du plus grand nombre. L'intelligence artificiel est le domaine d'étude qui fait réver les masses et les terrifient. Face à ces cerveau dit artificiel où l'homme modéle sons raisonement et son monde de prédilection, une idée est née. Comment des agents évolutrait t'il dans une société primaire ? Et comment les étudier ? Avec l'évolution flagrante des modéles de conversation (tel que Chat gpt ou Gemini) qui nous vendent monts et merveilles avec leurs promesse et benchMarck. Essayons de créer avec nos outils une intelligence artificiel en capacatié de survire dans une milieu primaire, et de voir si il existe une méthode analitique pour l'étudier et peut être comprendre ces faits et gestes. 

## Objectif

* Concevoir un monde virtuel où l'agent pourrait évoluer librement.
* Avoir des agents intelligent et capabler d'interagir entre eux et avec le monde qui les entoures.
* Chercher une fonction de calcul de la fitness interresante.
* Etudier leur évolution en sain du monde avec des mesures fiable.

## Le choix du modéles

Il nous faut dans un premier temps identifier contraites du projets : 

* Tout faire nous même.
* Sans NPU ou GPU.
* Finir le projet en moins d'un an.

Après cela, il nous faut souligner nos beoins : un agent capable d'évoluer dans une monde primaire en société ça implique quoi : 

* Capacité à interagir avec le monde et les autres agents.
* Des scores pour indiquer la perfomance de l'agent dans sont monde et ça sociéter.
* Evolution des agent au fil du temps, aclimatation a leur milieu de vie.
* Un monde capable d'accuillir les agent.

Cependant nous avions envie de rendre le plus intelligent possible notre agent tout en restant proche de l'être humain. L'option qui nous à parut la plus interresante parmis les différent grand modeles d'IA était le deep learning, implémenté des couches de neuronnes. Mais il manquait quelque chose qui allait rajouté une profondeur à la simulation de société : la mémoire. Elle permet différente perpective bien plus complexe tel sur les aspect sociale et de survie. Nous avons commencé par explorer les différentes formes d’IA, en particulier celles capables de gérer des données temporelles ou séquentielles.  Les réseaux de neurones récurrents, ou RNN, se sont vite imposés, car ils permettent de prendre en compte le contexte passé grâce à une mémoire interne. Cependant, les RNN classiques ont des limites, notamment lorsqu’il s’agit de retenir des informations sur le long terme. C’est là qu’interviennent les LSTM, ou Long Short-Term Memory. 

A fin de comprendre, il nous zoomer sur un neuronnes récurrent. Un neurone récurrent est un neurone qui, en plus de recevoir une entrée, prend aussi en compte l’état précédent du réseau.
 Cela permet de traiter des séries temporelles, comme du texte, des sons, ou des suites d’actions.
Cependant, comme dit précédemment, il oublie vite. C’est pour ça que l’on va s’intéresser aux LSTM, qui intègrent un mécanisme de filtrage de l'information.

## LSTM 

Les LSTM, proposés dans le blog de Christopher Olah, sont des réseaux qui intègrent plusieurs portes internes :
* Une porte d’oubli : qui décide quelles informations supprimer.
* Une porte d’entrée : pour savoir ce qu’il faut retenir.
* Et une porte de sortie, qui détermine l’état de sortie.


Ce mécanisme leur permet de modéliser des cycles comportementaux sur le long terme, ce qui est parfaitement adapté à notre problématique.
    
## Implémentation 

Pour l’implémentation, nous avons utilisé le langage C++ pour la rapidité d'exécution et la maîtrise de la mémoire.
 Nous avons traduit les équations mathématiques du LSTM et tenté de créer une version fonctionnelle en code.
Cette partie a été complexe car il fallait à la fois comprendre les formules, souvent issues de l’analyse matricielle, coder les fonctions, et vérifier que tout fonctionnait avec les bons types de données.



