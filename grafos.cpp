#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <queue>
#include <stack>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <utility>
#include <limits>
#include <functional>
#include <random>
#include <stdexcept>

// coloque os grafos em grafos analisados

enum class ModoDeRepresentacao {
    Lista,
    Matriz
};

enum class Direcionamento {
    dir,
    ndir
};

struct ComponenteConexa {
    int id;
    int tamanho;
    std::vector<int> vertices;

    //Refaz o que o operador < deve fazer quando se trata de structs
    bool operator<(const ComponenteConexa& outra) const {
        return tamanho > outra.tamanho;
    }
};

// EntradaDijkstra é o elemento da fila ou heap
using EntradaDijkstra = std::pair<double, int>;

class FilaHeap {
    std::priority_queue<EntradaDijkstra, std::vector<EntradaDijkstra>,
                        std::greater<EntradaDijkstra>> dados;

public:
    //Construtor vazio
    FilaHeap(int = 0){}

    void inserir(double distancia, int vertice){
        dados.push({distancia, vertice});
    }

    EntradaDijkstra minimo(){
        EntradaDijkstra menor = dados.top();
        dados.pop();
        return menor;
    }

    bool empty() const {
        return dados.empty();
    }
};

class FilaLista {
    int n;
    int abertos = 0;
    std::vector<double> estimativa;
    std::vector<bool> aberto;

public:
    FilaLista(int _n = 0) : n(_n), estimativa(_n + 1), aberto(_n + 1, false){}

    void inserir(double distancia, int vertice){
        if (!aberto[vertice]){
            aberto[vertice] = true;
            abertos++;
        }
        estimativa[vertice] = distancia;
    }

    EntradaDijkstra minimo(){
        int melhor = -1;
        for (int v = 1; v <= n; v++){
            if (aberto[v] && (melhor == -1 || estimativa[v] < estimativa[melhor])){
                melhor = v;
            }
        }
        aberto[melhor] = false;
        abertos--;
        return {estimativa[melhor], melhor};
    }

    bool empty() const {
        return abertos == 0;
    }
};

class Grafo {

    public: 

        int num_vertices;
        int num_arestas;
        bool tem_pesos_negativos;

        ModoDeRepresentacao modo;
        Direcionamento direcionamento;

        std::vector<std::vector<std::pair<int, double>>> vetor_adj;
        std::vector<std::vector<std::pair<int, double>>> vetor_adj_t;
        std::vector<std::vector<bool>> matriz_adj;
        std::vector<int> pai;
        std::vector<double> distancia;
        std::vector<std::vector<double>> dist_fw;
        std::vector<std::vector<int>> pai_fw;

        Grafo(ModoDeRepresentacao _modo, Direcionamento _direcionamento = Direcionamento::ndir){
            num_vertices = 0;
            num_arestas = 0;
            tem_pesos_negativos = false;
            modo = _modo;
            direcionamento = _direcionamento;
        }
        
        void restart_distancia(){
            for (int i = 1; i <= num_vertices; i++){
                distancia[i] = -1;
            }
        }

        void restart_pai(){
            for (int i = 1; i <= num_vertices; i++){
                pai[i] = -1;
            }
        }

        void imprimir_adj(){
            for (int i = 1; i <= num_vertices; i++){
                std::cout << i << " -> ";
                for (int j = 0; j < vetor_adj[i].size(); j++){
                    std::cout << "(" << vetor_adj[i][j].first << ", "
                              << vetor_adj[i][j].second << ") ";
                }
                std::cout << std::endl;
            }

        }

        void imprimir_matriz(){
            for (int i = 1; i <= num_vertices; i++){
                for (int j = 1; j <= num_vertices; j++){
                    std::cout << matriz_adj[i][j] << " ";
                }
                std::cout << std::endl;
            }
        }

