#include "grafos.cpp"
#include <filesystem>

int main() {
    namespace fs = std::filesystem;

    for (int i = 1; i <= 6; ++i) {
        std::string nome_grafo = "grafo_" + std::to_string(i) + ".txt";
        std::string caminho_entrada = "grafos analisados/" + nome_grafo;
        std::string pasta_saida = "grafos analisados/testes/grafo_" + std::to_string(i);

        fs::create_directories(pasta_saida);

        Grafo grafo(ModoDeRepresentacao::Lista);
        grafo.ler_grafo(caminho_entrada);

        grafo.representar_arvore_gerada_bfs(pasta_saida + "/arvore_bfs.csv", 1);
        grafo.representar_arvore_gerada_dfs(pasta_saida + "/arvore_dfs.csv", 1);
        grafo.imprimir_relatorio_grafo(pasta_saida + "/relatorio_grafo.txt");

        std::cout << "Processado: " << nome_grafo << " -> " << pasta_saida << "\n";
    }

    return 0;
}
