#include <opencv2/opencv.hpp>
#include <iostream>
#include <queue>
#include <stack>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <filesystem>
#include <thread>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <fstream>
#include <random>

using namespace cv;
using namespace std;
using namespace std::chrono;
namespace fs = std::__fs::filesystem;

const string DESKTOP_PATH = "/Volumes/LaCie/MazeStuff/";
// put ur path here

const int DX[4] = {1, 0, -1, 0};
const int DY[4] = {0, 1, 0, -1};

bool isValid(int x, int y, int width, int height) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

void shuffleDirections(std::vector<int>& directions) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(directions.begin(), directions.end(), g);
}

void generateMaze(const string& mazeFile, int width, int height, int startX, int startY, atomic<double>& progress) {
    ofstream mazeOut(mazeFile, ios::binary);
    vector<vector<int>> maze(height, vector<int>(width, 0));
    stack<pair<int, int>> stack;
    stack.push({startX, startY});
    maze[startY][startX] = 1;
    int totalCells = (width / 2) * (height / 2);
    int visitedCells = 1;
    double lastReportedProgress = 0.0;

    while (!stack.empty()) {
        int x = stack.top().first;
        int y = stack.top().second;

        vector<int> directions = {0, 1, 2, 3};
        shuffleDirections(directions);

        bool moved = false;
        for (int dir : directions) {
            int nx = x + 2 * DX[dir];
            int ny = y + 2 * DY[dir];

            if (isValid(nx, ny, width, height) && maze[ny][nx] == 0) {
                maze[ny][nx] = 1;
                maze[y + DY[dir]][x + DX[dir]] = 1;
                stack.push({nx, ny});
                moved = true;
                visitedCells++;
                progress = (visitedCells / static_cast<double>(totalCells)) * 100.0;

                if (progress - lastReportedProgress >= 1.0) { // Report every 1% progress
                    cout << "Maze generation progress: " << progress << "%" << endl;
                    lastReportedProgress = progress;
                }

                break;
            }
        }

        if (!moved) {
            stack.pop();
        }
    }

    for (const auto& row : maze) {
        mazeOut.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(int));
    }

    progress = 100.0;
    cout << "Maze generation progress: 100%" << endl;
    mazeOut.close();
    cout << "Maze generation complete." << endl;

    fs::path mazePath(mazeFile);
    auto mazeSize = fs::file_size(mazePath);
    cout << "Maze file stored at: " << mazeFile << endl;
    cout << "Maze file size: " << mazeSize << " bytes" << endl;
}



void calculateDistances(const string& mazeFile, const string& distFile, int width, int height, int startX, int startY, atomic<double>& progress) {
    ifstream mazeIn(mazeFile, ios::binary);
    ofstream distOut(distFile, ios::binary);

    vector<vector<int>> maze(height, vector<int>(width, 0));
    for (auto& row : maze) {
        mazeIn.read(reinterpret_cast<char*>(row.data()), row.size() * sizeof(int));
    }

    vector<vector<int>> distances(height, vector<int>(width, -1));
    queue<pair<int, int>> q;
    q.push({startX, startY});
    distances[startY][startX] = 0;
    int totalCells = 0;
    for (const auto& row : maze) {
        totalCells += count(row.begin(), row.end(), 1);
    }
    int processedCells = 0;

    while (!q.empty()) {
        int x = q.front().first;
        int y = q.front().second;
        q.pop();

        for (int dir = 0; dir < 4; ++dir) {
            int nx = x + DX[dir];
            int ny = y + DY[dir];

            if (isValid(nx, ny, width, height) && maze[ny][nx] == 1 && distances[ny][nx] == -1) {
                distances[ny][nx] = distances[y][x] + 1;
                q.push({nx, ny});
                processedCells++;
                progress = (processedCells / static_cast<double>(totalCells)) * 100.0;
            }
        }
    }

    for (const auto& row : distances) {
        distOut.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(int));
    }

    progress = 100.0;
    distOut.close();
    mazeIn.close();
    cout << "Distance calculation complete for start (" << startX << ", " << startY << ")." << endl;
}

Vec3b getColorForCell(int distTopRight, int distBottomLeft, int maxDist) {
    double normDistTopRight = distTopRight / static_cast<double>(maxDist);
    double normDistBottomLeft = distBottomLeft / static_cast<double>(maxDist);

    // Sinusoidal blend factor
    double blendFactor = 0.5 + 0.5 * sin(10 * (normDistTopRight + normDistBottomLeft) * M_PI);

    // Color ranges for blending
    int hue1 = static_cast<int>(360 * normDistTopRight);    // Range for top right
    int hue2 = static_cast<int>(360 * (1 - normDistBottomLeft)); // Range for bottom left

    // Blend the two hues
    int hue = static_cast<int>((hue1 * blendFactor + hue2 * (1 - blendFactor))) % 360;

    // Create a Vec3b color in HSV space
    Vec3b hsvColor;
    hsvColor[0] = hue;  // Hue
    hsvColor[1] = 200;  // Saturation (high value for vibrant colors)
    hsvColor[2] = 255;  // Value (maximum brightness)

    // Convert HSV to BGR for display
    Mat hsv(1, 1, CV_8UC3, hsvColor);
    Mat rgb;
    cvtColor(hsv, rgb, COLOR_HSV2BGR);

    return rgb.at<Vec3b>(0, 0);
}



