#include "grafos.cpp"
#include <iostream>
#include <string>

int main() {
    Grafo grafo(ModoDeRepresentacao::Lista);
    grafo.ler_grafo("grafos analisados/grafo_6.txt");
    std::string s;
    std::cin >> s;
    std::cout << "Feito";
    return 0;
}