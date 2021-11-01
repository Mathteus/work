#include "matriz_dinamica.hpp"
using namespace std;

int main(int argv, char** argc){
    int resposta{0};
    cout << "[1]Teste Unique Thread -- [2] Teste Multi Thread -- [3] Teste Usando MPSC Queue: ";
    cin >> resposta;
    srand(time(NULL));
    if(resposta == 1)
        unique_thread();
    else if(resposta == 2)
        multi_threads();
    else if(resposta == 3)
        //mpsc_queue();
        cout << "indisponivel\n";
    else
        cout << "opcao invalida...\n";
    return 0;
}

// g++ main.cpp -o main -pthread -lsqlite3 -fexceptions -march=native -O3  -std=c++2a