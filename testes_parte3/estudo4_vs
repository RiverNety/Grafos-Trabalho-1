#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <chrono>
#include <streambuf>
#include <stdexcept>
#include "grafos.cpp"

class BufferSilencioso : public std::streambuf {
protected:
    int overflow(int caractere) override { return caractere; }
};

static std::string nome_saida_padrao(const std::string& entrada) {
    const std::size_t barra = entrada.find_last_of("/\\");
    std::string nome = (barra == std::string::npos) ? entrada : entrada.substr(barra + 1);
    const std::size_t ponto = nome.find_last_of('.');
    if (ponto != std::string::npos) nome.resize(ponto);
    return nome + "_estudo4.txt";
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
        const int n = grafo.num_vertices;
        if (n <= 0) throw std::runtime_error("grafo sem vertices");

        constexpr int RODADAS = 10;
        using Relogio = std::chrono::steady_clock;
        double tempo_total_bf = 0.0;
        double tempo_total_fw = 0.0;
        int rodadas_com_ciclo_bf = 0;
        int rodadas_com_ciclo_fw = 0;

        BufferSilencioso buffer_nulo;
        std::streambuf* buffer_anterior = std::cerr.rdbuf(&buffer_nulo);

        // Bellman-Ford e executado uma vez para cada origem. Assim, ao final
        // de uma rodada, foram calculadas distancias de todos os pares alcancaveis.
        for (int rodada = 0; rodada < RODADAS; ++rodada) {
            const auto inicio = Relogio::now();
            bool rodada_sem_ciclo = true;
            for (int origem = 1; origem <= n; ++origem) {
                if (!grafo.bellman_ford(origem)) rodada_sem_ciclo = false;
            }
            const auto fim = Relogio::now();
            tempo_total_bf += std::chrono::duration<double>(fim - inicio).count();
            if (!rodada_sem_ciclo) ++rodadas_com_ciclo_bf;
        }

        // Floyd-Warshall calcula todos os pares em uma unica execucao.
        for (int rodada = 0; rodada < RODADAS; ++rodada) {
            const auto inicio = Relogio::now();
            const bool sucesso = grafo.floyd_warshall();
            const auto fim = Relogio::now();
            tempo_total_fw += std::chrono::duration<double>(fim - inicio).count();
            if (!sucesso) ++rodadas_com_ciclo_fw;
        }

        std::cerr.rdbuf(buffer_anterior);

        const double media_bf = tempo_total_bf / RODADAS;
        const double media_fw = tempo_total_fw / RODADAS;

        std::ofstream saida(arquivo_saida);
        if (!saida) throw std::runtime_error("nao foi possivel criar " + arquivo_saida);

        saida << "ESTUDO DE CASO 4 - BELLMAN-FORD TODOS OS PARES VS FLOYD-WARSHALL\n";
        saida << "Grafo: " << arquivo_entrada << "\n";
        saida << "Vertices: " << n << "\n";
        saida << "Arestas: " << grafo.num_arestas << "\n";
        saida << "Rodadas: " << RODADAS << "\n\n";
        saida << std::fixed << std::setprecision(9);
        saida << "Bellman-Ford para todos os pares:\n";
        saida << "  Uma rodada = executar Bellman-Ford para cada origem (1 ate " << n << ").\n";
        saida << "  Tempo medio por rodada: " << media_bf << " segundos\n";
        saida << "  Rodadas que detectaram ciclo negativo: " << rodadas_com_ciclo_bf << "/" << RODADAS << "\n\n";

        saida << "Floyd-Warshall:\n";
        saida << "  Uma rodada = uma execucao para todos os pares.\n";
        saida << "  Tempo medio por rodada: " << media_fw << " segundos\n";
        saida << "  Rodadas que detectaram ciclo negativo: " << rodadas_com_ciclo_fw << "/" << RODADAS << "\n\n";

        if (media_bf < media_fw) {
            saida << "Mais eficiente nesta execucao: Bellman-Ford repetido para todas as origens.\n";
        } else if (media_fw < media_bf) {
            saida << "Mais eficiente nesta execucao: Floyd-Warshall.\n";
        } else {
            saida << "Os tempos medios foram iguais dentro da precisao medida.\n";
        }

        saida << "Se houver ciclos negativos, as distancias minimas podem nao estar definidas para\n";
        saida << "os pares afetados; nesse caso, considere tambem os indicadores de ciclo acima.\n";

        std::cout << "Resultado salvo em: " << arquivo_saida << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

