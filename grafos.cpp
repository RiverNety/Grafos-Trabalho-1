#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <queue>
#include <stack>

using namespace std;


int vertices;
vector<vector<int>> vetor_adj;
vector<vector<int>> matriz_adj;
vector<bool> visitados;
vector<int> pai;
vector<int> distancia;

//reinicia quando le um arquivo novo
void ler_grafo(string arquivo){
    ifstream grafo_txt(arquivo);
    string linha;

    getline(grafo_txt, linha);

    //ajuste de tamanho
    vertices = stoi(linha);
    matriz_adj.resize(vertices + 1, vector<int>(vertices + 1, 0));
    vetor_adj.resize(vertices + 1, vector<int>(0));
    visitados.resize(vertices + 1, false);
    pai.resize(vertices + 1, -1);
    distancia.resize(vertices + 1, -1);  
    while(getline(grafo_txt, linha)){
        int u, v;
        sscanf(linha.c_str(), "%d %d", &u, &v);

        //grafo nao direcionado
        vetor_adj[u].push_back(v);
        vetor_adj[v].push_back(u);
        matriz_adj[u][v] = 1;
        matriz_adj[v][u] = 1;
    }

}

void restart_distancia(){
    for (int i = 1; i <= vertices; i++){
        distancia[i] = -1;
    }
}
void restart_visitados(){
    for (int i = 1; i <= vertices; i++){
        visitados[i] = false;
    }
}

void restart_pai(){
    for (int i = 1; i <= vertices; i++){
        pai[i] = -1;
    }
}

void imprimir_adj(){
    for (int i = 1; i <= vertices; i++){
        cout << i << " -> ";
        for (int j = 0; j < vetor_adj[i].size(); j++){
            cout << vetor_adj[i][j] << " ";
        }
        cout << endl;
    }

}

//como colocar os indices?
void imprimir_matriz(){
    for (int i = 1; i <= vertices; i++){
        for (int j = 1; j <= vertices; j++){
            cout << matriz_adj[i][j] << " ";
        }
        cout << endl;
    }
}

//dfs recursiva
void dfs(int v){
    visitados[v] = true;
    for (int u : vetor_adj[v]){
        if (!visitados[u]){
            dfs(u);
        }
    }
}

//dfs iterativa
void dfs_iterativa(int v){
    
    restart_visitados();
    restart_pai();

    stack<int> pilha;
    pilha.push(v);
    visitados[v] = true;
    pai[v] = -1;

    while (!pilha.empty()){
        v = pilha.top();
        pilha.pop();

        for (int u : vetor_adj[v]){
            if (!visitados[u]){
                visitados[u] = true;
                pai[u] = v;
                pilha.push(u);
            }
        }
    }
}

void bfs(int v){

    restart_distancia();
    restart_visitados();
    restart_pai();

    queue<int> fila;
    visitados[v] = true;
    fila.push(v);
    pai[v] = -1;
    distancia[v] = 0;
    while (!fila.empty()){
        v = fila.front();
        fila.pop();
        for (int u : vetor_adj[v]){
            if (!visitados[u]){
                fila.push(u);
                visitados[u] = true;
                pai[u] = v;
                distancia[u] = distancia[v] + 1;
            }
        }
    }

}

void imprimir_distancia(int v){
    cout << "Distancia de " << v << " ate" << endl;
    for (int i = 1; i <= vertices; i++){
        cout << i << " -> " << distancia[i] << endl;
    }
}
int main(){
    ler_grafo("teste.txt");
    imprimir_matriz();
    bfs(1);
    imprimir_distancia(1);
}