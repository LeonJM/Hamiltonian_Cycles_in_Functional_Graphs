// g++ -o filename filename.cpp -std=c++17

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <ctime>
#include <cmath>
#include <thread>


std::ofstream logFile("log.csv", std::ios::out | std::ios::app);
std::ofstream outFile("resultsDP_V1.csv", std::ios::out | std::ios::app);


unsigned short int N;
std::vector<unsigned short int> functionCombination;


// helper function to show combinations
void printCombination(std::vector<unsigned short int> c) {
    for (int i : c) {
        std::cout << i << ", ";
    }
}

// helper function
bool checkCombinationSequential(std::vector<unsigned short int> c) {
    if (c.size() >= 3 && c[c.size()-1]/2 < c[c.size()-2])  {    // last constant should be bigger {0, a, b, 2b}
        return true;     
    }
    for (unsigned short int i = 0; i < c.size() - 1; i ++) {    // sequence should not be sequential {0, 1, 2, 3}
        if (c[i+1] - c[i] != 1) {
            return false;
        }
    }


    return true;
}

class Graph {
    std::list<unsigned short int> *adjList;
    bool *found;
    bool connected;
    bool searched;

    std::vector<unsigned short int> vertexCombination;
    std::unordered_map<std::string, std::vector<unsigned short int>> DP;

    std::string constantsStr = "";  // logger
    unsigned short int k = 0; // number of functions

    public:
        // constructor
        Graph(std::vector<unsigned short int> c, bool *p) {
            adjList = new std::list<unsigned short int>[N];
            found = p;
            addEdges(c);
            connected = isConnected();
            searched = false;
        }

        // destructor
        ~Graph() {
            delete[] adjList;
        }

        // getter
        bool getConnected() {
            return connected;
        }

        // HC computation
        void computeHC() {
            logFile << "Starting... " << constantsStr << "\n" << std::flush;
            // logFile << "combination sequential: " << checkCombinationSequential(functionCombination) << "\n" << std::flush;    // debug
            std::vector<std::thread> threads;

            // Dynamic Programming approach through Held-Karp
            threads.push_back(std::thread(&Graph::startDP, this));

            // Backtracking approach
            threads.push_back(std::thread(&Graph::startBacktracking, this));

            // Wait for the threads to finish
            for (auto &t : threads) {
                t.join();
            }

            logFile << "    Finished... " << constantsStr << "\n" << std::flush;

        }

    private:
        void startDP() {
            std::cout << "Starting Held-Karp" << std::endl;     // debug
                for (unsigned short int k = 0; k < N; k ++) {   // k is the set size, offset starts at 1 since origin vertex is 0. Held-Karp uses subsets.
                generateVertexCombinations(1, k);   
            }
            searched = true;
        }

        void startBacktracking() {
            std::cout << "Starting Backtracking" << std::endl;     // debug 
            unsigned short int *path = new unsigned short int[N];  
            for (unsigned short int i = 0; i < N; i ++) {
                path[i] = N;    // placeholder, no sequence should have vertex N
            }
            path[0] = 0; // starting vertex
            backtracking(path, 1);
            searched = true;
            delete path;
        }

