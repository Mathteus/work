#include"matriz_dinamica.hpp"
using namespace std;

int main(int argv, char** argc){
    srand(time(NULL));
    multi_threads();
    return 0;
}

//g++ main.cpp -o main -pthread -lsqlite3 -std=c++17