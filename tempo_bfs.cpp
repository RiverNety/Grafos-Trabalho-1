#include "grafos.cpp"
#include <iostream>

int main(){

    Grafo grafo(ModoDeRepresentacao::Lista);
        grafo.ler_grafo("grafos analisados/grafo_5.txt");
        std::cout <<  "COMECOU" << "\n";
        grafo.bfs(1);
        std::cout <<  "ACABOU" << "\n";
}