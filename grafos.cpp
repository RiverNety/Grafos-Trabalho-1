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

// coloque os grafos em grafos analisados

enum class ModoDeRepresentacao {
    Lista,
    Matriz
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
using EntradaDijkstra = std::pair<float, int>;

class FilaHeap {
    std::priority_queue<EntradaDijkstra, std::vector<EntradaDijkstra>,
                        std::greater<EntradaDijkstra>> dados;

public:
    void inserir(float distancia, int vertice){
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
    std::vector<EntradaDijkstra> dados;

public:
    void inserir(float distancia, int vertice){
        dados.push_back({distancia, vertice});
    }

    EntradaDijkstra minimo(){
        auto posicao = std::min_element(dados.begin(), dados.end());
        EntradaDijkstra menor = *posicao;
        // esse erase o(n)
        dados.erase(posicao);
        return menor;
    }

    bool empty() const {
        return dados.empty();
    }
};

class Grafo {

    public: 

        int num_vertices;
        int num_arestas;

        ModoDeRepresentacao modo;

        std::vector<std::vector<std::pair<int, float>>> vetor_adj;
        std::vector<std::vector<bool>> matriz_adj;
        std::vector<int> pai;
        std::vector<float> distancia;

        Grafo(ModoDeRepresentacao _modo){
            num_vertices = 0;
            num_arestas = 0;
            modo = _modo;
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

            grafo_txt >> num_vertices;
            num_arestas = 0;
            std::string linha;

            if(modo == ModoDeRepresentacao::Lista){
                vetor_adj.assign(num_vertices + 1, std::vector<std::pair<int, float>>());
            }else{
                matriz_adj.assign(num_vertices + 1, std::vector<bool>(num_vertices + 1, 0));
            }
            
            pai.assign(num_vertices + 1, -1);
            distancia.resize(num_vertices + 1, -1);

            while (std::getline(grafo_txt, linha)){
                std::istringstream dados_aresta(linha);
                int u, v;
                if (!(dados_aresta >> u >> v)){
                    continue;
                }
                float peso;
                if (!(dados_aresta >> peso)){
                    peso = 1.0f;
                }   
                if (peso < 0.0f){
                    std::cout << "Erro, pesos negativos ainda não suportados." << std::endl;
                    //reset
                    num_vertices = 0;
                    num_arestas = 0;
                    vetor_adj.clear();
                    matriz_adj.clear();
                    restart_pai();
                    restart_distancia();
                    break;
                }

                num_arestas ++;

                //grafo nao direcionado
                if(modo == ModoDeRepresentacao::Lista){
                    vetor_adj[v].push_back({u, peso});
                    vetor_adj[u].push_back({v, peso});
                }else{
                    matriz_adj[u][v] = true;
                    matriz_adj[v][u] = true;
                }
            }

        }

        //dfs iterativa
        void dfs_iterativa(int v){
            
            restart_pai();
            restart_distancia();

            std::stack<int> pilha;

            pilha.push(v);
            distancia[v] = 0;
            pai[v] = -1;

            while (!pilha.empty()){
                v = pilha.top();
                pilha.pop();

                if(modo == ModoDeRepresentacao::Lista){
                    for (const auto& aresta : vetor_adj[v]){
                        int u = aresta.first;
                        if (distancia[u] == -1){
                            
                            distancia[u] = distancia[v] + 1;
                            pai[u] = v;
                            pilha.push(u);
                        }
                    }
                } else {
                    for (int u = num_vertices; u >= 1; u --){

                        if(matriz_adj[v][u] && distancia[u] == -1){
                            distancia[u] = distancia[v] + 1;
                            pai[u] = v;
                            pilha.push(u);
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

        template<class TipoFila>
        float djikstra(int origem, int destino = -1){
            pai.assign(num_vertices + 1, -1);

            //implementado apenas para lista
            if (modo != ModoDeRepresentacao::Lista){
                std::cerr << "Erro, djikstra implementado apenas para lista de adjacência.\n";
                return -1.0f;
            }

            //vértice válido
            if (origem < 1 || origem > num_vertices ||
                (destino != -1 && (destino < 1 || destino > num_vertices))){
                std::cerr << "Erro: vártice inválido.\n";
                return -1.0f;
            }

            float infinito = std::numeric_limits<float>::infinity();
            distancia.assign(num_vertices + 1, infinito);
            //abstração usando TipoFila, classes que implementam as 3 funções inserir, minimo e empty
            TipoFila fila;
            distancia[origem] = 0.0f;
            fila.inserir(0.0f, origem);

            while (!fila.empty()){
                auto entrada = fila.minimo();
                float distancia_atual = entrada.first;
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
                    float nova_distancia = distancia_atual + aresta.second;
                    if (nova_distancia < distancia[vizinho]){
                        distancia[vizinho] = nova_distancia;
                        pai[vizinho] = atual;
                        fila.inserir(nova_distancia, vizinho);
                    }
                }
            }

            if (destino == -1){
                return 0.0f;
            }

            //nao há caminho
            if (distancia[destino] == infinito){
                std::cout << "Não há caminho entre " << origem << " e " << destino << "\n";
                return -1.0f;
            }
            return distancia[destino];
        }

        //Faz o calculo do diametro exato
        int calcular_diametro_exato(int vertice_inicial = 1){
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
            int max_distancia = 0;
            for(int s = 1; s <= std::min(num_vertices, quantidade); ++s){
                bfs(s);

                int vertice_mais_distante = 0;
                int max_d_local = 0;
                for(int i = 1; i <= num_vertices; ++i){
                    if(distancia[i] > max_d_local) {
                        max_d_local = distancia[i];
                        vertice_mais_distante = i;
                    }
                }

                //Faz outra BFS num nó ainda mais distante
                bfs(vertice_mais_distante);
                for(int i = 1; i <= num_vertices; ++i){
                    if(distancia[i] > max_distancia){
                        max_distancia = distancia[i];
                    }
                }
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
                        } else {
                            for (int v = 1; v <= num_vertices; ++v) {
                                if (matriz_adj[u][v] && !visitado[v]) {
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

            double grau_medio = (2.0 * num_arestas) / static_cast<double>(num_vertices);
            
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
