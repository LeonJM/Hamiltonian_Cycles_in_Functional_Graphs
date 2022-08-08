#include <iostream>
#include <list>
#include <vector>
#include <cmath>
#include <time.h>
#include <fstream>
#include <string>

std::string primeFile = "primes1000.txt";

int N; // number of vertices.
std::vector<int> tempConstantCombs;
std::list<std::vector<int>> constantCombs; // combinations of constants.

/**
 * The mani graph class.
 * */
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

/**
 * Generate all combinations up to n.
 * */
void generateComb(int offset, int k) {
    //base case
    if (k == 0) {
        constantCombs.push_back(tempConstantCombs);
        return;
    }

    for (int i = offset; i <=  N - k; ++i) {
        tempConstantCombs.push_back(i);
        generateComb(i + 1, k - 1);
        tempConstantCombs.pop_back();
    }
}

// visual aid - debugging function
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

    std::ifstream inFile(primeFile);
    if (!inFile.is_open()) {
        std::cerr << "Could not open file " << primeFile << std::endl;
        return 1;
    }

    std::vector<int> primes; 
    std::string str;
    int prime;
    while (getline(inFile, str, ',')) {
        prime = stoi(str);
        if (prime > 19) { // reads up to that prime number.
            break;
        }
        primes.push_back(prime);
    }
    inFile.close();

    // std::vector<int> primes = {5};
    for (int n : primes) {
        // setup
        N = n;
        for (int i = 0; i <= n; i ++) {
            generateComb(0, i);
        }

        printCombinations();

        // graph generation
        // for (std::vector<int> i : constantCombs) {
        //     Graph g(n, i);

        //     // std::cout << "prime field: " << n << " constants: ";
        //     // for (int j : i) {
        //     //     std::cout << j << ", ";
        //     // }
        //     // std::cout << std::endl;

        //     // g.printAdjList();
        // }

        // reset global values
        constantCombs.clear();

    }

    clock_t endTime = clock();
    double runTime = double(endTime - startTime)/CLOCKS_PER_SEC;
    printf("Run Time: %.3f seconds.", runTime);

    return 0;
}