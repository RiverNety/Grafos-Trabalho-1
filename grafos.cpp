#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>

using namespace std;

vector<vector<int>> vetor_adj;
vector<vector<int>> matriz_adj;
int vertices;

void ler_grafo(string arquivo){
    ifstream grafo_txt(arquivo);
    string linha;

    getline(grafo_txt, linha);

    //ajuste de tamanho
    vertices = stoi(linha);
    matriz_adj.resize(vertices + 1, vector<int>(vertices + 1, 0));
    vetor_adj.resize(vertices + 1);
    
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

int main(){
    ler_grafo("teste.txt");
}