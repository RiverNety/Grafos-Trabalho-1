#include "../grafos.cpp"
#include <chrono>
#include <cmath>

// Execute a partir da raiz do projeto. Use --lista para comparar as filas.
int main(int argc, char* argv[]){
    const std::string arquivo = "grafos_analisados/grafo_W_3.txt";
    const int origem = 1;
    bool comparar_lista = argc == 2 && std::string(argv[1]) == "--lista";
    if (argc > 1 && !comparar_lista){
        std::cerr << "Uso: " << argv[0] << " [--lista]\n";
        return 1;
    }

    std::ifstream entrada(arquivo);
    int vertices_esperados = 0;
    if (!(entrada >> vertices_esperados) || vertices_esperados < 1){
        std::cerr << "Erro ao abrir ou ler " << arquivo << "\n";
        return 1;
    }
    entrada.close();

    Grafo grafo(ModoDeRepresentacao::Lista);
    auto inicio = std::chrono::steady_clock::now();
    grafo.ler_grafo(arquivo);
    auto fim = std::chrono::steady_clock::now();
    if (grafo.num_vertices != vertices_esperados || grafo.num_arestas == 0){
        std::cerr << "Falha na leitura do grafo.\n";
        return 1;
    }
    std::size_t adjacencias = 0;
    for (const auto& vizinhos : grafo.vetor_adj){
        adjacencias += vizinhos.size();
    }
    if (adjacencias != 2 * static_cast<std::size_t>(grafo.num_arestas)){
        std::cerr << "Quantidade de adjacencias inconsistente.\n";
        return 1;
    }
    std::cout << "Vertices: " << grafo.num_vertices
              << "\nArestas: " << grafo.num_arestas
              << "\nLeitura (s): " << std::chrono::duration<double>(fim - inicio).count()
              << std::endl;

    int destino = grafo.num_vertices;
    inicio = std::chrono::steady_clock::now();
    float distancia_heap = grafo.djikstra<FilaHeap>(origem, destino);
    fim = std::chrono::steady_clock::now();
    std::cout << "Heap (s): " << std::chrono::duration<double>(fim - inicio).count()
              << "\nDistancia de " << origem << " ate " << destino << ": "
              << distancia_heap << std::endl;

    if (grafo.pai[origem] != -1 || !std::isfinite(distancia_heap)){
        std::cerr << "Resultado invalido do Dijkstra.\n";
        return 1;
    }

    // Confere se a cadeia de pais chega a origem e reproduz a distancia.
    if (distancia_heap >= 0){
        int atual = destino;
        int passos = 0;
        double soma = 0;
        while (atual != origem){
            if (++passos > grafo.num_vertices){
                std::cerr << "Ciclo na cadeia de pais.\n";
                return 1;
            }
            int anterior = grafo.pai[atual];
            if (anterior < 1 || anterior > grafo.num_vertices){
                std::cerr << "Cadeia de pais nao chega a origem.\n";
                return 1;
            }
            float menor_peso = std::numeric_limits<float>::infinity();
            for (const auto& aresta : grafo.vetor_adj[anterior]){
                if (aresta.first == atual){
                    menor_peso = std::min(menor_peso, aresta.second);
                }
            }
            if (!std::isfinite(menor_peso)){
                std::cerr << "Pai sem aresta para o filho.\n";
                return 1;
            }
            soma += menor_peso;
            atual = anterior;
        }
        if (std::fabs(soma - distancia_heap) > 1e-4 * std::max(1.0, soma)){
            std::cerr << "Peso do caminho difere da distancia retornada.\n";
            return 1;
        }
        std::cout << "Caminho validado: " << passos << " arestas.\n";
    }else{
        grafo.bfs(origem);
        if (grafo.distancia[destino] != -1){
            std::cerr << "Dijkstra indicou destino inalcançavel incorretamente.\n";
            return 1;
        }
    }

    if (comparar_lista){
        inicio = std::chrono::steady_clock::now();
        float distancia_lista = grafo.djikstra<FilaLista>(origem, destino);
        fim = std::chrono::steady_clock::now();
        std::cout << "Lista (s): " << std::chrono::duration<double>(fim - inicio).count()
                  << "\nDistancia com lista: " << distancia_lista << std::endl;
        if (std::fabs(distancia_lista - distancia_heap) >
            1e-4f * std::max(1.0f, std::fabs(distancia_heap))){
            std::cerr << "As filas retornaram distancias diferentes.\n";
            return 1;
        }
    }
    std::cout << "Teste concluido com sucesso.\n";
    return 0;
}
