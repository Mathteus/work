#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <string>
#include <exception>
#include <thread>
#include <mutex>
#include <vector>
#include "sqlite3/sqlite3.h"
using namespace std;

void conection_database(string arr, string arr2, bool achado){
    sqlite3* DB;
    int resposta = {0};
    char* mensagem_erro;
    string comando{""};
    int possui = (achado) ? 1 : 0;
    mutex mute;
    comando = "insert into dinamica (matriz, vetor, possui) values ('" + arr + "' , '" + arr2 + "', '" + to_string(possui) + "');";

    lock_guard<mutex> lguard(mute);
    cout << "id this thead: " << this_thread::get_id() << "\n";
    resposta = sqlite3_open("BancoDados.db" , &DB);
    resposta = sqlite3_exec(DB, comando.c_str(), NULL, 0, &mensagem_erro);

    if (resposta != SQLITE_OK) {
        cerr << "erro: [" << sqlite3_errmsg(DB) <<  "]...\n" << endl;
        sqlite3_free(mensagem_erro);
    }

    sqlite3_close(DB);
}

void execucao(const unsigned int linha, const unsigned int coluna, unsigned int numero_maximo){
    unsigned int matriz[linha][coluna];
    unsigned int vetor_verificador[coluna];
    bool numeros_iguais{false};
    unsigned int total_numero_encotrados{0};
    string matriz_string{""}, vetor_string{""};
    mutex mute;
    lock_guard<mutex> lguard(mute);

    srand(time(NULL));
    for(size_t a=0; a<linha; a++){
        vetor_verificador[a] = (rand()%+numero_maximo) + 1;
        for(size_t b=0; b<coluna; b++){
            matriz[a][b] = (rand()%+numero_maximo) + 1;
        }
    }

    for(size_t a=0; a<linha; a++){
        for(size_t b=0; b<coluna; b++){
            if(matriz[a][b] == vetor_verificador[b])total_numero_encotrados++;
        }
    }
    if(total_numero_encotrados == coluna)numeros_iguais = true;

    for(size_t a=0; a<linha; a++){
        matriz_string += "[";
        for(size_t b=0; b<coluna; b++){
            matriz_string += to_string(matriz[a][b]);
            if(b < (coluna - 1))
                matriz_string += ",";
        }
        matriz_string += "];";
    }

    vetor_string += "[";
    for(size_t a=0; a<coluna; a++){
        vetor_string += to_string(vetor_verificador[a]);
        if(a < (coluna - 1))
            vetor_string += ",";
    }   
    vetor_string += "];";
    conection_database(matriz_string, vetor_string, numeros_iguais);
}

void unique_thread(){
    unsigned int l{2}, c{2}, numero_maximo{1}, vezes{1}, sucesso{0};
    printf("quantas matrizes dejesa colocar no danco de dados: ");
    cin >> vezes;

    printf("Digite o numero de linhas e colunas separados por um espaço: ");
    cin >> l >> c;
    if(l < 2 && c < 2){ l = 2; c = 2; }

    printf("digite um valor maximo para matriz receber [1/255]: ");
    cin >> numero_maximo;
    if(numero_maximo < 1)numero_maximo = 1;
    if(numero_maximo > 255)numero_maximo = 255;

    for(int a=0; a<vezes; a++){
        execucao(l, c, numero_maximo);
    }

    if(sucesso == vezes)
        cout << "\nTodas as operacoes foram bem sucedidas...\n";
}

void multi_threads(){
    unsigned int numero_threads_hardware = thread::hardware_concurrency();
    unsigned int numero_threads_uso = numero_threads_hardware - 1;
    vector<thread> threads(numero_threads_uso);

    unsigned int l{2}, c{2}, numero_maximo{1}, vezes{1}, sucesso{0};
    printf("quantas matrizes dejesa colocar no danco de dados: ");
    cin >> vezes;

    printf("Digite o numero de linhas e colunas separados por um espaço: ");
    cin >> l >> c;
    if(l < 2 && c < 2){ l = 2; c = 2; }

    unsigned int linha_thread1 = l / 2;
    unsigned int coluna_thread1 = c / 2;
    unsigned int linha_thread2 = l - linha_thread1;
    unsigned int coluna_thread2 = c - coluna_thread1;

    printf("digite um valor maximo para matriz receber [1/255]: ");
    cin >> numero_maximo;
    if(numero_maximo < 1)numero_maximo = 1;
    if(numero_maximo > 255)numero_maximo = 255;

    threads[0] = thread(execucao, linha_thread1, coluna_thread1, numero_maximo);
    threads[1] = thread(execucao, linha_thread2, coluna_thread2, numero_maximo);

    for(size_t a=0; a<numero_threads_uso; a++){
        threads[a].join();
    }
}