        //reinicia quando le um arquivo novo
        void ler_grafo(const std::string& arquivo){
            std::ifstream grafo_txt(arquivo);
            if (!grafo_txt){
                throw std::runtime_error("nao foi possivel abrir " + arquivo);
            }

            grafo_txt >> num_vertices;
            num_arestas = 0;
            tem_pesos_negativos = false;
            std::string linha;

            vetor_adj_t.clear();
            if(modo == ModoDeRepresentacao::Lista){
                vetor_adj.assign(num_vertices + 1, std::vector<std::pair<int, double>>());
            }else{
                matriz_adj.assign(num_vertices + 1, std::vector<bool>(num_vertices + 1, 0));
            }
            
            pai.assign(num_vertices + 1, -1);
            distancia.assign(num_vertices + 1, -1);

            while (std::getline(grafo_txt, linha)){
                std::istringstream dados_aresta(linha);
                int u, v;
                if (!(dados_aresta >> u >> v)){
                    continue;
                }
                if (u < 1 || u > num_vertices || v < 1 || v > num_vertices){
                    throw std::runtime_error("vertice fora do intervalo em " + arquivo);
                }
                double peso;
                if (!(dados_aresta >> peso)){
                    peso = 1.0;
                }   
                if (peso < 0.0){
                    tem_pesos_negativos = true;
                }
                num_arestas ++;

                if(modo == ModoDeRepresentacao::Lista){
                    if (direcionamento == Direcionamento::dir) {
                        vetor_adj[u].push_back({v, peso});
                    } else {
                        vetor_adj[v].push_back({u, peso});
                        vetor_adj[u].push_back({v, peso});
                    }
                }else{
                    matriz_adj[u][v] = true;
                    if (direcionamento == Direcionamento::ndir) {
                        matriz_adj[v][u] = true;
                    }
                }
            }

        }

        void construir_transposto(){
            if (modo != ModoDeRepresentacao::Lista || direcionamento == Direcionamento::ndir){
                return;
            }
            if ((int)vetor_adj_t.size() == num_vertices + 1){
                return;
            }
            vetor_adj_t.assign(num_vertices + 1, std::vector<std::pair<int, double>>());
            for (int u = 1; u <= num_vertices; u++){
                for (const auto& aresta : vetor_adj[u]){
                    vetor_adj_t[aresta.first].push_back({u, aresta.second});
                }
            }
        }

        //dfs iterativa
        void dfs_iterativa(int origem){
            
            restart_pai();
            restart_distancia();

            std::stack<std::pair<int, int>> pilha;
            pilha.push({origem, -1});

            while (!pilha.empty()){
                int v = pilha.top().first;
                int p = pilha.top().second;
                pilha.pop();

                if (distancia[v] != -1){
                    continue;
                }
                pai[v] = p;
                distancia[v] = (p == -1) ? 0 : distancia[p] + 1;

                if(modo == ModoDeRepresentacao::Lista){
                    for (auto it = vetor_adj[v].rbegin(); it != vetor_adj[v].rend(); ++it){
                        if (distancia[it->first] == -1){
                            pilha.push({it->first, v});
                        }
                    }
                } else {
                    for (int u = num_vertices; u >= 1; u --){
                        if(matriz_adj[v][u] && distancia[u] == -1){
                            pilha.push({u, v});
                        }
                    }
                }
            }
        }

        void bfs(int v){

            distancia.assign(num_vertices + 1, -1);
            pai.assign(num_vertices + 1, -1);

            if (v < 1 || v > num_vertices){
                std::cerr << "Erro: vertice inicial invalido para BFS." << std::endl;
                return;
            }

            std::queue<int> fila;
            fila.push(v);
            pai[v] = -1;
            distancia[v] = 0;

            while (!fila.empty()){

                v = fila.front();
                fila.pop();
                if(modo == ModoDeRepresentacao::Lista){
                    for (const auto& aresta : vetor_adj[v]){
                        int u = aresta.first;
                        if (distancia[u] == -1){
                            // arvore geradora
                            pai[u] = v;
                            distancia[u] = distancia[v] + 1;
                            fila.push(u);
                        }
                    }
                }else{
                    for (int u = 1; u <= num_vertices; u++){
                        if(matriz_adj[v][u] && distancia[u] == -1){
                            pai[u] = v;
                            distancia[u] = distancia[v] + 1;
                            fila.push(u);
                        }
                    }
                }
            }

        }

        
        void exportar_arvore(const std::string& arquivo_output){
            std::ofstream arquivo(arquivo_output);

            arquivo << "Vertice,Pai,Nivel\n";
            for (int i = 1; i <= num_vertices; ++i){
                arquivo << i << "," << pai[i] << "," << distancia[i] << "\n";
            }
            arquivo.close();

        }

        void representar_arvore_gerada_bfs(const std::string& arquivo_saida, int vertice_inicial){
            bfs(vertice_inicial);
            exportar_arvore(arquivo_saida);
        }

