#include <iostream>
#include <stdlib.h>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <climits>
#include <fstream>
#include <chrono>
#include <string>
#include <signal.h>

using namespace std::chrono;
using namespace std;

vector<vector<double>> graph;           // adjacency matrix of cities : graph[i][j] represents edge cost between i and j
unordered_set<int> visited;             // unordered set to store visited cities
vector<int> currPath, shortestPath;     // store current path and shortest path
vector<double> firstMin, secondMin;     // vector to store edge with least cost and second least cost respectively for city with corresponding index
double best_cost = INT_MAX;             // upper bound used to prune the nodes
int start_node = 0, nodes_expanded   = 0, init_least_bound = 0;

// flag variables to determine which bnb-dfs strategy to use (naive, greedy, lower_bound, greedy+lower_bound)
int use_bnb, use_greedy = 0, V, use_rem_low_bound = 0; 

high_resolution_clock::time_point start, stop;
int MAX_RUNTIME = 15; // max runtime of program (in minutes)

// print the shortest path found
void printShortestPath() {
    cout << "Shortest Path : ";
    for (auto node : shortestPath)
        cout << node << " " ;
    cout << endl;
}

// print the path currently being traversed
void printCurrPath() {
    cout << "Curr Path : ";
    for (auto node : currPath)
        cout << node << " " ;
    cout << endl;
}

// print resullts obtained
void printResults() {
    stop = high_resolution_clock::now(); 
    auto dur = duration_cast<milliseconds>(stop - start);
    cout << "\tTotal Cities : " << graph.size() << endl;
    cout << "\tTotal time taken (in ms): " << dur.count() << endl;
    cout << "\tNo. of Nodes Expanded : " << nodes_expanded << endl;
    cout << "\t" ;printShortestPath();
    cout << "\tShortest Path Cost : " << best_cost << endl;
    cout << endl;
}

// function to stop the program
void stop_func(int signum) {
    printResults();
    exit(0);   
}

// recursive function to perform branch and bound - dfs
// curr_vertex = current city, rem_low_bound = lower bound cost of remaining cities not yet traversed
void bnb_dfs(int curr_vertex, double cost, int rem_low_bound) { 

    stop = high_resolution_clock::now(); 
    auto dur = duration_cast<milliseconds>(stop - start);
    if (dur.count() >= MAX_RUNTIME*60000)
        stop_func(0);

    // update to shortest path if all cities traveresed and cost is least
    if(visited.size()==graph.size() && curr_vertex==start_node) {
        if (cost < best_cost) {
            best_cost = cost;
            shortestPath = currPath; 
            shortestPath.push_back(start_node);
            //printShortestPath();
            //cout << "cost : " << cost << endl;
        }
    }

    // find the lower bound of cost of traversing remaining cities from current
    if (currPath.size()==1) 
        rem_low_bound -= ((firstMin[currPath.back()] + firstMin[curr_vertex])/2); 
    else if (currPath.size()>1)
        rem_low_bound -= ((secondMin[currPath.back()] + firstMin[curr_vertex])/2); 
    
    // Choose strategy to prune the nodes  : if already visited or cost>best cost found or lower bound for current path > best cost found
    if (visited.find(curr_vertex)!=visited.end() 
            || (use_bnb && cost>=best_cost) 
            || (use_rem_low_bound && cost + rem_low_bound>= best_cost))
        return;

    nodes_expanded++;
    visited.insert(curr_vertex);
    currPath.push_back(curr_vertex);
    
    vector<pair<int,int>> edges;
    for(int i=0; i<graph[curr_vertex].size(); i++)
        if (graph[curr_vertex][i]!=INT_MAX &&  curr_vertex!=i)
            edges.push_back({graph[curr_vertex][i], i});

    // pick lower cost edges first for greedy strategy
    if (use_greedy) 
        sort(edges.begin(), edges.end());
    
    // traverse neighbours
    for (auto edge : edges)
        if (graph[curr_vertex][edge.second]!=INT_MAX && curr_vertex!=edge.second)
            bnb_dfs(edge.second, cost + graph[curr_vertex][edge.second], rem_low_bound);

    currPath.pop_back();
    visited.erase(curr_vertex);
}

// function to call bnb-dfs and measure its execution time
void solve_tsp () {
    start = high_resolution_clock::now(); 
    bnb_dfs (0, 0, init_least_bound);
    stop = high_resolution_clock::now(); 
    stop_func(0);  
}

// calculate least edge cost for every node
void calcfirstMin() { 
    for (int i=0; i<graph.size(); i++) {
        double min = INT_MAX, N = graph.size(); 
        for (int k=0; k<N; k++) 
            if (graph[i][k]<min && i != k) 
                min = graph[i][k]; 
        firstMin.push_back(min); 
    }
} 
  
// calculate second least edge cost for every node
void calcSecondMin() { 
    for (int i=0; i<graph.size(); i++) {
        double first = INT_MAX, second = INT_MAX, N = graph.size(); 
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

// calculate initial least bound cost
void calcInitLeastBound() {
    for (int i=0; i<graph.size(); i++) 
        init_least_bound += (firstMin[i] + secondMin[i]); 
    init_least_bound = (init_least_bound&1)? init_least_bound/2 + 1 : init_least_bound/2; 
}

// get distances from a line of file input
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

// initialie adjacency matrix using input file
void init_graph() {
    ifstream myfile;
    string filename, line;
    cout << "\nInput File Name : "; cin >> filename;
    myfile.open (filename);
    if (!myfile.is_open()) {
        cout << "File Not Found !!" << endl;
        exit(0);
    }
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

// reset the parameters for different types of bnb-dfs : naive, greedy, lower_bound, greedy+lower_bound
void reset(int useBound, int useRemLowBound, int useGreedy, string type) {
    cout << endl; cout << endl;
    shortestPath.clear(); currPath.clear(); 
    nodes_expanded = 0; best_cost = INT_MAX;
    use_bnb = useBound;  use_rem_low_bound = useRemLowBound;  use_greedy = useGreedy;
    cout << type << endl;
}

int main() {

    init_graph();
    signal(SIGINT, stop_func);

    /***** 
        UNCOMMENT THE CODE SECTIONS BELOW 
        TO RUN THE RESPECTIVE STATEGY (LESS OPTIMAL)
    *****/


    // reset(0, 0, 0, "Brute Force : ");
    // solve_tsp();

    // reset(1, 0, 0, "Naive Branch and Bound : ");
    // solve_tsp();

    // reset(1, 0, 1, "Branch and Bound With Greedy Strategy Only: ");
    // solve_tsp();

    // reset(1, 1, 0, "Branch and Bound With Lower Bound Strategy Only: ");
    // solve_tsp(); 

    reset(1, 1, 1, "Branch and Bound With Greedy And Lower Bound Strategy: " );
    solve_tsp();

    cout << "\n" << endl;

    return 0;                       
}
