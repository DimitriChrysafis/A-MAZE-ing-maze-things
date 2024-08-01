#ifndef CELL_H
#define CELL_H

struct Cell {
    // coordinates of cell
    int x, y;
    Cell() : x(0), y(0) {}
    Cell(int x, int y) : x(x), y(y) {}

    // equality operator to compare two cells.
    // reurns true if both cells have the same coordinates.


    bool operator==(const Cell& other) const {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Cell& other) const {
        return !(*this == other);
    }
};

#endif
