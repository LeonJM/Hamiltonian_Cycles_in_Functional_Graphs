#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <ctime>
#include <cmath>


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

class Graph {
    std::list<unsigned short int> *adjList;
    bool *found;
    bool connected;

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

            for (unsigned short int k = 0; k < N; k ++) {   // k is the set size, offset starts at 1 since origin vertex is 0. Held-Karp uses subsets.
                generateVertexCombinations(1, k);   
            }

            logFile << "    Finished... " << constantsStr << "\n" << std::flush;

        }

    private:
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
                if (*found) return;     // once a HC is found, finish
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

        // save results to csv file
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

};


void generateFunctionCombinations(unsigned short int offset, unsigned short int k, bool *p) {
    // base case
    if (k == 0) {
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

    // user discrepency to type in valid input
    std::cout << "Enter N: ";
    std::cin >> N;

    // display start time
    std::time_t startTime = time(0);
    std::string timestamp = ctime(&startTime);
    logFile << "Program start at " << timestamp << "\n" << std::flush;


    bool found = false;
    functionCombination.push_back(0);   // fixed function

    for (unsigned short int k = 2; k <= N/2 + 1; k ++) {
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