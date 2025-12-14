import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

# Fonctions d'activation manuelles (car on n'utilise pas PyTorch/TensorFlow)
def sigmoid(x): return 1 / (1 + np.exp(-x))
def tanh(x): return np.tanh(x)

class LSTMSimulator:
    def __init__(self, filename):
        with open(filename, 'r') as f:
            # Lecture des dimensions
            dims = list(map(int, f.readline().split()))
            self.input_size = dims[0]
            self.hidden_size = dims[1]
            content = f.read().split()
        
        # Conversion des poids en float
        data = [float(x) for x in content]
        iterator = iter(data)

        def load_mat(rows, cols):
            mat = np.zeros((rows, cols))
            for i in range(rows):
                for j in range(cols):
                    mat[i][j] = next(iterator)
            return mat

        def load_vec(size):
            vec = np.zeros(size)
            for i in range(size):
                vec[i] = next(iterator)
            return vec

        # Chargement des matrices (Même ordre que dans LSTM.h saveBrain)
        h, i = self.hidden_size, self.input_size
        self.Wf = load_mat(h, i); self.Wi = load_mat(h, i); self.Wc = load_mat(h, i); self.Wo = load_mat(h, i)
        self.Uf = load_mat(h, h); self.Ui = load_mat(h, h); self.Uc = load_mat(h, h); self.Uo = load_mat(h, h)
        self.bf = load_vec(h);    self.bi = load_vec(h);    self.bc = load_vec(h);    self.bo = load_vec(h)

        # États cachés
        self.h = np.zeros(self.hidden_size)
        self.c = np.zeros(self.hidden_size)

    def forward(self, x):
        # Implémentation exacte du LSTM en NumPy
        x = np.array(x)
        
        ft = sigmoid(np.dot(self.Wf, x) + np.dot(self.Uf, self.h) + self.bf)
        it = sigmoid(np.dot(self.Wi, x) + np.dot(self.Ui, self.h) + self.bi)
        ct_tilde = tanh(np.dot(self.Wc, x) + np.dot(self.Uc, self.h) + self.bc)
        self.c = ft * self.c + it * ct_tilde
        ot = sigmoid(np.dot(self.Wo, x) + np.dot(self.Uo, self.h) + self.bo)
        self.h = ot * tanh(self.c)
        
        return self.h

def visualize_decision_boundary():
    # ATTENTION : Ce script suppose que vous avez SIMPLIFIÉ la vision à 11 entrées comme conseillé.
    # Si vous êtes encore à 151 entrées, il faudra remplir les autres avec des 0.
    
    brain = LSTMSimulator("best_brain.txt")
    
    # On va faire varier 2 paramètres clés :
    # Axe X : Distance nourriture (0 = sur place, 1 = loin)
    # Axe Y : Énergie (0 = mort, 1 = max)
    resolution = 50
    energies = np.linspace(0, 1, resolution)
    distances = np.linspace(0, 1, resolution)
    
    # Grille pour stocker l'action choisie
    decision_grid = np.zeros((resolution, resolution))
    
    print("Génération de la carte de décision...")
    
    for i, nrj in enumerate(energies):
        for j, dist in enumerate(distances):
            # On réinitialise la mémoire pour voir la réaction "immédiate"
            brain.h = np.zeros(brain.hidden_size)
            brain.c = np.zeros(brain.hidden_size)
            
            # --- Construction du vecteur d'entrée (Simulé) ---
            # Index 0: Energie, Index 1: Satisfaction, Index 4: Dist Nourriture
            # Les indices dépendent EXACTEMENT de votre fonction Agent::perceive
            
            inputs = np.zeros(brain.input_size)
            inputs[0] = nrj      # Énergie variable
            inputs[1] = 0.5      # Satisfaction moyenne fixe
            
            # Simulation : On dit à l'agent qu'il y a de la nourriture à 'dist'
            inputs[4] = dist     # Dist nourriture variable (Index supposé 4)
            inputs[5] = 0.0      # Angle 0 (devant)
            
            if dist < 0.1:       # Si très proche
                inputs[9] = 1.0  # "On Food" flag (Index supposé 9)
            
            # Passage dans le cerveau
            output = brain.forward(inputs)
            action = np.argmax(output) # 0=Manger, 1=Parler, 2=Dormir, etc.
            
            # On stocke l'action (y inversé pour l'affichage)
            decision_grid[resolution - 1 - i, j] = action

    # Affichage
    plt.figure(figsize=(10, 8))
    
    # Création d'une colormap personnalisée pour les actions
    # Actions : 0:Eat, 1:Talk, 2:Sleep, 3:Book, 4+:Move
    cmap = sns.color_palette("viridis", as_cmap=True)
    
    sns.heatmap(decision_grid, cmap="Pastel1", xticklabels=False, yticklabels=False)
    
    # Labels manuels
    plt.xlabel("Distance à la Nourriture (0 -> Loin)") # Attention à l'échelle inversée selon votre code
    plt.ylabel("Énergie (100 -> 0)")
    plt.title("Carte de Décision du Cerveau : Que faire ?")
    
    # Légende approximative (dépend de votre enum)
    print("Légende couleurs (approximative) :")
    print("Regardez les valeurs brutes pour correspondre à vos 'cases' switch(action_choice)")
    
    plt.savefig("brain_heatmap.png")
    plt.show()

if __name__ == "__main__":
    try:
        visualize_decision_boundary()
    except FileNotFoundError:
        print("Erreur : Lancez d'abord la simulation C++ pour générer 'best_brain.txt'")
    except Exception as e:
        print(f"Erreur : {e}")
        print("Vérifiez que input_size correspond bien à votre code C++.")
