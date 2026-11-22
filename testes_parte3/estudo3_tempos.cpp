#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <chrono>
#include <sstream>
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
    return nome + "_estudo3.txt";
}

template <typename Funcao>
static double medir_media_segundos(Funcao funcao, int rodadas, int& falhas) {
    using Relogio = std::chrono::steady_clock;
    double total = 0.0;
    falhas = 0;

    for (int i = 0; i < rodadas; ++i) {
        const auto inicio = Relogio::now();
        const bool sucesso = funcao();
        const auto fim = Relogio::now();
        total += std::chrono::duration<double>(fim - inicio).count();
        if (!sucesso) ++falhas;
    }

    return total / rodadas;
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
        if (grafo.num_vertices < 100) {
            throw std::runtime_error("o grafo precisa ter pelo menos 100 vertices");
        }

        grafo.construir_transposto();

        // A biblioteca escreve mensagens de erro em cerr ao detectar ciclo negativo.
        // silenciamos essas mensagens durante as medicoes para nao incluir outros fatores no tempo
        BufferSilencioso buffer_nulo;
        std::streambuf* buffer_anterior = std::cerr.rdbuf(&buffer_nulo);

        constexpr int RODADAS = 10;
        int falhas_bf = 0;
        int falhas_fw = 0;

        // Para Bellman-Ford, medimos o caso de um destino especifico solicitado
        // no estudo 1: calcular os caminhos ate o vertice 100.
        const double media_bf = medir_media_segundos(
            [&grafo]() { return grafo.bellman_ford_para_destino(100); },
            RODADAS, falhas_bf);

        // Floyd-Warshall calcula os caminhos entre todos os pares.
        const double media_fw = medir_media_segundos(
            [&grafo]() { return grafo.floyd_warshall(); },
            RODADAS, falhas_fw);

        std::cerr.rdbuf(buffer_anterior);

        std::ofstream saida(arquivo_saida);
        if (!saida) throw std::runtime_error("nao foi possivel criar " + arquivo_saida);

        saida << "ESTUDO DE CASO 3 - TEMPO MEDIO DE EXECUCAO\n";
        saida << "Grafo: " << arquivo_entrada << "\n";
        saida << "Vertices: " << grafo.num_vertices << "\n";
        saida << "Arestas: " << grafo.num_arestas << "\n";
        saida << "Rodadas: " << RODADAS << "\n\n";
        saida << std::fixed << std::setprecision(9);
        saida << "Bellman-Ford (ate o vertice 100): " << media_bf << " segundos\n";
        saida << "Rodadas que detectaram ciclo negativo: " << falhas_bf << "/" << RODADAS << "\n\n";
        saida << "Floyd-Warshall (todos os pares): " << media_fw << " segundos\n";
        saida << "Rodadas que detectaram ciclo negativo: " << falhas_fw << "/" << RODADAS << "\n\n";
        saida << "OBSERVACAO: os escopos dos calculos diferem: Bellman-Ford foi executado para um destino,\n";
        saida << "enquanto Floyd-Warshall calcula todos os pares. O estudo 4 faz a comparacao entre\n";
        saida << "Bellman-Ford para todos os pares e Floyd-Warshall para todos os pares.\n";
        saida << "A leitura do arquivo e a escrita deste relatorio ficam fora da medicao.\n";

        std::cout << "Resultado salvo em: " << arquivo_saida << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

