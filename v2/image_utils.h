#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <string>
#include <vector>
#include "cell.h"

using namespace std;

void saveMazeImage(const string& filename, Cell start, Cell end, const vector<Cell>& path);
void animateSearch(Cell start, Cell end, int& stepCounter);
vector<unsigned char> interpolateColor(float t, const vector<unsigned char>& color1, const vector<unsigned char>& color2);
vector<vector<unsigned char>> generateRandomColors();
void saveCoolMap(const string& filename, Cell start, Cell end, const vector<std::vector<unsigned char>>& customColors = {}, bool randomColors = false);
void distanceAnimate(Cell start, const string& folder, int stepCount);

#endif
