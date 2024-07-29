import numpy as np
import random

class DisjointSet:
    
    #A class to represent a disjoint set for Kruskal's algorithm because
    

    def __init__(self):
        self.parent = {}

    def find(self, item):
        # Find the root of the set in which item belongs.

        if self.parent[item] == item:
            return item
        else:
            self.parent[item] = self.find(self.parent[item])
            return self.parent[item]

    def union(self, set1, set2):

        root1 = self.find(set1)
        root2 = self.find(set2)
        self.parent[root2] = root1

    def makeSet(self, item):
        self.parent[item] = item

def kruskals(maze):
    height, width = maze.shape
    sets = DisjointSet()
    edges = []

    # initialize each cell as a separate set and create a list of edges
    for x in range(0, height, 2):
        for y in range(0, width, 2):
            sets.makeSet((x, y))
            if x > 0:
                # add vertical edges
                edges.append(((x, y), (x - 2, y)))
            if y > 0:
                # add horizontal edges
                edges.append(((x, y), (x, y - 2)))

    random.shuffle(edges)  # Randomize the edge list

    # process each edge, joining sets if they are not already connected
    for (x1, y1), (x2, y2) in edges:
        if sets.find((x1, y1)) != sets.find((x2, y2)):
            sets.union((x1, y1), (x2, y2))

            # remove the wall between the connected cells
            maze[(x1 + x2) // 2, (y1 + y2) // 2] = 0
            maze[x1, y1] = 0
            maze[x2, y2] = 0

            yield maze.copy(), ((x1 + x2) // 2, (y1 + y2) // 2)
            yield maze.copy(), (x1, y1)
            yield maze.copy(), (x2, y2)

def generateMazeKruskals(height, width):
    maze = np.ones((height, width), dtype=np.uint8)
    return kruskals(maze)
