#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <list>

const int N = 7;
int grid[N][N] = {
    {0, 1, 0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0, 0, 1},
    {0, 0, 0, 0, 1, 1, 0},
    {1, 0, 1, 0, 0, 0, 0},
    {1, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 0},
    {0, 1, 1, 0, 0, 0, 0},
};
std::list<int> *adjList; // todo

std::unordered_map<std::string, std::vector<int>> DP;

std::vector<int> combination;

void printCombination(std::vector<int> c) {
    for (int i : c) {
        std::cout << i << ", ";
    }
}

std::string myToString(int v, std::vector<int> c) {
    std::string result = "";

    result += std::to_string(v);
    result += "-{";
    for (int i : c) result += std::to_string(i) + ",";
    result += "}";

    return result;
}

void HeldKarp(std::vector<int> comb) {

    if (comb.empty()) {     // init case

        for (int i = 1; i < N; i ++) {
            std::string key = myToString(i, {});
            // memorise
            if (grid[0][i]) {    // adjacency matrix
            // if (std::find(adjList[0].begin(), adjList[0].end(), i) != adjList[0].end()) {   // adjacency list
                DP[key] = {0,i};
            } else {
                DP[key] = {-1};
            }
            std::cout << i << "-{} := "; printCombination(DP[key]); std::cout << std::endl;     // debug
        }

    } else if (comb.size() == N-1) {  // algorithm output

        std::vector<int> path;
        for (int u = 0; u < comb.size(); u ++) {
                int tempU = comb[u];
                std::vector<int> tempComb = comb;
                tempComb.erase(tempComb.begin()+u);
                std::string subProblem = myToString(tempU, tempComb);

                if (DP[subProblem].size() == 1 || grid[tempU][0] == 0) {    // invalid
                // if (DP[subProblem].size() == 1 || std::find(adjList[tempU].begin(), adjList[tempU].end(), 0) != adjList[tempU].end()) {     // invalid // adjacency list
                    continue;
                } else {
                    path = DP[subProblem];
                    path.push_back(0);
                    printCombination(path);     // debug
                    // save results
                    return;
                }
            }

    } else {    // build the DP table

        for (int v = 1; v < N; v ++) {
            if (std::find(comb.begin(), comb.end(), v) != comb.end()) {     // if v in subset skip
                continue;
            }
            std::string key = myToString(v, comb);
            bool foundPath = false;
            std::cout << v << "-{"; printCombination(comb); std::cout << "}" << std::endl;      // debug
            for (int u = 0; u < comb.size() && !foundPath; u ++) {
                int tempU = comb[u];
                std::vector<int> tempComb = comb;
                tempComb.erase(tempComb.begin()+u);

                // memorise
                std::string subProblem = myToString(tempU, tempComb);
                if (DP[subProblem].size() == 1 || grid[tempU][v] == 0) {    // invalid
                // if (DP[subProblem].size() == 1 || std::find(adjList[tempU].begin(), adjList[tempU].end(), v) != adjList[tempU].end()) {    // invalid // adjacency list
                    DP[key] = {-1};
                } else {
                    foundPath = true;
                    DP[key] = DP[subProblem];
                    DP[key].push_back(v);
                }

            printCombination(DP[key]); std::cout << std::endl;      // debug

            }
            std::cout << std::endl;
        }
    }

}

void generateCombinations(int offset, int k) {
    // base case
    if (k == 0) {
        // printCombination(combination);
        HeldKarp(combination);
        std::cout << std::endl;
        return;
    }

    // recursive case
    for (int i = offset; i <= N - k; ++ i) {
        combination.push_back(i);
        generateCombinations(i + 1, k - 1);
        combination.pop_back();
    }
}


int main() {

    for (int k = 0; k < N; k ++) generateCombinations(1,k);
    

    return 0;
}