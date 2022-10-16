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
std::string resultFile = "results_final-draft_V4.csv";

std::ifstream inFile(primeFile);
std::ofstream outFile(resultFile, std::ios::out | std::ios::app);
std::ofstream logFile("log.csv",  std::ios::out | std::ios::app);


unsigned short int N; // number of vertices - current prime field
std::mutex foundLock;
std::vector<unsigned short int> combination;
std::vector<std::thread> threads;


class Graph {
    std::list<unsigned short int> *adjList;
    std::vector<unsigned short int> constants;
    bool *ptr;
    bool connected;

    std::string constantsStr = ""; // logger
    
    public:
        // constructor
        Graph(std::vector<unsigned short int> c, bool *p) {
            // setup
            this->adjList = new std::list<unsigned short int>[N];
            this->constants = c;
            this->ptr = p;
            // graph creation
            this->addEdges(this->constants);
            this->connected = this->isConnected();
        }

        // destructor - only use when single threading
        // ~Graph() {
        //     this->clean();
        // }

        // connected getter
        bool getConnected() {
            if (!this->connected) {
                this->clean();
                return false;
            }
            return true;
        }

        // HC computation
        void hasHC() {
            short int *path = new short int[N];
            for (unsigned short int i = 0; i < N; i ++) {
                path[i] = -1;
            }

            logFile << "Starting... " << constantsStr << "\n" << std::flush;

            // std::time_t tTimeStart = time(0);
            
            path[0] = 0;
            this->hasHCUtil(path, 1);
            delete path;
            this->clean();

            logFile << "    Finished... " << constantsStr << "\n" << std::flush;
        }

    private:
        // pseudo-destructor
        void clean() {
            delete[] this->adjList;
            std::vector<unsigned short int>().swap(this->constants);
        }

        // adds edge from x to f(x)
        void addEdges(std::vector<unsigned short int> c) {
            for (unsigned short int a : c) {
                constantsStr += std::to_string(a) + ", ";
                for (unsigned short int x = 0; x < N; x ++) {
                    unsigned int temp = pow(x, 2) + a;
                    temp = temp % N;
                    adjList[x].push_back(temp);
                }
            }
        }

        // returns graph connectedness
        bool isConnected() {
            bool *visited = new bool[N];
            for (unsigned short int u = 0; u < N; u ++) {
                for (unsigned short int i = 0; i < N; i ++) {
                    visited[i] = false;
                }
                traverse(u, visited);
                for (unsigned short int i = 0; i < N; i ++) {
                    if (!visited[i]) return false;
                }
            }
            delete visited;
            return true;
        }

        // dfs traversal
        void traverse(unsigned short int u, bool visited[]) {
            visited[u] = true;
            for (unsigned short int v : adjList[u]) {
                if (!visited[v]) traverse(v, visited);
            }
        }

        // HC computation
        bool hasHCUtil(short int path[], unsigned short int pos) {
            // base case
            if (pos == N) {
                if (std::find(adjList[path[pos-1]].begin(), adjList[path[pos-1]].end(), 0)
                    != adjList[path[pos-1]].end()) {
                        foundLock.lock();
                        if (*ptr) { // in case of HC found at the same step
                            foundLock.unlock();
                            return true;
                        }

                        logFile << "~~~~~~~~~~~~~~ FOUND " << constantsStr << "\n" << std::flush;

                        *ptr = true;
                        saveSolution(path);

                        foundLock.unlock();
                        return true;
                } else {
                    return false;
                }
            }

            // recursive case
            for (unsigned short int v = 0; v < N; v ++) {
                if (*ptr) return true;

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

        // check edge connections for hasHCUtil
        bool isSafe(unsigned short int v, short int path[], unsigned short int pos) {
            // check if last vertex added to path connects to v
            bool connect = (std::find(adjList[path[pos-1]].begin(), adjList[path[pos-1]].end(), v) 
                != adjList[path[pos-1]].end());

            if (!connect) {
                return false;
            }

            // check if v has already been included in path
            for (unsigned short int i = 0; i < pos; i ++) {
                if (path[i] == v) {
                    return false;
                }
            }

            return true;
        }

        void saveSolution(short int path[]) {
            outFile << N << ", ";
            outFile << constants.size() << ", ";
            for (unsigned short int i : constants) {
                outFile << i << "_";
            }
            outFile << ", ";
            for (unsigned short int i = 0; i < N; i ++) {
                outFile << path[i] << " -> ";
            }
            outFile << 0 << std::endl;
        }

};

void generateCombinations(unsigned short int offset, unsigned short int k, bool *p) {
    // base case
    if (k == 0) {
        Graph g(combination, p);
        if (!g.getConnected()) return;
        threads.push_back(std::thread(&Graph::hasHC, g));
        // g.hasHC();
        return;
    }

    // recursive case
    for (unsigned short int i = offset; i <= N - k; ++ i) {
        if (*p) {
            combination.clear();
            combination.push_back(0); // fixed value in combination
            break;
        }
        combination.push_back(i);
        generateCombinations(i + 1, k - 1, p);
        combination.pop_back();
    }
}

int main() {
    // user inputs
    std::string primeInput;
    unsigned short int initK;
    std::cout << "Enter prime (e.g. 41) or for 2 to prime, prefix \"-\": ";
    std:: cin >> primeInput;
    std::cout << "Enter initial k: ";
    std::cin >> initK;

    char switchCase;
    if (primeInput[0] == '-') {
        switchCase = 'a';
        primeInput.erase(0,1);
    } else {
        switchCase = 'b';
    }

    // display start time
    std::time_t startTime = time(0);
    std::string timestamp = ctime(&startTime);
    logFile << "Program Start at " << timestamp << "\n" << std::flush;

    // prime numbers
    std::vector<unsigned short int> primes;
    std::string str;
    unsigned short int prime;
    unsigned short int primeLimit = std::stoi(primeInput);

    if (!inFile.is_open()) {
        std::cerr << "Could not open file " << primeFile << std::endl;
        return 1;
    }

    switch(switchCase) {
        case 'a':
            while (std::getline(inFile, str, ',')) {
                prime = std::stoi(str);
                if (prime > primeLimit) break;
                primes.push_back(prime);
            }
            break;

        case 'b':
            primes = {(unsigned short) std::stoi(primeInput)};
            break;
    }

    inFile.close();

    // combinations
    combination.push_back(0); // fixed value in combination

    // graph generation
    bool goNext = false;
    
    for (unsigned short int n : primes) {
        N = n;
        goNext = false;

        for (unsigned short int k = initK; k <= N/2 + 1; k ++) {
            if (goNext) {
                break;
            }

            logFile << "Starting N=" << n << " with k=" << k << "\n" << std::flush; 

            generateCombinations(1, k - 1, &goNext); // offset is 1 as 0 is a fixed value

            for (auto &t : threads) {
                t.join();
            }
            threads.clear();
        }
    }

    // display run time
    std::time_t endTime = time(0);
    double runTime = endTime - startTime;
    logFile << "Run Time: " << runTime << " seconds\n\n" << std::flush;


    outFile.close();
    logFile.close();

    return 0;
}