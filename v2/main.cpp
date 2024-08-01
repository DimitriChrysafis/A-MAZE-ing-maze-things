#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include "image_utils.h"
#include "constants.h"
#include "maze_utils.h"
#include "cell.h"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

std::string generate_ffmpeg_command(const std::string& input_path, const std::string& output_path) {
    return "ffmpeg -framerate 30 -i " + input_path + "/%08d.png -c:v libx264 -preset slow -crf 18 " + output_path;
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    int startX = rand() % (width / 2) * 2 + 1;
    int startY = rand() % (height / 2) * 2 + 1;

    auto startTime = high_resolution_clock::now();
    dfs(startX, startY);
    auto endTime = high_resolution_clock::now();
    auto mazeGenDuration = duration_cast<milliseconds>(endTime - startTime);

    pair<Cell, Cell> farthestPoints = findFarthestPoints();
    Cell start = farthestPoints.first;
    Cell end = farthestPoints.second;

    cout << "Maze generation time: " << mazeGenDuration.count() << " ms" << endl;
    saveMazeImage("maze.png", start, end, {});

    startTime = high_resolution_clock::now();
    vector<Cell> path = findShortestPath(start, end);
    endTime = high_resolution_clock::now();
    auto pathfindingDuration = duration_cast<milliseconds>(endTime - startTime);

    cout << "Pathfinding time: " << pathfindingDuration.count() << " ms" << endl;
    saveMazeImage("maze_solved.png", start, end, path);


    vector<vector<unsigned char>> customColors = {
            {0, 0, 255},
            {0, 255, 255},
            {0, 255, 0},
            {255, 255, 0},
            {255, 0, 0}
    };

    saveCoolMap("heatmap.png", start, end, customColors, false);
    endTime = high_resolution_clock::now();
    auto coolMapRenderDuration = duration_cast<milliseconds>(endTime - startTime);

    cout << "Coolmap with custom colors rendering time: " << coolMapRenderDuration.count() << " ms" << endl;

    startTime = high_resolution_clock::now();
    saveCoolMap("coolmap_random.png", start, end, {}, true);
    endTime = high_resolution_clock::now();
    coolMapRenderDuration = duration_cast<milliseconds>(endTime - startTime);


    cout << "Random color coolmap time: " << coolMapRenderDuration.count() << " ms" << endl;

    string folder = "distance_animation";
    fs::create_directory(folder);

    int stepCount = 1;
    distanceAnimate(start, folder, stepCount);

    std::string input_path = "/Users/dimitrichrysafis/CLionProjects/Mazeology/cmake-build-debug/distance_animation";
    std::string output_path = "/Users/dimitrichrysafis/CLionProjects/Mazeology/cmake-build-debug/distance_animation/output.mp4";


    // i know more about rocket science than the next 30~ish lines
    string command = generate_ffmpeg_command(input_path, output_path);
    vector<std::string> args = {
            "sh",
            "-c",
            command
    };
    vector<char*> c_args;
    for (auto& arg : args) {
        c_args.push_back(arg.data());
    }
    c_args.push_back(nullptr);

    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Fork failed" << std::endl;
        return 1;
    }

    if (pid == 0) {
        execvp(c_args[0], c_args.data());
        cerr << "Exec failed" << std::endl;
        return 1;
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            cout << "status " << WEXITSTATUS(status) << std::endl;
        } else {
            cerr << "Child process failed" << std::endl;
        }
    }
    return 0;

}

/*
ffmpeg -framerate 24 -pattern_type glob -i '/Users/dimitrichrysafis/CLionProjects/Mazeology/cmake-build-debug/distance_animation*.png' \
-c:v libx264 -pix_fmt yuv420p /Users/dimitrichrysafis/CLionProjects/Mazeology/cmake-build-debug/output_video.mp4

 */