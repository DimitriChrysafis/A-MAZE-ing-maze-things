import numpy as np
import random
## read the comments bruh its hard otherwise
def dfs(maze, start):
    stack = [start]
    height, width = maze.shape
    visited = set()

    while stack:
        # get the cell im at rn
        cell = stack[-1]
        x, y = cell

        # mark the current cell as visited (part of the path)
        maze[x, y] = 0
        # add it to visited lmao read the code
        visited.add(cell)

        yield maze.copy(), (x, y)

        neighbors = []

        # check all four possible neighbors (up, down, left, right)
        for deltaX, deltaY in [(-2, 0), (2, 0), (0, -2), (0, 2)]:
            newX, newY = x + deltaX, y + deltaY

            # check if the neighbor is within the maze bounds and not visited
            if 0 <= newX < height and 0 <= newY < width and (newX, newY) not in visited:
                neighbors.append((newX, newY))

        if neighbors:
            # Randomly select a neighbor to visit next so its not like completely the same every dfs
            nextCell = random.choice(neighbors)
            newX, newY = nextCell

            # remove the wall between the current cell and the chosen neighbor
            maze[(x + newX) // 2, (y + newY) // 2] = 0

            # push the chosen neighbor onto the stack
            stack.append(nextCell)

            # yield the state after removing the wall
            yield maze.copy(), ((x + newX) // 2, (y + newY) // 2)
        else:
            # backtrack if there are no unvisited neighbors
            stack.pop()


def generateMazeDfs(height, width):
    maze = np.ones((height, width), dtype=np.uint8)

    # random start lpmao
    start = (
        random.choice(range(0, height, 2)),
        random.choice(range(0, width, 2))
    )

    return dfs(maze, start)
