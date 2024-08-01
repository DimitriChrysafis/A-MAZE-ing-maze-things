#include "maze_utils.h"
#include <stack>
#include <random>
#include <algorithm>
#include <iostream>
#include "constants.h"

using namespace std;

// initialize the maze with 0s
vector<vector<int>> maze(height, vector<int>(width, 0)); // 0 = wall, 1 = path
vector<vector<vector<Cell>>> adjList(height, vector<vector<Cell>>(width));

// WANNA GUESS HAT THIS DOES (read the na\me)
vector<Cell> getNeighbors(int x, int y) {
    vector<Cell> neighbors;
    if (x > 1) {
        neighbors.push_back(Cell(x - 2, y));
    }
    if (x < width - 2) {
        neighbors.push_back(Cell(x + 2, y));
    }
    if (y > 1) {
        neighbors.push_back(Cell(x, y - 2));
    }
    if (y < height - 2) {
        neighbors.push_back(Cell(x, y + 2));
    }
    return neighbors;
}

// no comment
bool isValid(int x, int y) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

// dfs to make the maze
void dfs(int startX, int startY) {
    stack<Cell> s;
    s.push(Cell(startX, startY));
    maze[startY][startX] = 1;

    random_device rd;
    mt19937 g(rd());

    while (!s.empty()) {
        Cell current = s.top();
        s.pop();

        // get and shuffle the neighbors to randomize the maze
        vector<Cell> neighbors = getNeighbors(current.x, current.y);
        shuffle(neighbors.begin(), neighbors.end(), g);

        for (auto& neighbor : neighbors) {
            if (isValid(neighbor.x, neighbor.y) && maze[neighbor.y][neighbor.x] == 0) {
                // mark the cell as a path
                maze[neighbor.y][neighbor.x] = 1;
                maze[(current.y + neighbor.y) / 2][(current.x + neighbor.x) / 2] = 1; // mark the cell between current and neighbor as a path

                adjList[current.y][current.x].push_back(neighbor);
                adjList[neighbor.y][neighbor.x].push_back(current);

                s.push(neighbor); // push the neighbor to the stack to continue dfs
            }
        }
    }
}

// function to find the farthest points in the maze using bfs
pair<Cell, Cell> findFarthestPoints() {
    // helper lambda function to perform bfs
    auto bfs = [&](Cell start) {
        queue<Cell> q;
        unordered_map<int, int> dist;
        q.push(start);
        dist[start.y * width + start.x] = 0;

        Cell farthest = start;
        while (!q.empty()) {
            Cell current = q.front();
            q.pop();

            for (auto& neighbor : adjList[current.y][current.x]) {
                int key = neighbor.y * width + neighbor.x;
                if (dist.find(key) == dist.end()) {
                    dist[key] = dist[current.y * width + current.x] + 1;
                    q.push(neighbor);
                    if (dist[key] > dist[farthest.y * width + farthest.x]) {
                        farthest = neighbor;
                    }
                }
            }
        }
        return farthest;
    };

    Cell randomStart = Cell(rand() % (width / 2) * 2 + 1, rand() % (height / 2) * 2 + 1);

    // THESE ARE FOR THE START AND EXIT
    Cell farthest1 = bfs(randomStart);
    Cell farthest2 = bfs(farthest1);
    return {farthest1, farthest2};
}

// function to find the shortest path between two cells using bfs
vector<Cell> findShortestPath(Cell start, Cell end) {
    queue<Cell> q;
    unordered_map<int, Cell> prev;
    q.push(start);
    prev[start.y * width + start.x] = start;

    while (!q.empty()) {
        Cell current = q.front();
        q.pop();

        if (current == end) {
            break;
        }

        for (auto& neighbor : adjList[current.y][current.x]) {
            int key = neighbor.y * width + neighbor.x;
            if (prev.find(key) == prev.end()) {
                q.push(neighbor);
                prev[key] = current;
            }
        }
    }

    // reconstruct the path from end to start
    vector<Cell> path;
    for (Cell at = end; at != start; at = prev[at.y * width + at.x]) {
        path.push_back(at);
    }
    path.push_back(start);
    // reverse the path to go from start to end )I<PORTANTAIDYFGKADJHFGDAJKFHA
    reverse(path.begin(), path.end());
    return path;
}


