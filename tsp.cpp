#include <iostream>
#include <stdlib.h>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <climits>
#include <fstream>
#include <chrono>
#include <string>
using namespace std::chrono;
using namespace std;

vector<vector<double>> graph;          // adjecency matrix of cities : graph[i][j] represents edge cost between i and j
unordered_set<int> visited;
vector<int> currPath, shortestPath;         // data structure to store the nodes visited in current path during dfs
double upperBound = INT_MAX;          // upper bound of path cost used to prune the path
int start_node = 0, use_bound, numPaths = 0;                 // node from which traversal is started

void printShortestPath() {
    cout << "Path : ";
    for (auto node : shortestPath)
        cout << node << " " ;
    cout << endl;
}

void branch_bound(int curr_vertex, double cost)
{
    if(visited.size()==graph.size() && curr_vertex==start_node) {
        if (cost < upperBound) {
            upperBound = cost;
            shortestPath = currPath;
        }
        numPaths++;
        //printCurrPath();
        //cout << "Total cost : " << cost << endl;
    }
    if (visited.find(curr_vertex)!=visited.end() || (use_bound && cost>=upperBound))
        return;
    visited.insert(curr_vertex);
    currPath.push_back(curr_vertex);
    for(int i=0; i<graph[curr_vertex].size(); i++) {
        if (graph[curr_vertex][i]!=INT_MAX &&  curr_vertex!=i)
            branch_bound(i, cost + graph[curr_vertex][i]);
    }
    currPath.pop_back();
    visited.erase(curr_vertex);
}


int calculate_upperBound() {
    return INT_MAX;
}

vector<double> getDistances(string& line) {
    int i=0;
    vector<double> ret;
    //cout << line.size() << endl;
    while(i<line.size()) {
        string temp = "";
        while(line[i]!=' ' && i<line.size()) {
            temp += line[i++];
            //cout << "i:" << i << " val:" << line[i] ;
        }
        i++;
        //cout << "temp : " << temp << " ";
        ret.push_back(stod(temp));
    }
    return ret;
}

void init_graph() {
    //graph = {{0, 2, 5, 20}, {2, 0, 50, 3}, {5, 50, 0, 4}, {20, 3, 4, 0}};
    ifstream myfile;
    string filename, line;
    cout << "Input File Name : "; cin >> filename;
    myfile.open (filename);
    int flag = 0;
    while (getline (myfile, line)) {
        //cout << "line : " << line << endl;
        if (flag==0) {
            flag = 1;
            continue;
        }
        graph.push_back(getDistances(line));
        // for (auto i : graph.back())
        //     cout << i << " ";
        // cout << endl;     
    }
}

void solve () {
    auto start = high_resolution_clock::now(); 
    branch_bound (0, 0);
    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Total Cities : " << graph.size() << endl;
    cout << "\tTotal time taken : " << duration.count() << endl;
    cout << "\tTotal Paths Checked : " << numPaths << endl;
    cout << "\thortest " ;
    printShortestPath();
    cout << "\thortest Path Cost : " << upperBound << endl;    
}

int main() {
    init_graph();                       // initialize graph data structure for representing network between cities
    upperBound = calculate_upperBound();
    use_bound = 0;
    cout << endl;
    cout << "Without Branch and Bound : " << endl;
    solve();
    cout << endl; cout << endl;
    shortestPath.clear(); currPath.clear(); 
    numPaths = 0; use_bound = 1; upperBound = INT_MAX;
    cout << "With Branch and Bound : " << endl;
    solve();
    return 0;                         // return the minimum cost found
}
