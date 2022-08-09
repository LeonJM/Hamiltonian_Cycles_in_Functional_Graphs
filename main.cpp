#include <iostream>
#include <list>
#include <vector>
#include <cmath>
#include <time.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>

std::string primeFile = "primes1000.txt";

int N; // number of vertices
std::vector<int> tempConstantCombs;
std::list<std::vector<int>> constantCombs; // combinations of constants

/**
 * main graph class
 * */
class Graph {
    int n; // number of vertices
    std::list<int> *l; // adjacency list

    public:
        Graph(int N, std::vector<int> inputs) {
            this->n = N;
            this->l = new std::list<int>[N];
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

        void hasHC() {
            int *path = new int[n];
            for (int i = 0; i < n; i ++) {
                path[i] = -1;
            }

            path[0] = 0; // start HC from vertex 0
            hasHCUtil(path, 1); // no need to start at vertex 0
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

        // utility recursive function
        bool hasHCUtil(int path[], int pos) {
            // base case
            if (pos == n) {
                if (std::find(l[path[pos-1]].begin(), l[path[pos-1]].end(), 0) != l[path[pos-1]].end()) { // is HC?
                    // has HC
                    std::cout << "HamCycle: ";
                    for (int i = 0; i < n; i ++) {
                        std::cout << path[i] << ", ";
                    }
                    std::cout << "0" << std::endl;
                    return true;
                } else {
                    return false;
                }
            }

            for (int v = 1; v < n; v ++) {
                if (isSafe(v, path, pos)) {
                    path[pos] = v;
                    if (hasHCUtil(path, pos + 1)) {
                        return true;
                    }
                    path[pos] = -1;
                }
            }

            return false;
        }

        // utility function - does edge path[pos-1] go to v
        bool isSafe(int v, int path[], int pos) {
            // check if last vertex added to path connects to v
            bool connect = (std::find(l[path[pos-1]].begin(), l[path[pos-1]].end(), v) != l[path[pos-1]].end());
            if (!connect) {
                return false; // last vertex added to path has no edge to v
            }

            // check if v has already been included in path
            for (int i = 0; i < pos; i ++) {
                if (path[i] == v) {
                    return false;
                }
            }

            return true;
        }

};

/**
 * generate all combinations up to n
 * */
void generateComb(int offset, int k) {
    // base case
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

    // open file with list of prime numbers
    std::ifstream inFile(primeFile);
    if (!inFile.is_open()) {
        std::cerr << "Could not open file " << primeFile << std::endl;
        return 1;
    }

    // prepare a vector of prime numbers
    std::vector<int> primes; 
    std::string str;
    int prime;
    while (getline(inFile, str, ',')) {
        prime = stoi(str);
        if (prime > 5) { // reads up to that prime number
            break;
        }
        primes.push_back(prime);
    }
    inFile.close();

    primes = {11}; // manual field - comment out when not in use
    for (int n : primes) {
        // setup
        N = n;
        for (int i = 0; i <= n; i ++) {
            generateComb(0, i);
        }

        printCombinations();

        // graph generation
        for (std::vector<int> i : constantCombs) {
            Graph g(n, i);

            std::cout << "prime field: " << n << " constants: ";
            for (int j : i) {
                std::cout << j << ", ";
            }
            std::cout << std::endl;

            g.printAdjList();
            g.hasHC();
            std::cout << std::endl;
        }

        // reset global values
        constantCombs.clear();

    }

    clock_t endTime = clock();
    double runTime = double(endTime - startTime)/CLOCKS_PER_SEC;
    printf("Run Time: %.3f seconds.", runTime);

    return 0;
}