        // add edge from x to f(x)
        void addEdges(std::vector<unsigned short int> c) {
            for (unsigned short int a : c) {
                constantsStr += std::to_string(a) + ", ";
                k ++;
                for (unsigned short int x = 0; x < N; x ++) {
                    unsigned int temp = pow(x,2) + a;
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

        // helper to make keys
        std::string myToString(unsigned short int v, std::vector<unsigned short int> c) {
            std::string result = "";

            result += std::to_string(v);
            result += "-{";
            for (unsigned short int i : c) result += std::to_string(i) + ",";
            result += "}";

            return result;
        }

        // vertex combinations
        void generateVertexCombinations(unsigned short int offset, unsigned short int k) {
            // base case
            if (k == 0) {
                HeldKarp(vertexCombination);
                return;
            }

            // recursive case
            for (unsigned short int i = offset; i <= N - k; ++i) {
                if (*found || searched) return;     // once a HC is found, finish
                vertexCombination.push_back(i);
                generateVertexCombinations(i + 1, k - 1);
                vertexCombination.pop_back();
            }
        }

        // DP algorithm
        void HeldKarp(std::vector<unsigned short int> c) {
            if (c.empty()) {    // initial case
                for(unsigned short int i = 1; i < N; i++) {     // v=0 is origin
                    std::string key = myToString(i, {});
                    if (std::find(adjList[0].begin(), adjList[0].end(), i) != adjList[0].end()) {
                        DP[key] = {0,i};
                    } else {
                        DP[key] = {};
                    }
                    // std::cout << key << ": "; printCombination(DP[key]); std::cout << std::endl; // debug
                }

            } else if (c.size() == N - 1) {     // result
                for (unsigned short int u = 0; u < c.size(); u ++) {
                    unsigned short int tempU = c[u];
                    std::vector<unsigned short int> tempC = c;
                    tempC.erase(tempC.begin()+u);
                    std::string subProblem = myToString(tempU, tempC);

                    if (DP[subProblem].empty() || std::find(adjList[tempU].begin(), adjList[tempU].end(), 0) == adjList[tempU].end()) {     // invalid
                        continue;
                    } else {
                        std::vector<unsigned short int> path;
                        *found = true;
                        std::cout << "FOUND w/ Held-Karp" << std::endl;     // debug
                        path = DP[subProblem];
                        path.push_back(0);
                        logFile << "~~~~~~~~~~~~~~ FOUND " << constantsStr << "\n" << std::flush;
                        saveResults(path);
                        // std::cout << "result: "; printCombination(path); std::cout << std::endl; // debug
                        return;
                    }
                }

            } else {    // build the DP table
                for (unsigned short int v = 1; v < N; v ++) {
                    if (std::find(c.begin(), c.end(), v) != c.end()) {     // if v in c skip
                    continue;
                    }
                    std::string key = myToString(v, c);
                    bool curFound = false;

                    // std::cout << key << ": "; // debug
                    
                    for (unsigned short int u = 0; u < c.size() && !curFound; u ++) {
                        unsigned short int tempU = c[u];
                        std::vector<unsigned short int> tempC = c;
                        tempC.erase(tempC.begin()+u);

                        // memorise and check DP table
                        std::string subProblem = myToString(tempU, tempC);
                        if (DP[subProblem].empty() || std::find(adjList[tempU].begin(), adjList[tempU].end(), v) == adjList[tempU].end()) {     // invalid
                            DP[key] = {};
                        } else {
                            curFound = true;
                            DP[key] = DP[subProblem];
                            DP[key].push_back(v);
                        }

                        // printCombination(DP[key]); std::cout << std::endl; // debug
                    }
                }
            }
        }

        // Backtracking algorithm
        bool backtracking(unsigned short int path[], unsigned short int pos) {
            // base case
            if (pos == N) {
                if (std::find(adjList[path[pos-1]].begin(), adjList[path[pos-1]].end(), 0) != adjList[path[pos-1]].end()) {
                    
                    *found = true;
                    std::cout << "FOUND w/ Backtracking" << std::endl;     // debug

                    logFile << "~~~~~~~~~~~~~~ FOUND " << constantsStr << "\n" << std::flush;

                    saveResults(path);

                    return true;
                } else {
                    return false;
                }
            }

            // recursive case
            for (unsigned short int v = 0; v < N; v ++) {
                if (*found || searched) return true;

                if (isSafe(v, path, pos)) {
                    path[pos] = v;
                    if (backtracking(path, pos + 1)) {
                        return true;
                    }
                    path[pos] = N;
                }
            }

            return false;
        }

        // check edge connections for backtracking()
        bool isSafe(unsigned short int v, unsigned short int path[], unsigned short int pos) {
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

        // save results to csv file - vector argument
        void saveResults(std::vector<unsigned short int> path) {
            outFile << "N=" << N << " - ";
            outFile << "k=" << k << " - ";
            outFile << "functions=" << constantsStr << "- ";
            outFile << "path= ";
            for (unsigned short int i : path) {
                outFile << i << " -> ";
            }
            outFile << "\n" << std::flush;
        }

        // save results to csv file - array
        void saveResults(unsigned short int path[]) {
            outFile << "N=" << N << " - ";
            outFile << "k=" << k << " - ";
            outFile << "functions=" << constantsStr << "- ";
            outFile << "path= ";
            for (unsigned short int i = 0; i < N; i ++) {
                outFile << path[i] << " -> ";
            }
            outFile << "0\n" << std::flush;
        }

};


void generateFunctionCombinations(unsigned short int offset, unsigned short int k, bool *p) {
    // base case
    if (k == 0) {
        if (checkCombinationSequential(functionCombination)) return;    // skip sequential functions
        Graph g(functionCombination, p);
        if (!g.getConnected()) return;  // skip disconnected graphs
        // printCombination(functionCombination); std::cout << std::endl; // debug
        g.computeHC();
        return;
    }

    // recursive case
    for (unsigned short int i = offset; i <= N - k; ++ i) {
        if (*p) {
            functionCombination.clear();
            functionCombination.push_back(0); // fixed value in combination
            return;
        }
        functionCombination.push_back(i);
        generateFunctionCombinations(i + 1, k - 1, p);
        functionCombination.pop_back();
    }
}


int main() {

    // user discrepancy to type in valid input
    std::cout << "Enter N: ";
    std::cin >> N;
    unsigned short int kInput;
    std::cout << "Enter k: ";
    std::cin >> kInput;

    // display start time
    std::time_t startTime = time(0);
    std::string timestamp = ctime(&startTime);
    logFile << "Program start at " << timestamp << "\n" << std::flush;


    bool found = false;
    functionCombination.push_back(0);   // fixed function

    for (unsigned short int k = kInput; k <= N/2 + 1; k ++) {
        if (found) {
            break;
        }

        logFile << "Starting N=" << N << " with k=" << k << "\n" << std::flush; 

        generateFunctionCombinations(1, k - 1, &found); // offset is 1 as 0 is a fixed value

    }

    // display run time
    std::time_t endTime = time(0);
    double runTime = endTime - startTime;
    logFile << "Run time: " << runTime << " seconds\n\n" << std::flush;

    // close files
    outFile.close();
    logFile.close();

    return 0;
}