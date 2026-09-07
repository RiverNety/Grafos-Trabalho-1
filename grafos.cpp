#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>

using namespace std;

vector<vector<int>> vetor_adj;
vector<vector<int>> matriz_adj;
int vertices;
vector<bool> visitados;


void ler_grafo(string arquivo){
    ifstream grafo_txt(arquivo);
    string linha;

    getline(grafo_txt, linha);

    //ajuste de tamanho
    vertices = stoi(linha);
    matriz_adj.resize(vertices + 1, vector<int>(vertices + 1, 0));
    vetor_adj.resize(vertices + 1);
    visitados.resize(vertices + 1, false)
    
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

int main(){
    ler_grafo("teste.txt");
    imprimir_matriz();
}