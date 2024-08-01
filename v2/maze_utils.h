#ifndef MAZE_UTILS_H
#define MAZE_UTILS_H

#include <vector>
#include <queue>
#include <unordered_map>
#include "cell.h"

using namespace std;

extern vector<vector<int>> maze;
extern vector<vector<vector<Cell>>> adjList;

vector<Cell> getNeighbors(int x, int y);
bool isValid(int x, int y);
void dfs(int startX, int startY);
pair<Cell, Cell> findFarthestPoints();
vector<Cell> findShortestPath(Cell start, Cell end);

#endif
