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
#include <mutex>


std::string primeFile = "primes1000.txt";
std::string resultFile = "results_final-draft_V3d.csv";

std::ifstream inFile(primeFile);
std::ofstream outFile(resultFile, std::ios::out | std::ios::app);
std::ofstream logFile("log.csv",  std::ios::out | std::ios::app);


int N; // number of vertices
std::vector<int> combination; // a combination of function's constants
std::vector<std::thread> threads;
std::mutex foundLock;


// visual aid - debugging function
void printCombination() {
    std::cout << "N: " << N << " - ";
    for (int i : combination) {
        std::cout << i << " ,";
    }
    std::cout << std::endl;
}


class Graph {
    std::list<int> *adjList;
    std::vector<int> constants;
    int inDegreeCount;
    bool* ptr;

    std::string constantsStr = "";  // logger

    public:
        Graph(std::vector<int> constants, bool* ptr) {
            adjList = new std::list<int>[N];
            this->constants = constants;
            inDegreeCount = 0;
            this->ptr = ptr;
            addEdges(constants);
            countInDegrees();

            // logger
            for (int i : constants) {
                constantsStr += std::to_string(i);
                constantsStr += ", ";
            }
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

            if (!isConnected()) return; // graph is not connected

            int *path = new int[N];
            for (int i = 0; i < N; i ++) {
                path[i] = -1;
            }

            // logger
            std::string startingStr = "Starting... " + constantsStr;
            logFile << startingStr << "\n" << std::flush;

            path[0] = 0; // start HC from vertex 0
            hasHCUtil(path, 1); // no need to start at vertex 0 - expensive process

            // logger
            std::string finishedStr = "    Finished... " + constantsStr;
            logFile << finishedStr << "\n" << std::flush;
        }

    private:
        // adds edge from x to f(x)
        void addEdges(std::vector<int> constants) {
            // std::cout << "Adding edges..." <<std::endl; // debugger
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
            // std::cout << "Counting indegrees..." <<std::endl; // debugger
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
                if (std::find(adjList[path[pos-1]].begin(), adjList[path[pos-1]].end(), 0) != adjList[path[pos-1]].end()) { // checks if path tail connects to path head
                    foundLock.lock();
                    if (*ptr) {
                        foundLock.unlock();
                        return true;
                    }

                    // logger
                    std::string foundStr = "        FOUND " + constantsStr;
                    logFile << foundStr << "\n" << std::flush;

                    *ptr = true; // found a HC for current N
                    writeToOutFile(path);

                    foundLock.unlock();
                    return true;
                } else {
                    return false;
                }
            }

            // recursive case
            for (int v = 1; v < N; v ++) {
                if (*ptr) return true; // no need to keep working once a HC is found

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

        // save HC sequence to outFile
        void writeToOutFile(int path[]) {
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
        }


        // utility function
        // TODO: may be redundant
        void traverse(int u, bool visited[]) {
            visited[u] = true;
            for (int v : adjList[u]) {
                if (!visited[v]) traverse(v, visited);
            }
        }


        // graph connectedness
        // TODO: may be redundant
        bool isConnected() {
            bool *visited = new bool[N];
            for (int u = 0; u < N; u ++) {
                for (int i = 0; i < N; i ++) {
                    visited[i] = false;
                }
                traverse(u, visited); // dfs
                for (int i = 0; i < N; i ++) {
                    if (!visited[i]) return false; // the graph is not connected
                }
            }
            return true;
        }
};


/**
 * generate all combinations from 0, 1, ..., n-1 of size k
 * backtracking
 * */
void generateCombinations(int offset, int k, bool* ptr) {
    //base case
    if (k == 0) {
        Graph g(combination, ptr);
        threads.push_back(std::thread(&Graph::hasHC, g));
        return;
    }

    // recursive case
    for (int i = offset; i <= N - k; ++ i) {
        if (*ptr) {
            combination.clear();
            combination.push_back(0); // permanent constant in combination
            break;
        }
        combination.push_back(i);
        generateCombinations(i + 1, k - 1, ptr);
        combination.pop_back();
    }
}


int main() {
    // user inputs
    std::string primeInput;
    std::cout << "Enter prime: ";
    std::cin >> primeInput;
    int kInput;
    std::cout << "Enter initial k: ";
    std::cin >> kInput;

    int switchCase = 0;
    if (primeInput[0] == '-') {
        switchCase = 1;
        primeInput.erase(0,1);
    } else {
        switchCase = 2;
    }


    // display the start time
    std::time_t startTime = time(0);
    std::string timestamp = ctime(&startTime);
    logFile << "Program Start at " << timestamp << "\n" << std::flush; // logger


    // source prime numbers
    std::vector<int> primes;
    std::string str;
    int prime;
    int primeLimit = stoi(primeInput); // max number to read in

    if (!inFile.is_open()) {
        std::cerr << "Could not open file " << primeFile << std::endl;
        return 1;
    }

    switch (switchCase) {
        case 1:
            while (std::getline(inFile, str, ',')) {
                prime = stoi(str);
                if (prime > primeLimit) break;
                primes.push_back(prime);
            }

            break;

        case 2:
            primes = {stoi(primeInput)}; // manual setup of primes vector 
            break;

        default:
            std::cout << "Premature end" << std::endl;
            return 0;
    }


    inFile.close();


    combination.push_back(0); // the permanent function's constant


    // generate combinations
    for (int n : primes) {
        N = n;
        bool goNext = false;
        bool* nxtPtr = &goNext;

        for (int k = kInput; k <= N/2 + 1; k ++) { 
            if (goNext) {
                break;
            }

            logFile << "Starting N=" << n << " with k=" << k << "\n" << std::flush; // logger
            
            generateCombinations(1, k-1, nxtPtr); // offset should be at 1 as 0 should be permanent

            for (auto &t : threads) {
                t.join();
            }
            threads.clear();
        }
    }


    // display the run time
    std::time_t endTime = time(0);
    double runTime = endTime - startTime;
    logFile << "Run Time: " << runTime << " seconds\n" << std::flush; // logger


    outFile.close();
    logFile.close();


    return 0;
}