// g++ -o filename filename.cpp -std=c++17

#include <iostream>
#include <list>
#include <vector>
#include <cmath>
#include <ctime>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <thread>


std::string primeFile = "primes1000.txt";
std::string resultFile = "results_final-draft_V3d.csv";

std::ifstream inFile(primeFile);
std::ofstream outFile(resultFile, std::ios::out | std::ios::app);


int N; // number of vertices
std::vector<int> combination; // a combination of function's constants


class Graph {
    std::list<int> *adjList;
    std::vector<int> constants;
    int inDegreeCount;
    bool* ptr;

    public:
        Graph(std::vector<int> constants, bool* ptr) {
            adjList = new std::list<int>[N];
            this->constants = constants;
            inDegreeCount = 0;
            this->ptr = ptr;
            addEdges(constants);
            countInDegrees();
        }

        // debugging function
        void printAdjList() {
            std::cout << "Number of indegrees " << inDegreeCount << std::endl;
            for (int i = 0; i < N; i ++) {
                std::cout << "Vertex " << i << " -> ";
                for (int nbr : adjList[i]) {
                    std::cout << nbr << ", ";
                }
                std::cout << std::endl;
            }

        }

        // function that finds the first HC in graph
        void hasHC() {
            if (inDegreeCount != N) return; // there are nodes that has no indegrees

            int *path = new int[N];
            for (int i = 0; i < N; i ++) {
                path[i] = -1;
            }

            // debug
            std::cout << "Computing for HC with N=" << N << " with combination ";
            for (int i : constants) {
                std::cout << i << ",";
            }
            std::cout << std::endl;

            path[0] = 0; // start HC from vertex 0
            hasHCUtil(path, 1); // no need to start at vertex 0 - expensive process
        }

    private:
        // adds edge from x to f(x)
        void addEdges(std::vector<int> constants) {
            // std::cout << "Adding edges..." <<std::endl; // debug
            for (int a : constants) {
                for (int x = 0; x < N; x ++) {
                    int temp = pow(x, 2) + a;
                    temp = temp % N;
                    adjList[x].push_back(temp);
                }
            }
        }

        // counts number of nodes with indegrees
        void countInDegrees() {
            // std::cout << "Counting indegrees..." <<std::endl; // debug
            for (int i = 0; i < N; i ++) {
                adjList[i].remove(i); // removes loops
                for (int x = 0; x < N; x ++) {
                    if (x == i) continue;
                    bool found = (std::find(adjList[x].begin(), adjList[x].end(), i) != adjList[x].end());
                    if (found) {
                        inDegreeCount ++;
                        break;
                    }
                }
            }
        }

        // utility function - does edge path[pos-1] go to v
        bool isSafe(int v, int path[], int pos) {
            // check if last vertex added to path connects to v
            bool connect = (std::find(adjList[path[pos-1]].begin(), adjList[path[pos-1]].end(), v) != adjList[path[pos-1]].end());
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

        /** 
         * utility recursive function
         * backtracking
         * */
        bool hasHCUtil(int path[], int pos) {
            // base case
            if (pos == N) {
                // has HC
                if (std::find(adjList[path[pos-1]].begin(), adjList[path[pos-1]].end(), 0) != adjList[path[pos-1]].end()) { // is HC?
                    // debug
                    std::cout << "Found HC with N=" << N << " with combination ";
                    for (int i : constants) {
                        std::cout << i << ",";
                    }
                    std::cout << std::endl;

                    // save results to a file
                    outFile << N << ", ";
                    outFile << constants.size() << ", ";
                    for (int i : constants) {
                        outFile << i << "_";
                    }
                    outFile << ", ";
                    for (int i = 0; i < N; i ++) {
                        outFile << path[i] << " -> ";
                    }
                    outFile << 0 << "\n";
                    
                    *ptr = true; // stops the program from searching any more graphs for the current N
                    return true;
                } else {
                    return false;
                }
            }

            // recursive case
            for (int v = 1; v < N; v ++) {
                if (*ptr) return false; // may not be false but no need to keep working once a HC is found

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
};


// visual aid - debugging function
void printCombination() {
    std::cout << "N: " << N << " - ";
    for (int i : combination) {
        std::cout << i << " ,";
    }
    std::cout << std::endl;
}


/**
 * generate all combinations from 0, 1, ..., n-1 of size k
 * backtracking
 * */
void generateCombinations(int offset, int k, bool* ptr) {
    //base case
    if (k == 0) {
        // printCombination(); // debugger
        Graph g(combination, ptr);
        // g.printAdjList(); // debugger
        g.hasHC();
        // std::thread t(&Graph::hasHC, g); // multithread
        // t.detach(); // seperates the thread from main thread
        return;
    }

    // recursive case
    for (int i = offset; i <= N - k; ++ i) {
        if (*ptr) {
            combination.clear();
            break;
        }
        combination.push_back(i);
        generateCombinations(i + 1, k - 1, ptr);
        combination.pop_back();
    }
}


int main() {
    // display the start time
    std::time_t startTime = time(0);
    std::string timestamp = ctime(&startTime);
    std::cout << "Program Start at " << timestamp << std::endl;


    // source prime numbers
    std::vector<int> primes;
    std::string str;
    int prime;
    int primeLimit = 61; // max number to read in

    if (!inFile.is_open()) {
        std::cerr << "Could not open file " << primeFile << std::endl;
        return 1;
    }

    while (std::getline(inFile, str, ',')) {
        prime = stoi(str);
        if (prime > primeLimit) break;
        primes.push_back(prime);
    }

    inFile.close();


    primes = {31}; // manual setup of primes vector


    // generate combinations
    for (int n : primes) {
        N = n;
        bool goNext = false;
        bool* nxtPtr = &goNext;

        for (int k = 2; k <= N/2 + 1; k ++) { // TODO int k = 2, changing to 4 for manual test
            if (goNext) {
                break;
            }
            generateCombinations(0, k, nxtPtr);
        }
    }


    outFile.close();


    // display the run time
    std::time_t endTime = time(0);
    double runTime = endTime - startTime;
    printf("Run Time: %.3f seconds.", runTime);

    return 0;
}