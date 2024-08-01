#include "image_utils.h"
#include "lodepng.h"
#include "maze_utils.h"
#include <iostream>
#include <filesystem>
#include "constants.h"
#include "cell.h"
#include <queue>
#include <unordered_set>
#include <random>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

using namespace std;

void saveMazeImage(const string& filename, Cell start, Cell end, const vector<Cell>& path) {
    // make the white background so it looks cool
    vector<unsigned char> image(width * height * 4, 255);

    // Fill the image based on the maze structure (iterate over x and y ect)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = 4 * (y * width + x);
            if (maze[y][x] == 0) {
                image[index + 0] = 0; // Red
                image[index + 1] = 0; // Green
                image[index + 2] = 0; // Blue
                image[index + 3] = 255; // Alpha
            }
        }
    }

    // mark the start cell in red
    image[(start.y * width + start.x) * 4 + 0] = 255;
    image[(start.y * width + start.x) * 4 + 1] = 0;
    image[(start.y * width + start.x) * 4 + 2] = 0;
    image[(start.y * width + start.x) * 4 + 3] = 255;

    // mark the end cell in blue
    image[(end.y * width + end.x) * 4 + 0] = 0;
    image[(end.y * width + end.x) * 4 + 1] = 0;
    image[(end.y * width + end.x) * 4 + 2] = 255;
    image[(end.y * width + end.x) * 4 + 3] = 255;


    // draw the path which solves it
    for (size_t i = 0; i < path.size(); ++i) {
        Cell cell = path[i];
        int index = 4 * (cell.y * width + cell.x);
        image[index + 0] = 0;
        image[index + 1] = 255;
        image[index + 2] = 0;
        image[index + 3] = 255;

        // mark the cells in between the path cells for animatinos
        if (i < path.size() - 1) {
            Cell nextCell = path[i + 1];
            int midX = (cell.x + nextCell.x) / 2;
            int midY = (cell.y + nextCell.y) / 2;
            int midIndex = 4 * (midY * width + midX);
            image[midIndex + 0] = 0;
            image[midIndex + 1] = 255;
            image[midIndex + 2] = 0;
            image[midIndex + 3] = 255;
        }
    }

    // oh my goodness, save the image and lodepng errors save me
    unsigned error = lodepng::encode(filename, image, width, height);
    if (error) {
        cout << "lodepng crash: " << error << ": " << lodepng_error_text(error) << endl;
    }
}


/*
 * I am not commenting this mainly cuz like
 * - BFS
 * draw the path
 * save the image
 * yah
 */
void animateSearch(Cell start, Cell end, int& stepCounter) {
    queue<Cell> q;
    unordered_map<int, Cell> prev;
    unordered_set<int> searched;
    q.push(start);
    prev[start.y * width + start.x] = start;

    saveMazeImage("frames/step_" + to_string(stepCounter++) + ".png", start, end, {});

    while (!q.empty()) {
        Cell current = q.front();
        q.pop();
        searched.insert(current.y * width + current.x);

        if (current == end) break;

        for (auto& neighbor : adjList[current.y][current.x]) {
            int key = neighbor.y * width + neighbor.x;
            if (prev.find(key) == prev.end()) {
                q.push(neighbor);
                prev[key] = current;

                vector<unsigned char> image(width * height * 4, 255);
                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        int index = 4 * (y * width + x);
                        if (maze[y][x] == 0) {
                            image[index + 0] = 0;
                            image[index + 1] = 0;
                            image[index + 2] = 0;
                            image[index + 3] = 255;
                        } else if (searched.find(y * width + x) != searched.end()) {
                            image[index + 0] = 255;
                            image[index + 1] = 255;
                            image[index + 2] = 0;
                            image[index + 3] = 255;
                        }
                    }
                }

                vector<Cell> tempPath;
                for (Cell at = neighbor; at != start; at = prev[at.y * width + at.x]) {
                    tempPath.push_back(at);
                }
                tempPath.push_back(start);
                reverse(tempPath.begin(), tempPath.end());

                for (const auto& cell : tempPath) {
                    int index = 4 * (cell.y * width + cell.x);
                    image[index + 0] = 0;
                    image[index + 1] = 255;
                    image[index + 2] = 0;
                    image[index + 3] = 255;
                }

                unsigned error = lodepng::encode("frames/step_" + to_string(stepCounter++) + ".png", image, width, height);
                if (error) {
                    cout << "Encoder error " << error << ": " << lodepng_error_text(error) << endl;
                }
            }
        }
    }
}


