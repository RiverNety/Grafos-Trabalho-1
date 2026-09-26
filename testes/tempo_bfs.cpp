#include "grafos.cpp"
#include <chrono>
#include <iostream>

int main(){

    Grafo grafo(ModoDeRepresentacao::Matriz);
        grafo.ler_grafo("grafos analisados/grafo_2.txt");
        std::cout <<  "COMECOU" << "\n";

        auto inicio = std::chrono::high_resolution_clock::now();
        grafo.bfs(1);
        auto fim = std::chrono::high_resolution_clock::now();

        auto duracao = std::chrono::duration<double, std::nano>(fim - inicio);
        std::cout << "Tempo da BFS: " << duracao.count() << " ns\n";
        std::cout <<  "ACABOU" << "\n";
}