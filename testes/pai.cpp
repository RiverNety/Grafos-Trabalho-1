#include "grafos.cpp"
#include <iostream>

int main(){
    Grafo grafo(ModoDeRepresentacao::Lista);
    grafo.ler_grafo("grafos analisados/grafo_1.txt");

    const int vertices_iniciais[] = {1, 2, 3};
    const int vertices_alvo[] = {10, 20, 30};

    for (int inicio : vertices_iniciais) {
        std::cout << "Busca no vertice " << inicio << ":\n";

        grafo.bfs(inicio);
        std::cout << "  BFS:\n";
        for (int alvo : vertices_alvo) {
            std::cout << "    Pai de " << alvo << ": ";
            if (grafo.distancia[alvo] == -1) {
                std::cout << "inalcancavel\n";
            } else {
                std::cout << grafo.pai[alvo] << '\n';
            }
        }

        grafo.dfs_iterativa(inicio);
        std::cout << "  DFS iterativa:\n";
        for (int alvo : vertices_alvo) {
            std::cout << "    Pai de " << alvo << ": ";
            if (grafo.distancia[alvo] == -1) {
                std::cout << "inalcancavel\n";
            } else {
                std::cout << grafo.pai[alvo] << '\n';
            }
        }
    }

    return 0;
}