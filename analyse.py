# -*- coding: utf-8 -*-
"""
Script d'analyse et de visualisation du réseau social des agents.

Version améliorée : Ce script analyse une FENÊTRE de temps (les 5000
derniers jours) pour obtenir une vue plus stable et représentative
des liens sociaux forts, plutôt qu'un simple instantané.
"""

import pandas as pd
import networkx as nx
import matplotlib.pyplot as plt
import sys

# Constante pour la période d'analyse
ANALYSIS_WINDOW_DAYS = 5000

def analyze_social_network(log_file="social_log.csv"):
    """
    Analyse le fichier de log social et génère une visualisation du réseau.
    """
    try:
        print(f"Lecture du fichier '{log_file}'...")
        df = pd.read_csv(log_file)
    except FileNotFoundError:
        print(f"ERREUR : Le fichier '{log_file}' n'a pas été trouvé.")
        print("Assurez-vous que le script est dans le même dossier que le fichier CSV.")
        sys.exit(1)

    if df.empty:
        print("Le fichier de log est vide. Aucune interaction sociale à analyser.")
        return

    # 2. On isole les données de la période d'analyse
    last_day = df['jour'].max()
    start_day = max(0, last_day - ANALYSIS_WINDOW_DAYS)
    
    print(f"Analyse du réseau sur la période [Jour {start_day} -> Jour {last_day}]...")
    df_recent = df[df['jour'] >= start_day].copy()

    if df_recent.empty:
        print(f"Aucune interaction n'a eu lieu dans les {ANALYSIS_WINDOW_DAYS} derniers jours.")
        return

    # 3. On agrège les scores pour obtenir la force totale des liens
    # On s'assure que (A,B) et (B,A) sont comptés comme la même relation
    df_recent['agent_pair'] = df_recent.apply(lambda row: tuple(sorted((row['agent_source_id'], row['agent_cible_id']))), axis=1)
    
    # On groupe par paire et on somme les scores
    final_scores = df_recent.groupby('agent_pair')['score_relation'].sum().reset_index()

    # Création d'un graphe non-orienté
    G = nx.Graph()

    # 4. Ajout des relations (arêtes) dans le graphe
    for _, row in final_scores.iterrows():
        agent1, agent2 = row['agent_pair']
        total_score = row['score_relation']
        
        if total_score > 0: # On ajoute seulement les liens non-nuls
             G.add_edge(int(agent1), int(agent2), weight=total_score)

    if G.number_of_nodes() == 0:
        print("Aucun agent n'a interagi sur la période. Impossible de générer un graphe.")
        return
        
    # 5. Analyse du réseau : trouver les agents les plus populaires
    degrees = dict(G.degree())
    sorted_degrees = sorted(degrees.items(), key=lambda item: item[1], reverse=True)
    
    print("\n--- Analyse des Agents ---")
    print(f"Nombre total d'agents avec des interactions : {G.number_of_nodes()}")
    print("Top 5 des agents les plus populaires (par nombre de relations) :")
    for i, (agent_id, degree) in enumerate(sorted_degrees[:5]):
        print(f"  {i+1}. Agent {agent_id} avec {degree} relations.")

    # 6. Préparation de la visualisation
    plt.figure(figsize=(14, 10))
    plt.title(f"Réseau Social Agrégé (Jours {start_day}-{last_day})", fontsize=20)

    pos = nx.spring_layout(G, k=0.9, iterations=50)

    node_sizes = [v * 100 for v in degrees.values()]
    edge_weights = [G[u][v]['weight'] * 0.1 for u, v in G.edges()] # facteur de mise à l'échelle ajusté

    nx.draw_networkx_nodes(G, pos, node_size=node_sizes, node_color='skyblue', alpha=0.9)
    nx.draw_networkx_edges(G, pos, width=edge_weights, edge_color='gray', alpha=0.6)
    nx.draw_networkx_labels(G, pos, font_size=10, font_family='sans-serif')
    
    print("\nFermez la fenêtre du graphique pour terminer le script.")
    plt.show()


if __name__ == "__main__":
    analyze_social_network()


