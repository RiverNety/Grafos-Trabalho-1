#include "grafos.cpp"

int main() {
    Grafo grafo(ModoDeRepresentacao::Lista);
    grafo.ler_grafo("grafos analisados/grafo_1.txt");

    std::cout << "Vertices: " << grafo.num_vertices << "\n";
    std::cout << "Arestas: " << grafo.num_arestas << "\n";
    std::cout << "Grau do vertice 1: " << grafo.vetor_adj[1].size() << "\n";

    return 0;
}
