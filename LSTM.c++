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
    LSTM(int input_size, int hidden_size) {

        
    }

    void initilize_weight () {}
}
