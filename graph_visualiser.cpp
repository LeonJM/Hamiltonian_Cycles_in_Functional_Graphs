#include <iostream>
#include <list>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iterator>


int N = 29; // input here
std::vector<int> constants = {0,1,2,12}; // input here


class Graph {
    std::list<int> *adjList;
    std::vector<int> constants;
    int inDegreeCount;

    public:
        Graph(std::vector<int> c) {
            adjList = new std::list<int>[N];
            constants = c;
            inDegreeCount = 0;
            addEdges(constants);
            countInDegrees();
        }

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

    private:
        void addEdges(std::vector<int> constants) {
            std::cout << "Adding edges..." <<std::endl; 
            for (int a : constants) {
                for (int x = 0; x < N; x ++) {
                    int temp = pow(x, 2) + a;
                    temp = temp % N;
                    adjList[x].push_back(temp);
                }
            }
        }

        void countInDegrees() {
            std::cout << "Counting indegrees..." <<std::endl; 
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
};

int main() {

    std::cout << "N: " << N << " - ";
    for (int i : constants) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;
    Graph g(constants);
    g.printAdjList();

    return 0;
}