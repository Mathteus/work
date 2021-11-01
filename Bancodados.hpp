#include <iostream>
#include <cstdlib>
#include <mutex>
#include <atomic>
#include <memory>
#include "sqlite3/sqlite3.h"
using namespace std;
typedef unsigned int unit;

class BancoDados{
private:
    static int callback(void *data, int argc, char **argv, char **azColName){
        //fprintf(stderr, "%s: ", (const char*)data);
        for (int i = 0; i < argc; i++){
            printf("%s \t %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
    }

    atomic<sqlite3*> DB;
    bool resposta;
    string erro, comando; 
    char* mensagem_erro;
    mutex mute;

public:
    atomic<unit> acessados;
    BancoDados(const char* name) : resposta(false), erro(""), comando(""), acessados(0) {
        resposta = sqlite3_open(name, reinterpret_cast<sqlite3**>(&DB));
        if (resposta != SQLITE_OK){
            cerr << "erro: [" << sqlite3_errmsg(DB) << "]\n"
                 << endl;
            sqlite3_free(mensagem_erro);
            exit(-1);
        }
    }

    ~BancoDados(){
        sqlite3_close(DB);
        sqlite3_free(mensagem_erro);
        exit(-1);
    }

    void inserir(string arr, string arr2, bool possui){
        comando = "insert into dinamica (matriz, vetor, possui) values ('" + arr + "' , '" + arr2 + "', '" + to_string(possui) + "');";
        resposta = sqlite3_exec(DB.load(std::memory_order_seq_cst), comando.c_str(), NULL, 0, &mensagem_erro);
        erro = sqlite3_errmsg(DB);

        if (erro == "no such table: dinamica"){
            comando = "create table dinamica (id integer not null primary key autoincrement, matriz text not null, vetor text not null, possui bool not null);";
            resposta = sqlite3_exec(DB.load(std::memory_order_seq_cst), comando.c_str(), NULL, 0, &mensagem_erro);
            comando = "insert into dinamica (matriz, vetor, possui) values ('" + arr + "' , '" + arr2 + "', '" + to_string(possui) + "');";
            resposta = sqlite3_exec(DB.load(std::memory_order_seq_cst), comando.c_str(), NULL, 0, &mensagem_erro);
        }

        if (resposta != SQLITE_OK){
            cerr << "erro: [" << sqlite3_errmsg(DB) << "]\n";
            sqlite3_free(mensagem_erro);
            exit(-1);
        }

        acessados++;
    }

    void select(){
        comando = "select * from dinamica;";
        resposta = sqlite3_exec(DB.load(std::memory_order_seq_cst), comando.c_str(), callback, 0, &mensagem_erro);
        erro = sqlite3_errmsg(DB);

        if (resposta != SQLITE_OK){
            cerr << "erro: [" << sqlite3_errmsg(DB) << "]\n";
            sqlite3_free(mensagem_erro);
            exit(-1);
        }
    }
};