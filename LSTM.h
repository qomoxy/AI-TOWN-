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
    // Le générateur est passé depuis l'extérieur pour éviter
    // les seeds non-déterministes et l'exposition d'un membre public.
    // Pour la mutation et le breed on utilise un rng passé en paramètre.
    // On conserve un générateur interne uniquement pour l'initialisation.
    std::default_random_engine generator;

    LSTM(int input_size, int hidden_size)
        : input_size(input_size), hidden_size(hidden_size) {
        h_prev = std::vector<double>(hidden_size, 0.0);
        c_prev = std::vector<double>(hidden_size, 0.0);
        initialize_weights();
    }

    void initialize_weights() {
        std::normal_distribution<double> distribution(0, sqrt(1.0 / input_size));

        auto init_matrix = [&](int rows, int cols) {
            std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols));
            for (int i = 0; i < rows; ++i)
                for (int j = 0; j < cols; ++j)
                    matrix[i][j] = distribution(generator);
            return matrix;
        };

        auto init_vector = [&](int size) {
            std::vector<double> vec(size);
            for (int i = 0; i < size; ++i)
                vec[i] = distribution(generator);
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

    // --- Getters ---
    LSTM_weight getWeights() const { return this->weights; }
    int getInputSize()  const { return input_size; }
    int getHiddenSize() const { return hidden_size; }
    std::vector<double> getC_prev() const { return c_prev; }
    std::vector<double> getH_prev() const { return h_prev; }

    void setWeights(const LSTM_weight& new_weights) {
        this->weights = new_weights;
    }

    // --- Forward pass ---
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
            for (int j = 0; j < input_size;  ++j) sum += weights.Wf[i][j] * x[j];
            for (int j = 0; j < hidden_size; ++j) sum += weights.Uf[i][j] * h_prev[j];
            f_gate[i] = sigmoid(sum + weights.bf[i]);
        }

        // Input gate
        for (int i = 0; i < hidden_size; ++i) {
            double sum = 0.0;
            for (int j = 0; j < input_size;  ++j) sum += weights.Wi[i][j] * x[j];
            for (int j = 0; j < hidden_size; ++j) sum += weights.Ui[i][j] * h_prev[j];
            i_gate[i] = sigmoid(sum + weights.bi[i]);
        }

        // Candidate cell state
        for (int i = 0; i < hidden_size; ++i) {
            double sum = 0.0;
            for (int j = 0; j < input_size;  ++j) sum += weights.Wc[i][j] * x[j];
            for (int j = 0; j < hidden_size; ++j) sum += weights.Uc[i][j] * h_prev[j];
            c_tilde[i] = tanh_act(sum + weights.bc[i]);
        }

        // Cell state update
        for (int i = 0; i < hidden_size; ++i)
            c_next[i] = f_gate[i] * c_prev[i] + i_gate[i] * c_tilde[i];

        // Output gate
        for (int i = 0; i < hidden_size; ++i) {
            double sum = 0.0;
            for (int j = 0; j < input_size;  ++j) sum += weights.Wo[i][j] * x[j];
            for (int j = 0; j < hidden_size; ++j) sum += weights.Uo[i][j] * h_prev[j];
            o_gate[i] = sigmoid(sum + weights.bo[i]);
        }

        // Hidden state
        for (int i = 0; i < hidden_size; ++i)
            h_next[i] = o_gate[i] * tanh_act(c_next[i]);

        c_prev = c_next;
        h_prev = h_next;
        return h_next;
    }

    // --- Mutation ---
    // CORRECTIF : l'amplitude de mutation augmente avec le taux (mutateRate)
    // pour vraiment "secouer" les poids lors des périodes de stagnation,
    // et non juste augmenter la fréquence des petites perturbations.
    void mutate(double mutateRate, std::mt19937& rng) {
        std::uniform_real_distribution<double> prob_dist(0.0, 1.0);

        // L'amplitude est proportionnelle au taux : 0.05 en normal, ~0.15 en haute mutation
        double amplitude = 0.05 + 0.22 * mutateRate;
        std::uniform_real_distribution<double> mutation_dist(-amplitude, amplitude);

        auto mutate_vector = [&](std::vector<double>& vec) {
            for (double& value : vec)
                if (prob_dist(rng) < mutateRate)
                    value += mutation_dist(rng);
        };

        auto mutate_matrix = [&](std::vector<std::vector<double>>& matrix) {
            for (auto& row : matrix)
                mutate_vector(row);
        };

        mutate_matrix(weights.Wf); mutate_matrix(weights.Wi);
        mutate_matrix(weights.Wc); mutate_matrix(weights.Wo);
        mutate_matrix(weights.Uf); mutate_matrix(weights.Ui);
        mutate_matrix(weights.Uc); mutate_matrix(weights.Uo);
        mutate_vector(weights.bf); mutate_vector(weights.bi);
        mutate_vector(weights.bc); mutate_vector(weights.bo);
    }

    // --- Crossover ---
    // CORRECTIF : crossover uniforme (chaque poids est pris chez l'un ou l'autre parent
    // avec proba 50/50) au lieu de la moyenne arithmétique qui convergeait trop vite.
    LSTM breed(const LSTM& parent, std::mt19937& rng) const {
        LSTM child(this->input_size, this->hidden_size);
        LSTM_weight child_weights;
        const LSTM_weight& pw = parent.getWeights();

        std::uniform_int_distribution<int> coin(0, 1);

        auto breed_vector = [&](const std::vector<double>& v1, const std::vector<double>& v2) {
            std::vector<double> result(v1.size());
            for (size_t i = 0; i < v1.size(); ++i)
                result[i] = coin(rng) ? v1[i] : v2[i];
            return result;
        };

        auto breed_matrix = [&](const std::vector<std::vector<double>>& m1,
                                const std::vector<std::vector<double>>& m2) {
            std::vector<std::vector<double>> result(m1.size());
            for (size_t i = 0; i < m1.size(); ++i)
                result[i] = breed_vector(m1[i], m2[i]);
            return result;
        };

        child_weights.Wf = breed_matrix(this->weights.Wf, pw.Wf);
        child_weights.Wi = breed_matrix(this->weights.Wi, pw.Wi);
        child_weights.Wc = breed_matrix(this->weights.Wc, pw.Wc);
        child_weights.Wo = breed_matrix(this->weights.Wo, pw.Wo);
        child_weights.Uf = breed_matrix(this->weights.Uf, pw.Uf);
        child_weights.Ui = breed_matrix(this->weights.Ui, pw.Ui);
        child_weights.Uc = breed_matrix(this->weights.Uc, pw.Uc);
        child_weights.Uo = breed_matrix(this->weights.Uo, pw.Uo);
        child_weights.bf = breed_vector(this->weights.bf, pw.bf);
        child_weights.bi = breed_vector(this->weights.bi, pw.bi);
        child_weights.bc = breed_vector(this->weights.bc, pw.bc);
        child_weights.bo = breed_vector(this->weights.bo, pw.bo);

        child.setWeights(child_weights);
        return child;
    }

    // --- Sauvegarde / Chargement ---
    void saveBrain(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Erreur : impossible d'ouvrir " << filename << " pour sauvegarder le cerveau\n";
            return;
        }

        file << "# LSTM Brain Save File\n";
        file << "# Input Size: " << input_size << ", Hidden Size: " << hidden_size << "\n";
        file << input_size << " " << hidden_size << "\n";

        auto save_matrix = [&](const std::vector<std::vector<double>>& matrix, const std::string& name) {
            file << "# " << name << "\n";
            for (const auto& row : matrix) {
                for (const auto& val : row) file << val << " ";
                file << "\n";
            }
        };

        auto save_vector = [&](const std::vector<double>& vec, const std::string& name) {
            file << "# " << name << "\n";
            for (const auto& val : vec) file << val << " ";
            file << "\n";
        };

        save_matrix(weights.Wf, "Wf"); save_matrix(weights.Wi, "Wi");
        save_matrix(weights.Wc, "Wc"); save_matrix(weights.Wo, "Wo");
        save_matrix(weights.Uf, "Uf"); save_matrix(weights.Ui, "Ui");
        save_matrix(weights.Uc, "Uc"); save_matrix(weights.Uo, "Uo");
        save_vector(weights.bf, "bf"); save_vector(weights.bi, "bi");
        save_vector(weights.bc, "bc"); save_vector(weights.bo, "bo");

        file.close();
        std::cout << "Cerveau sauvegardé dans " << filename << "\n";
    }

    // CORRECTIF : utilise input_sz / hidden_sz lus depuis le fichier,
    // et non des magic numbers (11, 8) en cas d'erreur.
    static LSTM loadBrain(const std::string& filename, int default_input, int default_hidden) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Erreur : impossible d'ouvrir " << filename << "\n";
            return LSTM(default_input, default_hidden);
        }

        std::string line;
        while (std::getline(file, line))
            if (!line.empty() && line[0] != '#') break;

        int input_sz, hidden_sz;
        std::istringstream iss(line);
        iss >> input_sz >> hidden_sz;

        LSTM brain(input_sz, hidden_sz);
        LSTM_weight loaded_weights;

        auto load_matrix = [&](std::vector<std::vector<double>>& matrix, int rows, int cols) {
            std::getline(file, line); // skip comment
            matrix.resize(rows, std::vector<double>(cols));
            for (int i = 0; i < rows; ++i)
                for (int j = 0; j < cols; ++j)
                    file >> matrix[i][j];
            std::getline(file, line);
        };

        auto load_vector = [&](std::vector<double>& vec, int size) {
            std::getline(file, line); // skip comment
            vec.resize(size);
            for (int i = 0; i < size; ++i)
                file >> vec[i];
            std::getline(file, line);
        };

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
        std::cout << "Cerveau chargé depuis " << filename << "\n";
        return brain;
    }
};
