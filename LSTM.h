#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>   
#include <sstream>   
#include <random>
#include <algorithm>
#include <tuple>

// La structure pour contenir tous les poids du LSTM
struct LSTM_weight { 
    std::vector<std::vector<double>> Wf, Wi, Wc, Wo;
    std::vector<std::vector<double>> Uf, Ui, Uc, Uo;
    std::vector<double> bf, bi, bc, bo;
}; 

class LSTM {
private:
    // Paramètres
    int input_size;
    int hidden_size;

    LSTM_weight weights;

    // États cachés
    std::vector<double> h_prev;
    std::vector<double> c_prev;

    // Fonctions d'activation
    double sigmoid(double x) {
        return 1.0 / (1.0 + std::exp(-x));
    }

    double tanh_act(double x) { 
        return std::tanh(x);
    }

public:
    std::default_random_engine generator; 

    LSTM(int input_size, int hidden_size) 
        : input_size(input_size), hidden_size(hidden_size) {
            h_prev = std::vector<double>(hidden_size, 0.0);
            c_prev = std::vector<double>(hidden_size, 0.0);
            initialize_weights();
        }

    void initialize_weights() {
        std::normal_distribution<double> distribution(0, sqrt(1.0/input_size)); 

        auto init_matrix = [&](int rows, int cols) {
            std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols));
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    matrix[i][j] = distribution(generator);
                }
            }
            return matrix;
        };

        auto init_vector = [&](int size) {
            std::vector<double> vec(size);
            for (int i = 0; i < size; ++i) {
                vec[i] = distribution(generator);
            }
            return vec;
        };
        
        weights.Wf = init_matrix(hidden_size, input_size);
        weights.Wi = init_matrix(hidden_size, input_size);
        weights.Wc = init_matrix(hidden_size, input_size);
        weights.Wo = init_matrix(hidden_size, input_size);

        weights.Uf = init_matrix(hidden_size, hidden_size);
        weights.Ui = init_matrix(hidden_size, hidden_size);
        weights.Uc = init_matrix(hidden_size, hidden_size);
        weights.Uo = init_matrix(hidden_size, hidden_size);

        weights.bf = init_vector(hidden_size);
        weights.bi = init_vector(hidden_size);
        weights.bc = init_vector(hidden_size);
        weights.bo = init_vector(hidden_size);
    }

    // getters
    LSTM_weight getWeights() const { 
        return this->weights;
    }

    int getInputSize() const {
        return input_size;
    }

    int getHiddenSize() const {
        return hidden_size;
    }

    std::vector<double> getC_prev() const {
        return c_prev;
    }

    std::vector<double> getH_prev() const {  
        return h_prev;
    }

    void setWeights(const LSTM_weight& new_weights) { 
        this->weights = new_weights;
    }

    std::vector<double> forward(const std::vector<double>& x) {
        std::vector<double> f_gate(hidden_size);
        std::vector<double> o_gate(hidden_size);
        std::vector<double> i_gate(hidden_size);
        std::vector<double> c_tilde(hidden_size);
        std::vector<double> h_next(hidden_size);
        std::vector<double> c_next(hidden_size);

        // Forget gate
        for (int i = 0; i < hidden_size; ++i) {
            double sum = 0.0;
            for (int j = 0; j < input_size; ++j) sum += weights.Wf[i][j] * x[j];
            for (int j = 0; j < hidden_size; ++j) sum += weights.Uf[i][j] * h_prev[j];
            f_gate[i] = sigmoid(sum + weights.bf[i]);
        }

        // Input gate
        for (int i = 0; i < hidden_size; ++i) {
            double sum = 0.0;
            for (int j = 0; j < input_size; ++j) sum += weights.Wi[i][j] * x[j];
            for (int j = 0; j < hidden_size; ++j) sum += weights.Ui[i][j] * h_prev[j];
            i_gate[i] = sigmoid(sum + weights.bi[i]);
        }

        // Candidate cell state
        for (int i = 0; i < hidden_size; ++i) {
            double sum = 0.0;
            for (int j = 0; j < input_size; ++j) sum += weights.Wc[i][j] * x[j];
            for (int j = 0; j < hidden_size; ++j) sum += weights.Uc[i][j] * h_prev[j];
            c_tilde[i] = tanh_act(sum + weights.bc[i]);
        }

        // Cell state update
        for (int i = 0; i < hidden_size; ++i) {
            c_next[i] = f_gate[i] * c_prev[i] + i_gate[i] * c_tilde[i];
        }

        // Output gate
        for (int i = 0; i < hidden_size; ++i) {
            double sum = 0.0;
            for (int j = 0; j < input_size; ++j) sum += weights.Wo[i][j] * x[j];
            for (int j = 0; j < hidden_size; ++j) sum += weights.Uo[i][j] * h_prev[j];
            o_gate[i] = sigmoid(sum + weights.bo[i]);
        } 

        // Hidden state
        for (int i = 0; i < hidden_size; ++i) {
            h_next[i] = o_gate[i] * tanh_act(c_next[i]);
        }

        c_prev = c_next;
        h_prev = h_next;

        return h_next;
    }

    void mutate(double mutateRate) {
        std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
        std::uniform_real_distribution<double> mutation_dist(-0.05, 0.05);

        auto mutate_vector = [&](std::vector<double>& vec) {
            for (double& value : vec) {
                if (prob_dist(generator) < mutateRate) {
                    value += mutation_dist(generator);
                }
            }
        };

        auto mutate_matrix = [&](std::vector<std::vector<double>>& matrix) {
            for (auto& row : matrix) {
                mutate_vector(row);
            }
        };
        
        mutate_matrix(weights.Wf);
        mutate_matrix(weights.Wi);
        mutate_matrix(weights.Wc);
        mutate_matrix(weights.Wo);
        mutate_matrix(weights.Uf);
        mutate_matrix(weights.Ui);
        mutate_matrix(weights.Uc);
        mutate_matrix(weights.Uo);
        
        mutate_vector(weights.bf);
        mutate_vector(weights.bi);
        mutate_vector(weights.bc);
        mutate_vector(weights.bo);
    }

    LSTM breed(const LSTM& parent) const {
        LSTM child(this->input_size, this->hidden_size);
        LSTM_weight child_weights;
        
        const LSTM_weight& parent_weights = parent.getWeights();

        auto breed_vector = [&](const std::vector<double>& v1, const std::vector<double>& v2) {
            std::vector<double> result(v1.size());
            for (size_t i = 0; i < v1.size(); ++i) {
                result[i] = (v1[i] + v2[i]) / 2.0;
            }
            return result;
        };
        
        auto breed_matrix = [&](const std::vector<std::vector<double>>& m1, const std::vector<std::vector<double>>& m2) {
            std::vector<std::vector<double>> result(m1.size());
            for (size_t i = 0; i < m1.size(); ++i) {
                result[i] = breed_vector(m1[i], m2[i]);
            }
            return result;
        };

        child_weights.Wf = breed_matrix(this->weights.Wf, parent_weights.Wf);
        child_weights.Wi = breed_matrix(this->weights.Wi, parent_weights.Wi);
        child_weights.Wc = breed_matrix(this->weights.Wc, parent_weights.Wc);
        child_weights.Wo = breed_matrix(this->weights.Wo, parent_weights.Wo);
        
        child_weights.Uf = breed_matrix(this->weights.Uf, parent_weights.Uf);
        child_weights.Ui = breed_matrix(this->weights.Ui, parent_weights.Ui);
        child_weights.Uc = breed_matrix(this->weights.Uc, parent_weights.Uc);
        child_weights.Uo = breed_matrix(this->weights.Uo, parent_weights.Uo);

        child_weights.bf = breed_vector(this->weights.bf, parent_weights.bf);
        child_weights.bi = breed_vector(this->weights.bi, parent_weights.bi);
        child_weights.bc = breed_vector(this->weights.bc, parent_weights.bc);
        child_weights.bo = breed_vector(this->weights.bo, parent_weights.bo);

        child.setWeights(child_weights);

        return child;
    }


    void saveBrain(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Erreur : impossible d'ouvrir " << filename << " pour sauvegarder le cerveau" << std::endl;
            return;
        }

        // Sauvegarder les dimensions du réseau
        file << "# LSTM Brain Save File\n";
        file << "# Input Size: " << input_size << ", Hidden Size: " << hidden_size << "\n";
        file << input_size << " " << hidden_size << "\n";

        // Helper pour sauvegarder une matrice
        auto save_matrix = [&](const std::vector<std::vector<double>>& matrix, const std::string& name) {
            file << "# " << name << "\n";
            for (const auto& row : matrix) {
                for (const auto& val : row) {
                    file << val << " ";
                }
                file << "\n";
            }
        };

        // Helper pour sauvegarder un vecteur
        auto save_vector = [&](const std::vector<double>& vec, const std::string& name) {
            file << "# " << name << "\n";
            for (const auto& val : vec) {
                file << val << " ";
            }
            file << "\n";
        };

        // Sauvegarder toutes les matrices W (input → hidden)
        save_matrix(weights.Wf, "Wf (Forget Gate - Input to Hidden)");
        save_matrix(weights.Wi, "Wi (Input Gate - Input to Hidden)");
        save_matrix(weights.Wc, "Wc (Cell Gate - Input to Hidden)");
        save_matrix(weights.Wo, "Wo (Output Gate - Input to Hidden)");

        // Sauvegarder toutes les matrices U (hidden → hidden)
        save_matrix(weights.Uf, "Uf (Forget Gate - Hidden to Hidden)");
        save_matrix(weights.Ui, "Ui (Input Gate - Hidden to Hidden)");
        save_matrix(weights.Uc, "Uc (Cell Gate - Hidden to Hidden)");
        save_matrix(weights.Uo, "Uo (Output Gate - Hidden to Hidden)");

        // Sauvegarder tous les biais
        save_vector(weights.bf, "bf (Forget Gate Bias)");
        save_vector(weights.bi, "bi (Input Gate Bias)");
        save_vector(weights.bc, "bc (Cell Gate Bias)");
        save_vector(weights.bo, "bo (Output Gate Bias)");

        file.close();
        std::cout << "Cerveau sauvegardé avec succès dans " << filename << std::endl;
    }


    static LSTM loadBrain(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Erreur : impossible d'ouvrir " << filename << " pour charger le cerveau" << std::endl;
            return LSTM(11, 8); // Retourner un cerveau par défaut en cas d'erreur
        }

        std::string line;
        
        // Ignorer les commentaires et lire les dimensions
        while (std::getline(file, line)) {
            if (line[0] != '#') break;
        }
        
        int input_sz, hidden_sz;
        std::istringstream iss(line);
        iss >> input_sz >> hidden_sz;

        LSTM brain(input_sz, hidden_sz);
        LSTM_weight loaded_weights;

        // Helper pour charger une matrice
        auto load_matrix = [&](std::vector<std::vector<double>>& matrix, int rows, int cols) {
            // Ignorer le commentaire
            std::getline(file, line);
            
            matrix.resize(rows, std::vector<double>(cols));
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    file >> matrix[i][j];
                }
            }
            std::getline(file, line); // Consommer la fin de ligne
        };

        // Helper pour charger un vecteur
        auto load_vector = [&](std::vector<double>& vec, int size) {
            // Ignorer le commentaire
            std::getline(file, line);
            
            vec.resize(size);
            for (int i = 0; i < size; ++i) {
                file >> vec[i];
            }
            std::getline(file, line); // Consommer la fin de ligne
        };

        // Charger toutes les matrices et vecteurs
        load_matrix(loaded_weights.Wf, hidden_sz, input_sz);
        load_matrix(loaded_weights.Wi, hidden_sz, input_sz);
        load_matrix(loaded_weights.Wc, hidden_sz, input_sz);
        load_matrix(loaded_weights.Wo, hidden_sz, input_sz);

        load_matrix(loaded_weights.Uf, hidden_sz, hidden_sz);
        load_matrix(loaded_weights.Ui, hidden_sz, hidden_sz);
        load_matrix(loaded_weights.Uc, hidden_sz, hidden_sz);
        load_matrix(loaded_weights.Uo, hidden_sz, hidden_sz);

        load_vector(loaded_weights.bf, hidden_sz);
        load_vector(loaded_weights.bi, hidden_sz);
        load_vector(loaded_weights.bc, hidden_sz);
        load_vector(loaded_weights.bo, hidden_sz);

        brain.setWeights(loaded_weights);
        file.close();

        std::cout << "Cerveau chargé avec succès depuis " << filename << std::endl;
        return brain;
    }
};