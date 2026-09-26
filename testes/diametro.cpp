#include "grafos.cpp"
#include<iostream>

int main() {
    Grafo grafo(ModoDeRepresentacao::Lista);
    grafo.ler_grafo("grafos analisados/grafo_6.txt");
    std::cout<<grafo.calcular_diametro_aproximado()<< std::endl;
    return 0;
}