// THIS IS SO COOL
vector<unsigned char> interpolateColor(float t, const vector<vector<unsigned char>>& colors) {
    // get the number of colors in the gradient
    int numColors = colors.size();

    // scale the input 't' to match the number of intervals between colors
    // 't' ranges from 0 to 1, so multiply by (numColors - 1) to cover all intervals
    float scaledT = t * (numColors - 1);

    // determine the two colors between which 't' falls
    // idx1 is the lower index, and idx2 is the higher index, capped by numColors - 1
    int idx1 = floor(scaledT);
    int idx2 = min(idx1 + 1, numColors - 1);

    // calculate the alpha value for linear interpolation
    // this gives the fractional distance between the two colors
    float alpha = scaledT - idx1;

    // prepare a vector to hold the interpolated color with 3 elements for rgb channels
    vector<unsigned char> result(3);

    // loop through each color channel (rgb) to compute the interpolated value
    for (int i = 0; i < 3; ++i) {
        // linearly interpolate between the two colors' channels
        // (1 - alpha) * colors[idx1][i] gives the contribution of the first color
        // alpha * colors[idx2][i] gives the contribution of the second color
        // the sum gives the interpolated value for the current channel
        result[i] = static_cast<unsigned char>((1 - alpha) * colors[idx1][i] + alpha * colors[idx2][i]);
    }

    // return the interpolated color as a vector of unsigned chars
    return result;
}


// i aint commenting this either cuz like its just a function that generates random colors
vector<vector<unsigned char>> generateRandomColors() {
    vector<vector<unsigned char>> colors(256, vector<unsigned char>(3));
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);

    for (auto& color : colors) {
        color[0] = dis(gen);
        color[1] = dis(gen);
        color[2] = dis(gen);
    }
    return colors;
}


// NOTE BEFORE READING:::
// I HAD TO HAVE A LOT OF DEBUG STATEMENTS CUZ IT WASNR WORKING, IGNORE
void saveCoolMap(const string& filename, Cell start, Cell end, const vector<vector<unsigned char>>& customColors, bool randomColors) {
    cout << "starting saveCoolMap" << endl;
    // create an image buffer initialized to white
    vector<unsigned char> image(width * height * 4, 255);

    // unordered_map to store the distance from the start cell
    unordered_map<int, int> dist;
    queue<Cell> q;

    q.push(start);
    dist[start.y * width + start.x] = 0;
    int maxDist = 0;

    // perform a bfs
    while (!q.empty()) {
        Cell current = q.front();
        q.pop();

        int currentDist = dist[current.y * width + current.x];
        maxDist = max(maxDist, currentDist);

        // explore neighbors
        for (const auto& neighbor : adjList[current.y][current.x]) {
            int key = neighbor.y * width + neighbor.x;
            if (dist.find(key) == dist.end()) {
                dist[key] = currentDist + 1;
                q.push(neighbor);
            }
        }
    }

    vector<vector<unsigned char>> gradientColors;
    if (randomColors) {
        cout << "using random colors" << endl;
        gradientColors = generateRandomColors();
    } else if (!customColors.empty()) {
        cout << "using custom colors, size: " << customColors.size() << endl; // debug statement
        // interpolate custom colors to create a gradient of 256 colors
        for (int i = 0; i < 256; ++i) {
            float t = i / 255.0f;
            gradientColors.push_back(interpolateColor(t, customColors));
        }
    } else {
        cout << "using default colors" << endl;
        for (int i = 0; i < 256; ++i) {
            float norm = i / 255.0f;
            gradientColors.push_back(interpolateColor(norm, {{0, 0, 255}, {0, 255, 255}, {0, 255, 0}, {255, 255, 0}, {255, 0, 0}}));
        }
    }

    cout << "starting heatmap generation" << endl;
    // generate the heatmap based on distances and gradient colors (berry kewl)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = 4 * (y * width + x);
            if (maze[y][x] == 0) {
                // set walls to black
                image[index + 0] = 0;
                image[index + 1] = 0;
                image[index + 2] = 0;
                image[index + 3] = 255;
            } else {
                int distance = dist.count(y * width + x) ? dist[y * width + x] : 0;
                float norm = float(distance) / maxDist;
                int colorIndex = min(static_cast<int>(norm * 255), 255);

                if (colorIndex < 0 || colorIndex >= gradientColors.size()) {
                    cout << "invalid colorIndex: " << colorIndex << " at position (" << x << ", " << y << ")" << endl;
                }

                // get the color from the gradient
                const auto& color = gradientColors[colorIndex];

                // apply the color to the image
                image[index + 0] = color[0];
                image[index + 1] = color[1];
                image[index + 2] = color[2];
                image[index + 3] = 255; // alpha
            }
        }
    }

    cout << "saving the heatmap image" << endl; // debug statement
    unsigned error = lodepng::encode(filename, image, width, height);
    if (error) {
        cout << "lonepng is ass: " << error << ": " << lodepng_error_text(error) << endl;
    } else {
        cout << "image saved yayh" << endl;
    }
}



