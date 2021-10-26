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

class BancoDados {
    private:
        sqlite3* DB;
        bool resposta;
        char* mensagem_erro;
        string erro, comando;

        static int callback(void *data, int argc, char **argv, char **azColName){
            int i;
            //fprintf(stderr, "%s: ", (const char*)data);
            
            for(i = 0; i<argc; i++){
                printf("%s \t %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
            }
            
            printf("\n");
            return 0;
        }

    public:
        BancoDados(const char* name) : resposta{false}, mensagem_erro{""}, erro{""}, comando{""} {
            resposta = sqlite3_open(name, &DB);
            if (resposta != SQLITE_OK){
                cerr << "erro: [" << sqlite3_errmsg(DB) <<  "]\n" << endl; 
                sqlite3_free(mensagem_erro);
                exit(-1);
            }
        }

        ~BancoDados(){
            sqlite3_close(DB);
        }

        void inserir(string arr, string arr2, bool possui){
            cout << "this thread: " << this_thread::get_id() << endl;
            comando = "insert into matriz_dinamica (matriz, vetor, possui) values ('" + arr + "' , '" + arr2 + "', '" + to_string(possui) + "');";
            resposta = sqlite3_exec(DB, comando.c_str(), NULL, 0, &mensagem_erro);
            erro = sqlite3_errmsg(DB);

            if(erro == "no such table: matriz_dinamica"){
                comando = "create table matriz_dinamica (id integer not null primary key autoincrement, matriz text not null, vetor text not null, possui bool not null);";
                resposta = sqlite3_exec(DB, comando.c_str(), NULL, 0, &mensagem_erro);
                comando = "insert into matriz_dinamica (matriz, vetor, possui) values ('" + arr + "' , '" + arr2 + "', '" + to_string(possui) + "');";
                resposta = sqlite3_exec(DB, comando.c_str(), NULL, 0, &mensagem_erro);
            }

            if (resposta != SQLITE_OK) {
                cerr << "erro: [" << sqlite3_errmsg(DB) <<  "]\n" << endl;
                sqlite3_free(mensagem_erro);
                sqlite3_close(DB);
                exit(-1);
            }
        }

};

int randomico(int maximo, int minimo){
    return (rand()%maximo + minimo);
}

void conection_database(string arr, string arr2, bool achado, sqlite3* DB){ }

void execucao(const unsigned int linha, const unsigned int coluna, unsigned int numero_maximo, sqlite3* DB){
    unsigned int matriz[linha][coluna];
    unsigned int vetor_verificador[coluna];
    bool numeros_iguais{false}, pular{false};
    unsigned int total_numero_encotrados{0};
    string matriz_string{""}, vetor_string{""};

    for(size_t a=0; a<linha; a++){
        for(size_t b=0; b<coluna; b++){
            matriz[a][b] = randomico(numero_maximo, 1);
        }
    }

    for(size_t a=0; a<linha; a++){
        vetor_verificador[a] = randomico(numero_maximo, 1);
    }

    for(size_t d=0; d<coluna; d++){
        pular = false;
        for(size_t a=0; a<linha; a++){
            for(size_t b=0; b<coluna; b++){
                if(matriz[a][b] == vetor_verificador[d]){
                    total_numero_encotrados++;
                    pular = true;
                    break;
                }
                if(pular)break;
            }
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

    mutex mute;
    lock_guard<mutex> lguard(mute);
    conection_database(matriz_string, vetor_string, numeros_iguais, DB);
}

void unique_thread(){

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

    for(int a=0; a<vezes; a++){
        execucao(linha, coluna, numero_maximo, DB);
    }
}

void multi_threads(){
    sqlite3* DB; char* mensagem_erro;
    int resposta = sqlite3_open("BancoDados.db", &DB);
    if (resposta != SQLITE_OK){
        cerr << "erro: [" << sqlite3_errmsg(DB) <<  "]\n" << endl; 
        sqlite3_free(mensagem_erro);
        exit(-1);
    }

    unsigned int numero_threads_hardware = thread::hardware_concurrency();
    unsigned int numero_threads_uso = numero_threads_hardware - 1;
    vector<thread> threads(numero_threads_uso);

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
    tarefas_threads++;

    for(int t=0; t<tarefas_threads; t++){
        for(size_t a=0; a<numero_threads_uso; a++){
            threads[a] = thread(execucao, linha, coluna, numero_maximo, DB);
        }

        for(size_t a=0; a<numero_threads_uso; a++){
            threads[a].join();
        }
    }
}