        void representar_arvore_gerada_dfs(const std::string& arquivo_saida, int vertice_inicial){
            dfs_iterativa(vertice_inicial);
            exportar_arvore(arquivo_saida);
        }

        //graus de todos os vértices
        std::vector<int> obter_graus(){
            std::vector<int> graus(num_vertices + 1, 0);
            for (int i = 1; i <= num_vertices; ++i) {
                    if (modo == ModoDeRepresentacao::Lista) {
                        graus[i] = vetor_adj[i].size();
                    } else {
                        int count = 0;
                        for (int j = 1; j <= num_vertices; ++j) {
                            if (matriz_adj[i][j]) count++;
                        }
                        graus[i] = count;
                    }
                }
            return graus;
        }

        //Calcula distancia entre dois vertices por BFS
        int calcular_distancia(int u, int v){
            bfs(u);
            return distancia[v];
        }

        std::vector<int> caminho_ate(int v){
            std::vector<int> caminho;
            if (distancia[v] == std::numeric_limits<double>::infinity()){
                return caminho;
            }
            for (int x = v; x != -1; x = pai[x]){
                caminho.push_back(x);
            }
            std::reverse(caminho.begin(), caminho.end());
            return caminho;
        }

        std::vector<int> caminho_para_destino(int v){
            std::vector<int> caminho;
            if (distancia[v] == std::numeric_limits<double>::infinity()){
                return caminho;
            }
            for (int x = v; x != -1; x = pai[x]){
                caminho.push_back(x);
            }
            return caminho;
        }

        bool bellman_ford_em(const std::vector<std::vector<std::pair<int, double>>>& adj, int origem){
            if (modo != ModoDeRepresentacao::Lista){
                std::cerr << "Erro, bellman_ford apenas para lista de adjacência.\n";
                return false;
            }

            if (origem < 1 || origem > num_vertices){
                std::cerr << "Erro: vértice de origem inválido para Bellman-Ford.\n";
                return false;
            }

            double infinito = std::numeric_limits<double>::infinity();
            distancia.assign(num_vertices + 1, infinito);
            pai.assign(num_vertices + 1, -1);
            distancia[origem] = 0.0f;

            std::vector<int> atualizados(1, origem); //começa com a origem (atualizada pra 0)
            std::vector<bool> adicionado(num_vertices + 1, false);

            for (int i = 0; i < num_vertices; ++i){
                std::vector<int> proximos; // quem vai ser atualizado na proxima iteracao, temporario para cada passo do loop

                for (int u : atualizados){
                    for (const auto& aresta : adj[u]){
                        int v = aresta.first;
                        if (distancia[u] + aresta.second < distancia[v]){
                            distancia[v] = distancia[u] + aresta.second;
                            pai[v] = u;
                            if (!adicionado[v]){
                                proximos.push_back(v); //não adicionar o vértice mais de uma vez
                                adicionado[v] = true;
                            }
                        }
                    }
                }

                for (int v : proximos){
                    adicionado[v] = false;
                }

                if (proximos.empty()){
                    return true;
                }
                if (i == num_vertices - 1){
                    std::cerr << "Erro: ciclo negativo alcancável a partir da origem.\n";
                    return false;
                }

                atualizados.swap(proximos);
            }

            return true;
        }

        bool bellman_ford(int origem){
            return bellman_ford_em(vetor_adj, origem);
        }

        bool bellman_ford_para_destino(int destino){
            construir_transposto();
            if (direcionamento == Direcionamento::dir){
                return bellman_ford_em(vetor_adj_t, destino);
            }
            return bellman_ford_em(vetor_adj, destino);
        }

