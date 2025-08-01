#include "Agent.h"

int main() {
    // Création d'agents
    Agent alice("Alice", 1, 3, 1.0, 100.0, 0.8, 5, 3);
    Agent bob("Bob", 2, 5, 0.8, 90.0, 0.7, 5, 3);

    // Alice réfléchit
    std::vector<double> input = {1.0, 0.5, 0.2, 0.8, 0.0};
    auto thoughts = alice.think(input);
    
    // Mutation du cerveau de Bob
    bob.mutateBrain(0.1);

    // Reproduction
    Agent charlie = alice.breedWith(bob, "Charlie", 3);

    return 0;
}

/* last main 
int main()
{
    // Zone de test
    LSTM a(5,3);
    std::vector<double> b =  a.forward({1,5,4,7,4});
    for(int i = 0 ; i < b.size() ; i++)
    {
        std::cout << b[i] << std::endl;
    }
    Weights c = a.getWeights();
    a.setWeights(c);
    b =  a.forward({1,5,4,7,4});
    for(int i = 0 ; i < b.size() ; i++)
    {
        std::cout << b[i] << std::endl;
    }
    
    return 0;
}
    */