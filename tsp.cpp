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

vector<vector<double>> graph;          // graphecency matrix of cities : graph[i][j] represents edge cost between i and j
unordered_set<int> visited;
vector<int> currPath, shortestPath, firstMin, secondMin;         
double upperBound = INT_MAX;          // upper bound of path cost used to prune the path
int start_node = 0, use_bnb, user_mst, nodes_expanded = 0, use_greedy = 0, V, mst_w = 0, use_rem_low_bound = 0, init_least_bound = 0;

void printShortestPath() {
    cout << "Shortest Path : ";
    for (auto node : shortestPath)
        cout << node << " " ;
    cout << endl;
}

void branch_bound(int curr_vertex, double cost, int rem_low_bound) {

    if(visited.size()==graph.size() && curr_vertex==start_node) {
        if (cost < upperBound) {
            upperBound = cost;
            shortestPath = currPath; 
            shortestPath.push_back(start_node);
        }
    }

    if (currPath.size()==1) 
        rem_low_bound -= ((firstMin[currPath.back()] + firstMin[curr_vertex])/2); 
    else if (currPath.size()>1)
        rem_low_bound -= ((secondMin[currPath.back()] + firstMin[curr_vertex])/2); 

    if (visited.find(curr_vertex)!=visited.end() 
            || (use_bnb && cost>=upperBound) 
            || (use_rem_low_bound && cost + rem_low_bound>= upperBound))
        return;

    nodes_expanded++;

    visited.insert(curr_vertex);
    currPath.push_back(curr_vertex);
    
    vector<pair<int,int>> edges;
    for(int i=0; i<graph[curr_vertex].size(); i++)
        if (graph[curr_vertex][i]!=INT_MAX &&  curr_vertex!=i)
            edges.push_back({graph[curr_vertex][i], i});
    if (use_greedy) sort(edges.begin(), edges.end());
    for (auto edge : edges)
        if (graph[curr_vertex][edge.second]!=INT_MAX && curr_vertex!=edge.second)
            branch_bound(edge.second, cost + graph[curr_vertex][edge.second], rem_low_bound);

    currPath.pop_back();
    visited.erase(curr_vertex);
}

void solve () {
    auto start = high_resolution_clock::now(); 
    branch_bound (0, 0, init_least_bound);
    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "\tTotal Cities : " << graph.size() << endl;
    cout << "\tTotal time taken (in ms): " << duration.count() << endl;
    cout << "\tNo. of Nodes Expanded : " << nodes_expanded << endl;
    cout << "\t" ;printShortestPath();
    cout << "\tShortest Path Cost : " << upperBound << endl;    
}

void calcfirstMin() { 
    for (int i=0; i<graph.size(); i++) {
        int min = INT_MAX, N = graph.size(); 
        for (int k=0; k<N; k++) 
            if (graph[i][k]<min && i != k) 
                min = graph[i][k]; 
        firstMin.push_back(min); 
    }
} 
  

void calcSecondMin() { 
    for (int i=0; i<graph.size(); i++) {
        int first = INT_MAX, second = INT_MAX, N = graph.size(); 
        for (int j=0; j<N; j++) { 
            if (i == j) 
                continue; 
            if (graph[i][j] <= first) { 
                second = first; 
                first = graph[i][j]; 
            } else if (graph[i][j] <= second && graph[i][j] != first) 
                second = graph[i][j]; 
        } 
        secondMin.push_back(second); 
    }
}

void calcInitLeastBound() {
    for (int i=0; i<graph.size(); i++) 
        init_least_bound += (firstMin[i] + secondMin[i]); 
    init_least_bound = (init_least_bound&1)? init_least_bound/2 + 1 : init_least_bound/2; 
}


vector<double> getDistances(string& line) {
    int i=0;
    vector<double> ret;
    while(i<line.size()) {
        string temp = "";
        while(line[i]!=' ' && i<line.size())
            temp += line[i++];
        i++;
        ret.push_back(stod(temp));
    }
    return ret;
}

void init_graph() {
    ifstream myfile;
    string filename, line;
    cout << "\nInput File Name : "; cin >> filename;
    myfile.open (filename);
    int flag = 0;
    while (getline (myfile, line)) {
        if (flag==0) {
            flag = 1;
            continue;
        }
        graph.push_back(getDistances(line));  
    }
    calcfirstMin();
    calcSecondMin();
    calcInitLeastBound();
}

void reset(int useBound, int useRemLowBound, int useGreedy, string type) {
    cout << endl; cout << endl;
    shortestPath.clear(); currPath.clear(); 
    nodes_expanded = 0; upperBound = INT_MAX;
    use_bnb = useBound;  use_rem_low_bound = useRemLowBound;  use_greedy = useGreedy;
    cout << type << endl;
}

int main() {
    init_graph();

    // reset(0, 0, 0, "Brute Force : ");
    // solve();

    reset(1, 0, 0, "Naive Branch and Bound : ");
    solve();

    reset(1, 0, 1, "Branch and Bound And Greedy: ");
    solve();

    reset(1, 1, 0, "Branch and Bound And Remaining Lower Bound (RLB): ");
    solve();

    reset(1, 1, 1, "Branch and Bound And Greedy And RLB: " );
    solve();

    cout << "\n" << endl;

    return 0;                       
}
