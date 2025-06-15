#include <vector>
#include <cmath>
#include <iostream>
#include <random>

using namespace std;

class LSTM {
private:
    // Paramètres
    int input_size;
    int hidden_size;
    
    // Poids
    vector<vector<double>> Wf, Wi, Wc, Wo;
    vector<vector<double>> Uf, Ui, Uc, Uo;
    vector<double> bf, bi, bc, bo;
    
    // États cachés
    vector<double> h_prev;
    vector<double> c_prev;

    // Fonctions d'activation
    constexpr inline double sigmoid(double x) {
        return 1.0 / (1.0 + exp(-x));
    }

    constexpr inline double tanh(double x) {
        return ::tanh(x);
    }

public:
    LSTM(int input_size, int hidden_size) 
        : input_size(input_size), hidden_size(hidden_size) {
            h_prev = vector<double>(hidden_size, 0.0);
            c_prev = vector<double>(hidden_size, 0.0);
            initialize_weights();
        }

    constexpr inline void initialize_weights() {
        default_random_engine generator;
        normal_distribution<double> distribution(-1, 1);

        auto init_matrix = [&](int rows, int cols) {
            vector<vector<double>> matrix(rows, vector<double>(cols));
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    matrix[i][j] = distribution(generator);
                }
            }
            return matrix;
        };

        constexpr auto init_vector = [&](int size) {
            vector<double> vec(size);
            for (int i = 0; i < size; ++i) {
                vec[i] = distribution(generator);
            }
            return vec;
        };

        // Initialisation des poids d'entrée (hidden_size x input_size)
        Wf = init_matrix(hidden_size, input_size);
        Wi = init_matrix(hidden_size, input_size);
        Wc = init_matrix(hidden_size, input_size);
        Wo = init_matrix(hidden_size, input_size);

        // Initialisation des poids récurrents (hidden_size x hidden_size)
        Uf = init_matrix(hidden_size, hidden_size);
        Ui = init_matrix(hidden_size, hidden_size);
        Uc = init_matrix(hidden_size, hidden_size);
        Uo = init_matrix(hidden_size, hidden_size);

        // Initialisation des biais
        bf = init_vector(hidden_size);
        bi = init_vector(hidden_size);
        bc = init_vector(hidden_size);
        bo = init_vector(hidden_size);
    }

    vector<double> forward(const vector<double>& x) {
        vector<double> f_gate(hidden_size);
        vector<double> h_gate(hidden_size);
        vector<double> o_gate(hidden_size);
        vector<double> i_gate(hidden_size);
        vector<double> c_tilde(hidden_size);
        vector<double> h_next(hidden_size);
        vector<double> c_next(hidden_size);

        // Forget gate: f_t = σ(Wf * x + Uf * h_prev + bf)
        for (int i = 0; i < hidden_size; ++i) {
            double sum = 0.0;
            for (int j = 0; j < input_size; ++j) sum += Wf[i][j] * x[j];
            for (int j = 0; j < hidden_size; ++j) sum += Uf[i][j] * h_prev[j];
            f_gate[i] = sigmoid(sum + bf[i]);
        }

        // Input gate: i_t = σ(Wi * x + Ui * h_prev + bi)
        for (int i = 0; i < hidden_size; ++i) {
            double sum = 0.0;
            for (int j = 0; j < input_size; ++j) sum += Wi[i][j] * x[j];
            for (int j = 0; j < hidden_size; ++j) sum += Ui[i][j] * h_prev[j];
            i_gate[i] = sigmoid(sum + bi[i]);
        }

        // Candidate cell state: c_tilde = tanh(Wc * x + Uc * h_prev + bc)
        for (int i = 0; i < hidden_size; ++i) {
            double sum = 0.0;
            for (int j = 0; j < input_size; ++j) sum += Wc[i][j] * x[j];
            for (int j = 0; j < hidden_size; ++j) sum += Uc[i][j] * h_prev[j];
            c_tilde[i] = tanh(sum + bc[i]);
        }

        // Cell state update: c_next = f_t ⊙ c_prev + i_t ⊙ c_tilde
        for (int i = 0; i < hidden_size; ++i) {
            c_next[i] = f_gate[i] * c_prev[i] + i_gate[i] * c_tilde[i];
        }

        // Output gate: o_t = σ(Wo * x + Uo * h_prev + bo)
        for (int i = 0; i < hidden_size; ++i) {
            double sum = 0.0;
            for (int j = 0; j < input_size; ++j) sum += Wo[i][j] * x[j];
            for (int j = 0; j < hidden_size; ++j) sum += Uo[i][j] * h_prev[j];
            o_gate[i] = sigmoid(sum + bo[i]);
        }

        // Hidden state: h_next = o_t ⊙ tanh(c_next)
        for (int i = 0; i < hidden_size; ++i) {
            h_next[i] = o_gate[i] * tanh(c_next[i]);
        }

        // Mettez à jour les états précédents
        c_prev = c_next;
        h_prev = h_next;

        return h_next;
    }

    constexpr void mutate(double mutateRate) {
        auto mutate_vector = [&](vector<double>& vec) {
            for (double& value : vec) {
                if (static_cast<double>(rand()) / RAND_MAX < mutateRate) {
                    value += static_cast<double>(rand()) / RAND_MAX * 0.1 - 0.05;
                }
            }
        };

        auto mutate_matrix = [&](vector<vector<double>>& matrix) {
            for (auto& row : matrix) {
                mutate_vector(row);
            }
        };

        mutate_matrix(Wf);
        mutate_matrix(Wi);
        mutate_matrix(Wc);
        mutate_matrix(Wo);
        mutate_matrix(Uf);
        mutate_matrix(Ui);
        mutate_matrix(Uc);
        mutate_matrix(Uo);
        
        mutate_vector(bf);
        mutate_vector(bi);
        mutate_vector(bc);
        mutate_vector(bo);
    }

    constexpr LSTM breed(const LSTM& parent) {
        LSTM child(input_size, hidden_size);
        
        auto breed_vector = [&](vector<double>& child_vector, const vector<double>& this_vector, const vector<double>& parent_vector) {
            for (int i = 0; i < child_vector.size(); ++i) {
                child_vector[i] = (this_vector[i] + parent_vector[i]) / 2;
            }
        };
        
        auto breed_matrix = [&](vector<vector<double>>& child_matrix, const vector<vector<double>>& this_matrix, const vector<vector<double>>& parent_matrix) {
            for (int i = 0; i < child_matrix.size(); ++i) {
                breed_vector(child_matrix[i],this_matrix[i],parent_matrix[i]);
            }
        };

        breed_matrix(child.Wf, this->Wf, parent.Wf);
        breed_matrix(child.Wi, this->Wi, parent.Wi);
        breed_matrix(child.Wc, this->Wc, parent.Wc);
        breed_matrix(child.Wo, this->Wo, parent.Wo);
        breed_matrix(child.Uf, this->Uf, parent.Uf);
        breed_matrix(child.Ui, this->Ui, parent.Ui);
        breed_matrix(child.Uc, this->Uc, parent.Uc);
        breed_matrix(child.Uo, this->Uo, parent.Uo);

        breed_vector(child.bf, this->bf, parent.bf);
        breed_vector(child.bi, this->bi, parent.bi);
        breed_vector(child.bc, this->bc, parent.bc);
        breed_vector(child.bo, this->bo, parent.bo);

        return child;
    }

// Inutile là pour l'évolution
/*
    void backward(const vector<double>& x, const vector<double>& h_next_grad) {
        // Calculer les gradients pour :
        // - Paramètres (W, U, b)
        // - États précédents (h_prev, c_prev)
        // - Entrée x

        
    }

    void update_parameters(double learning_rate) {
        // Appliquer la descente de gradient :
        // W = W - learning_rate * dW
        // (Idem pour U et b)
        Wc = Wc - learning_rate * dW;
        Wi = Wi - learning_rate * dW;
        Wf = Wf - learning_rate * dw; 
        Wo = Wo - learning_rate * dW;

        Uc = Uc - learning_rate * dW;
        Uf= Uf - learning_rate * dW;
        Ui = Ui - learning_rate * dW;
        Uo = Uo - learning_rate * dW;

        bf = bf - learning_rate * dW;
        bi = bi - learning_rate * dW;
        bc = bc - learning_rate * dW;
        bo = bo - learning_rate * dW;

    }
*/
    
}