        bool floyd_warshall(){
            if (modo != ModoDeRepresentacao::Lista){
                std::cerr << "Erro, floyd_warshall apenas para lista de adjacência.\n";
                return false;
            }

            double infinito = std::numeric_limits<double>::infinity();
            dist_fw.assign(num_vertices + 1, std::vector<double>(num_vertices + 1, infinito));
            pai_fw.assign(num_vertices + 1, std::vector<int>(num_vertices + 1, -1));

            for (int u = 1; u <= num_vertices; u++){
                dist_fw[u][u] = 0.0;
            }
            for (int u = 1; u <= num_vertices; u++){
                for (const auto& aresta : vetor_adj[u]){
                    int v = aresta.first;
                    if (aresta.second < dist_fw[u][v]){
                        dist_fw[u][v] = aresta.second;
                        pai_fw[u][v] = u;
                    }
                }
            }

            for (int i = 1; i <= num_vertices; i++){
                if (dist_fw[i][i] < 0){
                    return false;
                }
            }

            for (int k = 1; k <= num_vertices; k++){
                for (int i = 1; i <= num_vertices; i++){
                    if (dist_fw[i][k] == infinito){
                        continue;
                    }
                    for (int j = 1; j <= num_vertices; j++){
                        double nova = dist_fw[i][k] + dist_fw[k][j];
                        if (nova < dist_fw[i][j]){
                            dist_fw[i][j] = nova;
                            pai_fw[i][j] = pai_fw[k][j];
                        }
                    }
                }
                for (int i = 1; i <= num_vertices; i++){
                    if (dist_fw[i][i] < 0){
                        return false;
                    }
                }
            }
            return true;
        }

        std::vector<int> caminho_fw(int i, int j){
            std::vector<int> caminho;
            if (dist_fw[i][j] == std::numeric_limits<double>::infinity()){
                return caminho;
            }
            for (int x = j; x != i; x = pai_fw[i][x]){
                caminho.push_back(x);
            }
            caminho.push_back(i);
            std::reverse(caminho.begin(), caminho.end());
            return caminho;
        }

        template<class TipoFila>
        double djikstra(int origem, int destino = -1){
            pai.assign(num_vertices + 1, -1);

            //implementado apenas para lista
            if (modo != ModoDeRepresentacao::Lista){
                std::cerr << "Erro, djikstra implementado apenas para lista de adjacência.\n";
                return -1.0;
            }

            if (tem_pesos_negativos){
                std::cerr << "Erro, djikstra não suporta pesos negativos.\n";
                return -1.0;
            }

            //vértice válido
            if (origem < 1 || origem > num_vertices ||
                (destino != -1 && (destino < 1 || destino > num_vertices))){
                std::cerr << "Erro: vártice inválido.\n";
                return -1.0;
            }

            double infinito = std::numeric_limits<double>::infinity();
            distancia.assign(num_vertices + 1, infinito);
            //abstração usando TipoFila, classes que implementam as 3 funções inserir, minimo e empty
            TipoFila fila(num_vertices);
            distancia[origem] = 0.0;
            fila.inserir(0.0, origem);

            while (!fila.empty()){
                auto entrada = fila.minimo();
                double distancia_atual = entrada.first;
                int atual = entrada.second;

                //descarta caminhos maiores até um vertice v, semelhante a sugestão do professor
                //se o custo que eu estou vendo agora no topo da hea (minimo) eh maior que o que ja esta registrado
                //nao precisa processar (continue), se for menor processa de novo
                //quando um vertice sasi da heap ele ja tem distancia menor, por isso essa comparacao vale
                if (distancia_atual > distancia[atual]){
                    continue;
                }

                //ver vizinhos e atualizar distancias
                for (auto& aresta : vetor_adj[atual]){
                    int vizinho = aresta.first;
                    double nova_distancia = distancia_atual + aresta.second;
                    if (nova_distancia < distancia[vizinho]){
                        distancia[vizinho] = nova_distancia;
                        pai[vizinho] = atual;
                        fila.inserir(nova_distancia, vizinho);
                    }
                }
            }

            if (destino == -1){
                return 0.0;
            }

            //nao há caminho
            if (distancia[destino] == infinito){
                std::cout << "Não há caminho entre " << origem << " e " << destino << "\n";
                return -1.0;
            }
            return distancia[destino];
        }

        //Faz o calculo do diametro exato
        int calcular_diametro_exato(){
            int max_distancia = 0;
            for(int i = 1; i <= num_vertices; ++i){
                bfs(i);
                for (int j = 1; j <= num_vertices; ++j){
                    //Faz BFS em todos os vertices e substitui pela maior distancia
                    if (distancia[j] > max_distancia){
                        max_distancia = distancia[j];
                    }
                }
            }
            return max_distancia;
        }

