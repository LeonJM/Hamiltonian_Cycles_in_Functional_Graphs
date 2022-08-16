#include <iostream>
#include <list>
#include <vector>
#include <cmath>
#include <ctime>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>


std::string primeFile = "primes1000.txt";
std::string resultFile = "results_final-draft.csv";

int N; // number of vertices
std::vector<int> tempConstantCombs;
std::list<std::vector<int>> constantCombs; // combinations of constants

std::ofstream outFile(resultFile, std::ios::out | std::ios::app); // save results here


/**
 * main graph class
 * */
class Graph {
    int n; // number of vertices
    std::list<int> *l; // adjacency list
    std::vector<int> constants; // constants of functions 
    bool* fndPtr;

    public:
        Graph(int N, std::vector<int> inputs, bool* fndPtr) {
            this->n = N;
            this->l = new std::list<int>[N];
            this->constants = inputs;
            this->fndPtr = fndPtr;
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

        /**
         * function that finds the "first" HC in the graph
         * */
        void hasHC() {
            int *path = new int[n];
            for (int i = 0; i < n; i ++) {
                path[i] = -1;
            }

            path[0] = 0; // start HC from vertex 0
            hasHCUtil(path, 1); // no need to start at vertex 0
        }

    private:
        /**
         * adds edge from x to f(x)
         * */
        void addInputs(std::vector<int> inputs) {
            for (int a : inputs) {
                for (int i = 0; i < n; i ++) {
                    int temp = pow(i, 2) + a;
                    temp = temp%n;
                    l[i].push_back(temp);
                }
            }
        }

        // utility recursive function
        bool hasHCUtil(int path[], int pos) {
            // base case
            if (pos == n) {
                // has HC
                if (std::find(l[path[pos-1]].begin(), l[path[pos-1]].end(), 0) != l[path[pos-1]].end()) { // is HC?
                    // save results to a file
                    outFile << n << ", ";
                    outFile << constants.size() << ", ";
                    for (int i : constants) {
                        outFile << i << "_";
                    }
                    outFile << ", ";
                    for (int i = 0; i < n; i ++) {
                        outFile << path[i] << " -> ";
                    }
                    outFile << 0 << "\n";
                    *fndPtr = true;

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
 * generate all combinations from 0, 1, ..., n-1 of size k
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
    std::cout << "Program Start at " << time(NULL) << std::endl;
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
        if (prime > 19) { // reads up to that prime number
            break;
        }
        primes.push_back(prime);
    }
    inFile.close();

    primes = {29}; // manual field - comment out when not in use
    for (int n : primes) {
        // setup
        N = n;  // set global variable
        bool found = false;
        bool* fndPtr = &found;
        for (int i = 2; i <= n/2 + 1; i ++) { // range is because of Ghouila-Houri's theorem
            if (found) {
                break;
            }

            generateComb(0, i); // O(nCi)
            // printCombinations();

            // graph generation
            for (std::vector<int> c : constantCombs) {
                if (found) {
                    break;
                }
                
                Graph g(n, c, fndPtr);
                g.hasHC(); // NP-complete

                // std::cout << "prime field: " << n << " constants: ";
                // for (int j : c) {
                //     std::cout << j << ", ";
                // }
                // std::cout << std::endl;
                // g.printAdjList();
                // std::cout << std::endl;

            }

            constantCombs.clear(); // reset global variable
        }


        // graph generation
        // for (std::vector<int> i : constantCombs) {
        //     if (found) {
        //         break; // only want to find the first HC possible for the prime field
        //     }
        //     Graph g(n, i, fndPtr);

        //     // std::cout << "prime field: " << n << " constants: ";
        //     // for (int j : i) {
        //     //     std::cout << j << ", ";
        //     // }
        //     // std::cout << std::endl;
        //     // g.printAdjList();

        //     g.hasHC();
        //     // std::cout << std::endl;
        // }

        // reset global values
        // constantCombs.clear();
    }
    
    outFile.close();

    clock_t endTime = clock();
    double runTime = double(endTime - startTime)/CLOCKS_PER_SEC;
    printf("Run Time: %.3f seconds.", runTime);

    return 0;
}