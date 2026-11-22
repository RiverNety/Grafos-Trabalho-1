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
    return nome + "_estudo1.txt";
}

static std::string formatar_distancia(double d) {
    if (std::isinf(d)) return (d < 0) ? "-INF" : "INF";
    return std::to_string(d);
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

        std::ofstream saida(arquivo_saida);
        if (!saida) throw std::runtime_error("nao foi possivel criar " + arquivo_saida);

        saida << "ESTUDO DE CASO 1 - BELLMAN-FORD ATE O VERTICE 100\n";
        saida << "Grafo: " << arquivo_entrada << "\n";
        saida << "Vertices: " << grafo.num_vertices << "\n";
        saida << "Arestas: " << grafo.num_arestas << "\n\n";
        saida << std::fixed << std::setprecision(6);

        const bool sem_ciclo_negativo = grafo.bellman_ford_para_destino(100);

        if (!sem_ciclo_negativo) {
            saida << "ATENCAO: Bellman-Ford detectou ciclo negativo alcancavel no grafo transposto.\n";
            saida << "Isso significa que existe ciclo negativo a partir do qual e possivel chegar ao vertice 100.\n";
            saida << "As estimativas abaixo nao devem ser interpretadas como distancias minimas validas.\n\n";
        }

        const int vertices[] = {10, 20, 30};
        for (int origem : vertices) {
            saida << "Origem: " << origem << " | Destino: 100\n";
            saida << "Distancia: " << formatar_distancia(grafo.distancia[origem]) << "\n";

            if (sem_ciclo_negativo && std::isfinite(grafo.distancia[origem])) {
                const std::vector<int> caminho = grafo.caminho_para_destino(origem);
                if (caminho.empty()) {
                    saida << "Caminho: nao encontrado\n";
                } else {
                    saida << "Caminho: ";
                    for (std::size_t i = 0; i < caminho.size(); ++i) {
                        if (i) saida << " -> ";
                        saida << caminho[i];
                    }
                    saida << "\n";
                }
            } else if (std::isinf(grafo.distancia[origem]) && grafo.distancia[origem] > 0) {
                saida << "Caminho: nao existe caminho direcionado ate 100\n";
            } else if (!sem_ciclo_negativo) {
                saida << "Caminho: nao reconstruido porque ha ciclo negativo relevante\n";
            }
            saida << "\n";
        }

        std::cout << "Resultado salvo em: " << arquivo_saida << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

