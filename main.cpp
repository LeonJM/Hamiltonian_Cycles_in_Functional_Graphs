#include <iostream>
#include <list>
#include <vector>
#include <cmath>
#include <time.h>

std::vector<int> constants; // vertices 0, 1, 2, .., n-1
std::vector<int> tempConstantCombs;
std::list<std::vector<int>> constantCombs; // combinations of constants

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

void generateComb(int offset, int k) {
    //base case
    if (k == 0) {
        // pretty_print(tempConstantCombs);
        constantCombs.push_back(tempConstantCombs);
        return;
    }

    for (int i = offset; i <=  constants.size() - k; ++i) {
        tempConstantCombs.push_back(constants[i]);
        generateComb(i + 1, k - 1);
        tempConstantCombs.pop_back();
    }
}

void printCombinations() {
    for (std::vector<int> i : constantCombs) {
        for (int j : i) {
            std::cout << j << " ,";
        }
        std::cout << std::endl;
    }
}

int main() {

    clock_t startTime = clock();

    // setup
    // int n = 3;
    // for (int i = 0; i < n; i++) {
    //     constants.push_back(i);
    // }
    // for (int i = 0; i <= n; i ++) {
    //     generateComb(0, i);
    // }

    // for (std::vector<int> i : constantCombs) {
    //     Graph g(n, i);
    //     g.printAdjList();
    // }

    std::vector<int> primes = {1, 3, 5, 7};
    for (int n : primes) {
        for (int i = 0; i < n; i ++) {
            constants.push_back(i);
        }
        for (int i = 0; i <= n; i ++) {
            generateComb(0, i);
        }

        for (std::vector<int> i : constantCombs) {
            Graph g(n, i);
            std::cout << "prime field: " << n << " constants: ";
            for (int j : i) {
                std::cout << j << ", ";
            }
            std::cout << std::endl;
            g.printAdjList();
        }

        // reset global values
        constants.clear();
        constantCombs.clear();

    }

    clock_t endTime = clock();
    double runTime = double(endTime - startTime)/CLOCKS_PER_SEC;
    printf("Run Time: %.3f seconds.", runTime);

    return 0;
}