        //Faz um calculo aproximado do diametro usando o algoritmo de Duplo Sweep
        int calcular_diametro_aproximado(int quantidade = 10){
            std::mt19937 gerador(42);
            std::uniform_int_distribution<int> sorteio(1, num_vertices);
            int max_distancia = 0;
            for(int t = 0; t < quantidade; ++t){
                int s = sorteio(gerador);
                int anterior = -1;
                while (true){
                    bfs(s);

                    int vertice_mais_distante = s;
                    int max_d_local = 0;
                    for(int i = 1; i <= num_vertices; ++i){
                        if(distancia[i] > max_d_local) {
                            max_d_local = distancia[i];
                            vertice_mais_distante = i;
                        }
                    }
                    if (max_d_local <= anterior){
                        break;
                    }
                    anterior = max_d_local;
                    s = vertice_mais_distante;
                }
                max_distancia = std::max(max_distancia, anterior);
            }
            return max_distancia;
        }

        void imprimir_distancia(int v){
            std::cout << "Distancia de " << v << " ate" << std::endl;
            for (int i = 1; i <= num_vertices; i++){
                std::cout << i << " -> " << distancia[i] << std::endl;
            }
        }

        std::vector<ComponenteConexa> obter_componentes_conexas(){
            bool dir = (direcionamento == Direcionamento::dir);
            if (dir) construir_transposto();

            std::vector<bool> visitado(num_vertices + 1, false);
            std::vector<ComponenteConexa> componentes;
            int comp_id = 1;

            for (int i = 1; i <= num_vertices; ++i) {
                if (!visitado[i]) {
                    ComponenteConexa comp;
                    comp.id = comp_id++;
                    
                    std::queue<int> lista;
                    lista.push(i);
                    visitado[i] = true;

                    while (!lista.empty()) {
                        int u = lista.front();
                        lista.pop();
                        comp.vertices.push_back(u);

                        if (modo == ModoDeRepresentacao::Lista) {
                            for (const auto& aresta : vetor_adj[u]) {
                                int v = aresta.first;
                                if (!visitado[v]) {
                                    visitado[v] = true;
                                    lista.push(v);
                                }
                            }
                            if (dir) {
                                for (const auto& aresta : vetor_adj_t[u]) {
                                    int v = aresta.first;
                                    if (!visitado[v]) {
                                        visitado[v] = true;
                                        lista.push(v);
                                    }
                                }
                            }
                        } else {
                            for (int v = 1; v <= num_vertices; ++v) {
                                if ((matriz_adj[u][v] || (dir && matriz_adj[v][u])) && !visitado[v]) {
                                    visitado[v] = true;
                                    lista.push(v);
                                }
                            }
                        }
                    }
                    comp.tamanho = comp.vertices.size();
                    componentes.push_back(comp);
                }
            }

            std::sort(componentes.begin(), componentes.end());
            return componentes;
        }

        void imprimir_relatorio_grafo(const std::string& arquivo_saida){
            std::ofstream arquivo(arquivo_saida);
            std::vector<int>graus = obter_graus();

            std::vector<int>graus_validos(graus.begin() + 1, graus.end());
            std::sort(graus_validos.begin(), graus_validos.end());

            int grau_min = graus_validos.front();
            int grau_max = graus_validos.back();

            double fator = (direcionamento == Direcionamento::dir) ? 1.0 : 2.0;
            double grau_medio = (fator * num_arestas) / static_cast<double>(num_vertices);
            
            double mediana_grau;
            int n = graus_validos.size();
            if(n%2 == 0){
                mediana_grau = (graus_validos[n/2 - 1] + graus_validos[n/2])/2.0;
            }else{
                mediana_grau = graus_validos[n/2];
            }

            //A partir daqui é onde de fato imprimimos as informações necessárias
            arquivo << "Dados do grafo \n";
            arquivo << "Numero de Vertices: " << num_vertices << "\n";
            arquivo << "Numero de Arestas: " << num_arestas << "\n";
            arquivo << "Grau Minimo: " << grau_min << "\n";
            arquivo << "Grau Maximo: " << grau_max << "\n";
            arquivo << "Grau Medio: " << std::fixed << std::setprecision(2) << grau_medio << "\n";
            arquivo << "Mediana de Grau: " << mediana_grau << "\n\n";

            auto componentes_conexas = obter_componentes_conexas();
            arquivo << "Componentes Conexas: " << componentes_conexas.size() << "\n";
            for (size_t i = 0; i < componentes_conexas.size(); ++i) {
                arquivo << "Componente " << i + 1 << " - Tamanho: " << componentes_conexas[i].tamanho << "\n";
                arquivo << "Vertices: ";
                for (int v : componentes_conexas[i].vertices) arquivo << v << " ";
                arquivo << "\n\n";
            }
            arquivo.close();

        }
    
};