void renderMazeSegment(const string& mazeFile, const string& distTopRightFile, const string& distBottomLeftFile, Mat& image, int startY, int endY, int width, int maxDist, atomic<double>& progress, int segmentIndex, int numSegments) {
    ifstream mazeIn(mazeFile, ios::binary);
    ifstream distTopRightIn(distTopRightFile, ios::binary);
    ifstream distBottomLeftIn(distBottomLeftFile, ios::binary);

    vector<int> row(width);
    vector<int> distTopRightRow(width);
    vector<int> distBottomLeftRow(width);

    mazeIn.seekg(startY * width * sizeof(int), ios::beg);
    distTopRightIn.seekg(startY * width * sizeof(int), ios::beg);
    distBottomLeftIn.seekg(startY * width * sizeof(int), ios::beg);

    int segmentCells = width * (endY - startY);
    int processedCells = 0;

    for (int y = startY; y < endY; ++y) {
        mazeIn.read(reinterpret_cast<char*>(row.data()), row.size() * sizeof(int));
        distTopRightIn.read(reinterpret_cast<char*>(distTopRightRow.data()), distTopRightRow.size() * sizeof(int));
        distBottomLeftIn.read(reinterpret_cast<char*>(distBottomLeftRow.data()), distBottomLeftRow.size() * sizeof(int));

        for (int x = 0; x < width; ++x) {
            if (row[x] == 0) {
                image.at<Vec3b>(y, x) = Vec3b(0, 0, 0);
            } else {
                image.at<Vec3b>(y, x) = getColorForCell(distTopRightRow[x], distBottomLeftRow[x], maxDist);
            }
            processedCells++;
            progress = (((segmentIndex * segmentCells + processedCells) / static_cast<double>(width * (endY - startY))) / numSegments) * 100.0;
        }
    }

    progress = 100.0;
    mazeIn.close();
    distTopRightIn.close();
    distBottomLeftIn.close();
    cout << "Rendering segment " << segmentIndex + 1 << " complete." << endl;
}

void renderMaze(const string& mazeFile, const string& distTopRightFile, const string& distBottomLeftFile, int width, int height, const string& outputFile, atomic<double>& progress) {
    Mat image(height, width, CV_8UC3, Scalar(255, 255, 255));

    ifstream distTopRightIn(distTopRightFile, ios::binary);
    ifstream distBottomLeftIn(distBottomLeftFile, ios::binary);

    int maxDistTopRight = 0;
    int maxDistBottomLeft = 0;

    vector<int> row(width);

    for (int y = 0; y < height; ++y) {
        distTopRightIn.read(reinterpret_cast<char*>(row.data()), row.size() * sizeof(int));
        for (int x = 0; x < width; ++x) {
            if (row[x] > maxDistTopRight) {
                maxDistTopRight = row[x];
            }
        }
    }

    for (int y = 0; y < height; ++y) {
        distBottomLeftIn.read(reinterpret_cast<char*>(row.data()), row.size() * sizeof(int));
        for (int x = 0; x < width; ++x) {
            if (row[x] > maxDistBottomLeft) {
                maxDistBottomLeft = row[x];
            }
        }
    }

    distTopRightIn.close();
    distBottomLeftIn.close();

    int maxDist = max(maxDistTopRight, maxDistBottomLeft);

    int numThreads = thread::hardware_concurrency();
    vector<thread> threads;
    int segmentHeight = height / numThreads;

    cout << "Starting rendering with " << numThreads << " threads..." << endl;

    for (int i = 0; i < numThreads; ++i) {
        int startY = i * segmentHeight;
        int endY = (i == numThreads - 1) ? height : startY + segmentHeight;
        threads.emplace_back(renderMazeSegment, mazeFile, distTopRightFile, distBottomLeftFile, ref(image), startY, endY, width, maxDist, ref(progress), i, numThreads);
    }

    for (auto& t : threads) {
        t.join();
    }

    imwrite(outputFile, image);
    cout << "Rendering complete." << endl;
}
pair<int, int> bfsFarthestPoint(const vector<vector<int>>& maze, int width, int height, int startX, int startY) {
    vector<vector<bool>> visited(height, vector<bool>(width, false));
    queue<pair<pair<int, int>, int>> q; // ((x, y), distance)
    q.push({{startX, startY}, 0});
    visited[startY][startX] = true;

    pair<int, int> farthestPoint = {startX, startY};
    int maxDist = 0;

    while (!q.empty()) {
        int x = q.front().first.first;
        int y = q.front().first.second;
        int dist = q.front().second;
        q.pop();

        if (dist > maxDist) {
            maxDist = dist;
            farthestPoint = {x, y};
        }

        for (int dir = 0; dir < 4; ++dir) {
            int nx = x + DX[dir];
            int ny = y + DY[dir];

            if (isValid(nx, ny, width, height) && maze[ny][nx] == 1 && !visited[ny][nx]) {
                visited[ny][nx] = true;
                q.push({{nx, ny}, dist + 1});
            }
        }
    }

    return farthestPoint;
}

