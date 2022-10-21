#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

const int N = 5;
int grid[N][N] = {
    {0, 1, 1, 0, 0},
    {0, 0, 1, 1, 0},
    {1, 1, 0, 0, 1},
    {1, 1, 0, 0, 1},
    {0, 1, 1, 1, 0},
};

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
    // init case
    if (comb.empty()) {
        for (int i = 1; i < N; i ++) {
            // memorsise i-{} = 0->i
            std::cout << i << "-{} := " << 0 << "->" << i << std::endl; 
        }
    } else {

        for (int v = 1; v < N; v ++) {
            if (std::find(comb.begin(), comb.end(), v) != comb.end()) {
                continue;
            }
            std::cout << v << "-{";
            printCombination(comb);
            std::cout << "}" << std::endl;
            for (int u = 0; u < comb.size(); u ++) {

                int tempU = comb[u];
                std::vector<int> tempComb = comb;
                tempComb.erase(tempComb.begin()+u);
                std::cout << tempU << "-{";
                printCombination(tempComb);
                std::cout << "} ->" << v << std::endl;
                // v-S = u-S* -> v


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

    // for (int k = 0; k < N-1; k ++) generateCombinations(1,k);
    std::string temp = myToString(42, {0,1,2});
    std::cout << temp << std::endl;
    DP[temp] = {0,1,2,4,3,0};
    for (int i : DP[temp]) {
        std::cout << i << ",";
    }
    std::cout << std::endl;

    return 0;
}