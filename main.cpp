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
// unique_thread 5000 matrizes 100x100 numero max 255 1#59.71     2#64.99     3#65.36     4#59.43       5#59.17     media 61:73 segundos para rodar
// multi thread 5000 matrizes 100x100 numero max 255 1#56.78     2#63.54     3#63.22     4#63.32       5#65.33     media 62:43 segundos para rodar