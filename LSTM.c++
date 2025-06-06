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
    double sigmoid(double x) {
        return 1.0 / (1.0 + exp(-x));
    }

    double tanh(double x) {
        return ::tanh(x);
    }

public:
    LSTM(int input_size, int hidden_size) 

        : input_size(input_size), hidden_size(hidden_size) {

            h_prev =vector<double>(hidden_size,0,0);
            c_prev = vector<double>(input_size,0,0);

            initilize_weights();

        }

    void initialize_weights() {
        default_random_engine generator;
        normal_distribution<double> distribution(0.0, 0.1);  // Petites valeurs initiales

        auto init_matrix = [&](int rows, int cols) {
            vector<vector<double>> matrix(rows, vector<double>(cols));
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    matrix[i][j] = distribution(generator);
                }
            }
            return matrix;
        };

        auto init_vector = [&](int size) {
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


        // Cell state update: c_next = f_t ⊙ c_prev + i_t ⊙ c_tilde
        for(int i =0; i <hidden_size; i++) {
            c_next[i] = f_gate[i] * c_prev[i] + i_gate[i] * c_tilde[i];
        }

        c_prev = c_next;
        h_prev = h_next;

        return h_next;

    }

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
    }

    
}
