#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <chrono>
#include <atomic>
#include <mutex>
#include "MpScQueue.h"
#include "sthread.hpp"
#include "Bancodados.hpp"
using namespace std;
using namespace chrono;
typedef unsigned int unit;

inline int randomico(int maximo, int minimo){
    return (rand() % maximo + minimo);
}

void execucao(const unsigned int linha, const unsigned int coluna, unsigned int numero_maximo, shared_ptr<BancoDados> bancodados){
    unit matriz[linha][coluna];
    unit vetor_verificador[coluna];
    bool numeros_iguais{false}, pular{false};
    unit total_numero_encotrados{0};
    string matriz_string{""}, vetor_string{""};

    for (size_t a = 0; a < linha; a++){
        for (size_t b = 0; b < coluna; b++){
            matriz[a][b] = randomico(numero_maximo, 1);
        }
    }

    for (size_t a = 0; a < linha; a++){
        vetor_verificador[a] = randomico(numero_maximo, 1);
    }

    for (size_t d = 0; d < coluna; d++){
        pular = false;
        for (size_t a = 0; a < linha; a++){
            for (size_t b = 0; b < coluna; b++){
                if (matriz[a][b] == vetor_verificador[d]){
                    total_numero_encotrados++;
                    pular = true;
                    break;
                }
                if (pular)
                    break;
            }
        }
    }
    if (total_numero_encotrados == coluna)
        numeros_iguais = true;

    for (size_t a = 0; a < linha; a++){
        matriz_string += "[";
        for (size_t b = 0; b < coluna; b++){
            matriz_string += to_string(matriz[a][b]);
            if (b < (coluna - 1))
                matriz_string += ",";
        }
        matriz_string += "];";
    }

    vetor_string += "[";
    for (size_t a = 0; a < coluna; a++){
        vetor_string += to_string(vetor_verificador[a]);
        if (a < (coluna - 1))
            vetor_string += ",";
    }
    vetor_string += "];";

    bancodados->inserir(matriz_string, vetor_string, numeros_iguais);
}

void unique_thread(){
    shared_ptr<BancoDados> bancodados(new BancoDados("BancoDados.db"));
    unit linha{2}, coluna{2}, numero_maximo{1}, vezes{1};

    printf("quantas matrizes dejesa colocar no danco de dados: ");
    cin >> vezes;

    printf("Digite o numero de linhas e colunas separados por um espaço: ");
    cin >> linha >> coluna;
    if (linha < 2) linha = 2;
    if (coluna < 2) coluna = 2;

    printf("digite um valor maximo para matriz receber [1/255]: ");
    cin >> numero_maximo;
    if (numero_maximo < 1) numero_maximo = 1;
    if (numero_maximo > 255) numero_maximo = 255;

    steady_clock::time_point time1 = steady_clock::now();
    for (int a = 0; a < vezes; a++){
        execucao(linha, coluna, numero_maximo, bancodados);
    }
    steady_clock::time_point time2 = steady_clock::now();
    duration<double> tempo = duration_cast<duration<double>>(time2 - time1);
    cout << "resultado em unico thread:\n";
    cout << "programa demorou " << tempo.count() << " segundos para alocar as matrizes no banco de dados..\n";
}

void multi_threads(){
    shared_ptr<BancoDados> bancodados(new BancoDados("BancoDados.db"));
    unit numero_threads_hardware = thread::hardware_concurrency();
    unit numero_threads_uso = numero_threads_hardware - 1;
    vector<thread> threads(numero_threads_uso);
    unit linha{2}, coluna{2}, numero_maximo{1}, vezes{1};

    printf("quantas matrizes dejesa colocar no danco de dados: ");
    cin >> vezes;

    printf("Digite o numero de linhas e colunas separados por um espaço: ");
    cin >> linha >> coluna;
    if (linha < 2) linha = 2;
    if (coluna < 2) coluna = 2;

    printf("digite um valor maximo para matriz receber [1/255]: ");
    cin >> numero_maximo;
    if (numero_maximo < 1) numero_maximo = 1;
    if (numero_maximo > 255) numero_maximo = 255;

    vezes /= numero_threads_uso;

    if ((vezes % numero_threads_uso) != 0)
        vezes++;

    steady_clock::time_point time1 = steady_clock::now();
    for (int t=0; t<vezes; t++){
        for (size_t a=0; a<numero_threads_uso; a++){
            unique_ptr<SThread> threads(new SThread(thread(execucao, linha, coluna, numero_maximo, bancodados)));
        }
    }
    steady_clock::time_point time2 = steady_clock::now();
    duration<double> tempo = duration_cast<duration<double>>(time2 - time1);
    cout << "resultado em multi thread:\n";
    cout << "programa demorou " << tempo.count() << " segundos para alocar as matrizes no banco de dados..\n";
}

/*
void mpsc_queue(){
    shared_ptr<BancoDados> bancodados(new BancoDados("BancoDados.db"));
    unsigned int numero_threads_hardware = thread::hardware_concurrency();
    unsigned int numero_threads_uso = numero_threads_hardware - 1;
    MpScQueue<thread> threads;
    unsigned int linha{2}, coluna{2}, numero_maximo{1}, vezes{1};
    
    printf("quantas matrizes dejesa colocar no danco de dados: ");
    cin >> vezes;

    printf("Digite o numero de linhas e colunas separados por um espaço: ");
    cin >> linha >> coluna;
    if(linha < 2)linha = 2; 
    if(coluna < 2)coluna = 2;

    printf("digite um valor maximo para matriz receber [1/255]: ");
    cin >> numero_maximo;
    if(numero_maximo < 1)numero_maximo = 1;
    if(numero_maximo > 255)numero_maximo = 255;

    unsigned int tarefas_threads =  vezes / numero_threads_uso;

    if((tarefas_threads%numero_threads_uso) != 0)
        tarefas_threads++;

    steady_clock::time_point time1 = steady_clock::now();
    for(int t=0; t<tarefas_threads; t++){
        for(size_t a=0; a<numero_threads_uso; a++){
        }
    }
    steady_clock::time_point time2 = steady_clock::now();
    duration<double> tempo = duration_cast<duration<double>>(time2 - time1);
    cout << "resultado com mpsc queue:\n";
    cout << "programa demorou " << tempo.count() << " segundos para alocar " << bancodados->acessados.load() << " matrizes no banco de dados..\n";
}
*/
