#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include "grafos.cpp"

static std::string nome_saida_padrao(const std::string& entrada) {
    const std::size_t barra = entrada.find_last_of("/\\");
    std::string nome = (barra == std::string::npos) ? entrada : entrada.substr(barra + 1);
    const std::size_t ponto = nome.find_last_of('.');
    if (ponto != std::string::npos) nome.resize(ponto);
    return nome + "_estudo2.txt";
}

static std::string formatar_distancia(double d) {
    if (std::isinf(d)) return (d < 0) ? "-INF" : "INF";
    return std::to_string(d);
}

static std::vector<int> reconstruir_caminho_seguro(const Grafo& grafo, int origem, int destino) {
    std::vector<int> caminho;
    if (!std::isfinite(grafo.dist_fw[origem][destino])) return caminho;

    int atual = destino;
    caminho.push_back(atual);

    // O limite evita um loop infinito caso a matriz de predecessores esteja inconsistente.
    for (int passos = 0; atual != origem && passos < grafo.num_vertices; ++passos) {
        atual = grafo.pai_fw[origem][atual];
        if (atual < 1 || atual > grafo.num_vertices) return {};
        caminho.push_back(atual);
    }

    if (caminho.back() != origem) return {};
    std::reverse(caminho.begin(), caminho.end());
    return caminho;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Uso: " << argv[0] << " grafo_W_1.txt [resultado.txt]\n";
        return 1;
    }

    const std::string arquivo_entrada = argv[1];
    const std::string arquivo_saida = (argc == 3) ? argv[2] : nome_saida_padrao(arquivo_entrada);

    try {
        Grafo grafo(ModoDeRepresentacao::Lista, Direcionamento::dir);
        grafo.ler_grafo(arquivo_entrada);

        const int vertices[] = {10, 20, 30, 40, 50};
        if (grafo.num_vertices < 50) {
            throw std::runtime_error("o grafo precisa ter pelo menos 50 vertices");
        }

        std::ofstream saida(arquivo_saida);
        if (!saida) throw std::runtime_error("nao foi possivel criar " + arquivo_saida);

        saida << "ESTUDO DE CASO 2 - FLOYD-WARSHALL\n";
        saida << "Grafo: " << arquivo_entrada << "\n";
        saida << "Vertices: " << grafo.num_vertices << "\n";
        saida << "Arestas: " << grafo.num_arestas << "\n\n";
        saida << std::fixed << std::setprecision(6);

        const bool sem_ciclo_negativo = grafo.floyd_warshall();
        if (!sem_ciclo_negativo) {
            saida << "CICLO NEGATIVO DETECTADO.\n";
            saida << "A implementacao atual interrompe Floyd-Warshall ao detectar o ciclo.\n";
            saida << "Portanto, a matriz parcial nao sera apresentada como resultado final.\n";
            std::cout << "Ciclo negativo detectado. Aviso salvo em: " << arquivo_saida << "\n";
            return 0;
        }

        saida << "MATRIZ DE DISTANCIAS (linhas = origem; colunas = destino)\n\t";
        for (int v : vertices) saida << v << '\t';
        saida << '\n';

        for (int origem : vertices) {
            saida << origem << '\t';
            for (int destino : vertices) {
                saida << formatar_distancia(grafo.dist_fw[origem][destino]) << '\t';
            }
            saida << '\n';
        }

        saida << "\nCAMINHOS MINIMOS ENTRE OS PARES\n";
        for (int origem : vertices) {
            for (int destino : vertices) {
                saida << origem << " -> " << destino << ": ";
                if (!std::isfinite(grafo.dist_fw[origem][destino])) {
                    saida << "nao existe caminho\n";
                    continue;
                }

                const std::vector<int> caminho = reconstruir_caminho_seguro(grafo, origem, destino);
                if (caminho.empty()) {
                    saida << "nao foi possivel reconstruir\n";
                    continue;
                }

                for (std::size_t i = 0; i < caminho.size(); ++i) {
                    if (i) saida << " -> ";
                    saida << caminho[i];
                }
                saida << " | distancia = " << formatar_distancia(grafo.dist_fw[origem][destino]) << '\n';
            }
        }

        std::cout << "Resultado salvo em: " << arquivo_saida << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}