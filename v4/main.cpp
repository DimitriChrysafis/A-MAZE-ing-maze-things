#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>

using namespace std;

void generateMaze(int width, int height, const string& filename) {
    size_t totalCells = width * height;
    vector<uint8_t> maze(totalCells, 255);
    vector<bool> visited(totalCells, false);

    vector<pair<int, int>> stack;
    stack.reserve(totalCells);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis;

    int startX = 1;
    int startY = 1;
    stack.push_back({startX, startY});
    visited[startY * width + startX] = true;
    maze[startY * width + startX] = 0;

    const pair<int, int> directions[] = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};

    while (!stack.empty()) {
        int cx, cy;
        tie(cx, cy) = stack.back();
        pair<int, int> neighbors[4];
        int neighborsCount = 0;

        for (const auto& [dx, dy] : directions) {
            int nx = cx + dx;
            int ny = cy + dy;
            if (nx > 0 && nx < width - 1 && ny > 0 && ny < height - 1 && !visited[ny * width + nx]) {
                neighbors[neighborsCount++] = {nx, ny};
            }
        }

        if (neighborsCount > 0) {
            int index = dis(gen) % neighborsCount;
            int nx, ny;
            tie(nx, ny) = neighbors[index];
            maze[((cy + ny) / 2) * width + (cx + nx) / 2] = 0;
            maze[ny * width + nx] = 0;
            visited[ny * width + nx] = true;
            stack.push_back({nx, ny});
        } else {
            stack.pop_back();
        }
    }

    ofstream outfile(filename, ios::binary);
    outfile.write(reinterpret_cast<const char*>(maze.data()), totalCells);
    outfile.close();
}

int main() {
    int width = 100000;
    int height = 100000;
    string filename = "/Users/dimitrichrysafis/desktop/maze.bin";

    auto startGenTime = chrono::high_resolution_clock::now();
    generateMaze(width, height, filename);
    auto endGenTime = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = endGenTime - startGenTime;
    cout << "time: " << elapsed.count() << " sec" << endl;

}