pair<pair<int, int>, pair<int, int>> generateRandomStartAndEnd(const string& mazeFile, int width, int height) {
    ifstream mazeIn(mazeFile, ios::binary);
    vector<vector<int>> maze(height, vector<int>(width, 0));
    for (auto& row : maze) {
        mazeIn.read(reinterpret_cast<char*>(row.data()), row.size() * sizeof(int));
    }
    mazeIn.close();

    int startX = rand() % width;
    int startY = rand() % height;
    while (maze[startY][startX] == 0) { // Ensure starting point is on a path
        startX = rand() % width;
        startY = rand() % height;
    }

    pair<int, int> firstFarthestPoint = bfsFarthestPoint(maze, width, height, startX, startY);
    pair<int, int> secondFarthestPoint = bfsFarthestPoint(maze, width, height, firstFarthestPoint.first, firstFarthestPoint.second);

    return {firstFarthestPoint, secondFarthestPoint};
}

void createArtisticMaze(int width, int height, const string& outputImage) {
    string mazeFile = DESKTOP_PATH + "maze.bin";
    string distTopRightFile = DESKTOP_PATH + "dist_top_right.bin";
    string distBottomLeftFile = DESKTOP_PATH + "dist_bottom_left.bin";

    atomic<double> progressMaze(0.0);
    atomic<double> progressDistTopRight(0.0);
    atomic<double> progressDistBottomLeft(0.0);
    atomic<double> progressRender(0.0);

    auto start = high_resolution_clock::now();

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> disWidth(0, width - 1);
    uniform_int_distribution<> disHeight(0, height - 1);

    int startX = disWidth(gen);
    int startY = disHeight(gen);
    cout << "Generating maze..." << endl;
    thread mazeThread(generateMaze, mazeFile, width, height, startX, startY, ref(progressMaze));
    mazeThread.join();
    cout << "Maze generation done." << endl;

    auto [startTopRight, startBottomLeft] = generateRandomStartAndEnd(mazeFile, width, height);

    cout << "Calculating distances to top right..." << endl;
    thread distTopRightThread(calculateDistances, mazeFile, distTopRightFile, width, height, startTopRight.first, startTopRight.second, ref(progressDistTopRight));
    cout << "Calculating distances to bottom left..." << endl;
    thread distBottomLeftThread(calculateDistances, mazeFile, distBottomLeftFile, width, height, startBottomLeft.first, startBottomLeft.second, ref(progressDistBottomLeft));

    distTopRightThread.join();
    cout << "Distance calculation to top right done." << endl;
    distBottomLeftThread.join();
    cout << "Distance calculation to bottom left done." << endl;

    cout << "Rendering maze..." << endl;
    renderMaze(mazeFile, distTopRightFile, distBottomLeftFile, width, height, outputImage, progressRender);
    cout << "Rendering done." << endl;

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "Maze generation and rendering took " << duration.count() << " milliseconds." << endl;

    fs::path mazePath(mazeFile);
    fs::path distTopRightPath(distTopRightFile);
    fs::path distBottomLeftPath(distBottomLeftFile);

    auto mazeSize = fs::file_size(mazePath);
    auto distTopRightSize = fs::file_size(distTopRightPath);
    auto distBottomLeftSize = fs::file_size(distBottomLeftPath);

    cout << "Maze file size: " << mazeSize << " bytes" << endl;
    cout << "Distance to top right file size: " << distTopRightSize << " bytes" << endl;
    cout << "Distance to bottom left file size: " << distBottomLeftSize << " bytes" << endl;

    auto totalFootprint = mazeSize + distTopRightSize + distBottomLeftSize;
    cout << "Total footprint before removal: " << totalFootprint << " bytes" << endl;

    if (fs::remove(mazeFile)) {
        cout << "Removed file: " << mazeFile << " (" << mazeSize << " bytes)" << endl;
    }
    if (fs::remove(distTopRightFile)) {
        cout << "Removed file: " << distTopRightFile << " (" << distTopRightSize << " bytes)" << endl;
    }
    if (fs::remove(distBottomLeftFile)) {
        cout << "Removed file: " << distBottomLeftFile << " (" << distBottomLeftSize << " bytes)" << endl;
    }
    cout << "Total footprint after removal: " << (totalFootprint - (mazeSize + distTopRightSize + distBottomLeftSize)) << " bytes" << endl;
}

string generateRandomSuffix() {
    int randomNum = rand() % 1000;
    stringstream ss;
    ss << setw(3) << setfill('0') << randomNum;
    return ss.str();
}

int main() {
    srand(time(0));
    int width = 10000;
    int height = 10000;

    string suffix = generateRandomSuffix();
    string fileName = DESKTOP_PATH + "artistic_maze_" + suffix + ".png";

    createArtisticMaze(width, height, fileName);

    cout << "COMPLETED" << fileName << endl;
}
