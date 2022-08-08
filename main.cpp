#include <iostream>
#include <list>
#include <vector>
#include <cmath>

class Graph {
    int n; // number of vertices
    std::list<int> *l; // adjacency list

    public:
        Graph(int N, std::vector<int> inputs) {
            this->n = N;
            l = new std::list<int>[N];
            this->addInputs(inputs);
        }

        void printAdjList() {
            for (int i = 0; i < n; i ++) {
                std::cout << "Vertex " << i << " -> ";
                for (int nbr : l[i]) {
                    std::cout << nbr << ", ";
                }
                std::cout << std::endl;
            }
        }

    private:
        void addEdge(int x, int y) {
            l[x].push_back(y);
        }

        /**
         * adds edge from x to f(x)
         * */
        void addInputs(std::vector<int> inputs) {
            for (int a : inputs) {
                for (int i = 0; i < n; i ++) {
                    int temp = pow(i, 2) + a;
                    temp = temp%n;
                    this->addEdge(i, temp);
                }
            }
        }

};

int main() {

    int n = 5;
    std::vector<int> inputs = {0, 1, 2};
    Graph g(n, inputs);
    g.printAdjList();

    return 0;
}