// read the funcitons above this just animates it's literally a bfs but it saves every step count
void distanceAnimate(Cell start, const string& folder, int stepCount) {
    unordered_map<int, int> dist;
    queue<Cell> q;
    q.push(start);
    dist[start.y * width + start.x] = 0;

    vector<vector<unsigned char>> gradientColors(256, vector<unsigned char>(3));
    for (int i = 0; i < 256; ++i) {
        float norm = i / 255.0f;
        vector<unsigned char> color = interpolateColor(norm, {{255, 0, 0}, {255, 255, 0}, {0, 255, 0}, {0, 255, 255}, {0, 0, 255}, {255, 0, 255}});
        gradientColors[i] = color;
    }

    int frameCounter = 1;
    int totalCells = width * height;
    int processedCells = 0;
    int maxDist = 0;

    while (!q.empty()) {
        int qSize = q.size();
        for (int i = 0; i < qSize; ++i) {
            Cell current = q.front();
            q.pop();
            processedCells++;

            int currentDist = dist[current.y * width + current.x];
            maxDist = max(maxDist, currentDist);
            for (const auto& neighbor : adjList[current.y][current.x]) {
                int key = neighbor.y * width + neighbor.x;
                if (dist.find(key) == dist.end()) {
                    dist[key] = currentDist + 1;
                    q.push(neighbor);
                }
            }
        }

        if (frameCounter % stepCount == 0) {
            vector<unsigned char> image(width * height * 4, 255);
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    int index = 4 * (y * width + x);
                    if (maze[y][x] == 0) {
                        image[index + 0] = 0;
                        image[index + 1] = 0;
                        image[index + 2] = 0;
                        image[index + 3] = 255;
                    } else {
                        int distance = dist.count(y * width + x) ? dist[y * width + x] : 0;
                        float norm = float(distance) / maxDist;
                        int colorIndex = min(static_cast<int>(norm * 255), 255);
                        const auto& color = gradientColors[colorIndex];

                        image[index + 0] = color[0];
                        image[index + 1] = color[1];
                        image[index + 2] = color[2];
                        image[index + 3] = 255;
                    }
                }
            }
            // this is so insanely important i can't express bro like message me if u wanna learn why i cant explain in 1 comment
            ostringstream filenameStream;
            filenameStream << folder << "/" << setw(2) << setfill('0') << frameCounter++ << ".png";
            string filename = filenameStream.str();
            unsigned error = lodepng::encode(filename, image, width, height);
            if (error) {
                cout << "Encoder error " << error << ": " << lodepng_error_text(error) << endl;
            }

            // print frame number and estimated completion percentage
            float completion = 100.0f * processedCells / totalCells;
            // bullshit line above ^^^^^^^
            cout << "Frame " << frameCounter << endl;
        } else {
            frameCounter++;
        }
    }
}