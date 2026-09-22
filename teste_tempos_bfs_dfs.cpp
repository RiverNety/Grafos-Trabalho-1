#include "grafos.cpp"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

const int QUANTIDADE_EXECUCOES = 100;

using Relogio = std::chrono::high_resolution_clock;
using Duracao = std::chrono::duration<double, std::nano>;

std::vector<double> medir_bfs(Grafo& grafo, int vertice_inicial) {
    std::vector<double> tempos;
    tempos.reserve(QUANTIDADE_EXECUCOES);

    for (int i = 0; i < QUANTIDADE_EXECUCOES; ++i) {
        auto inicio = Relogio::now();
        grafo.bfs(vertice_inicial);
        auto fim = Relogio::now();
        tempos.push_back(Duracao(fim - inicio).count());
    }

    return tempos;
}

std::vector<double> medir_dfs(Grafo& grafo, int vertice_inicial) {
    std::vector<double> tempos;
    tempos.reserve(QUANTIDADE_EXECUCOES);

    for (int i = 0; i < QUANTIDADE_EXECUCOES; ++i) {
        auto inicio = Relogio::now();
        grafo.dfs_iterativa(vertice_inicial);
        auto fim = Relogio::now();
        tempos.push_back(Duracao(fim - inicio).count());
    }

    return tempos;
}

double calcular_media(const std::vector<double>& tempos) {
    double soma = 0.0;
    for (double tempo : tempos) {
        soma += tempo;
    }
    return soma / tempos.size();
}

double calcular_desvio_padrao(const std::vector<double>& tempos, double media) {
    double soma_quadrados = 0.0;
    for (double tempo : tempos) {
        double diferenca = tempo - media;
        soma_quadrados += diferenca * diferenca;
    }
    return std::sqrt(soma_quadrados / tempos.size());
}

void imprimir_resultado(const std::string& algoritmo,
                        const std::vector<double>& tempos) {
    double media = calcular_media(tempos);
    double desvio_padrao = calcular_desvio_padrao(tempos, media);

    std::cout << algoritmo << ":\n";
    std::cout << "  Media: " << media << " ns\n";
    std::cout << "  Desvio padrao: " << desvio_padrao << " ns\n";
}

int main() {
    std::cout << std::fixed << std::setprecision(2);

    for (int numero_grafo = 1; numero_grafo <= 6; ++numero_grafo) {
        std::string caminho = "grafos analisados/grafo_" +
                              std::to_string(numero_grafo) + ".txt";
        Grafo grafo(ModoDeRepresentacao::Lista);
        grafo.ler_grafo(caminho);

        std::vector<double> tempos_bfs = medir_bfs(grafo, 1);
        std::vector<double> tempos_dfs = medir_dfs(grafo, 1);

        std::cout << "\nGrafo " << numero_grafo << ":\n";
        imprimir_resultado("BFS", tempos_bfs);
        imprimir_resultado("DFS", tempos_dfs);
    }

    return 0;
}
