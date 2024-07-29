import numpy as np
import random


def prims(maze, start):
    height, width = maze.shape
    frontier = [(start, start)]

    while frontier:
        # choose a random edge from the frontier
        (currentX, currentY), (prevX, prevY) = random.choice(frontier)
        frontier.remove(((currentX, currentY), (prevX, prevY)))  # Remove it from the frontier

        # If the cell is a wall
        if maze[currentX, currentY] == 1:
            # mark the current cell as part of the maze
            maze[currentX, currentY] = 0
            # mark the previous cell as part of the maze
            maze[prevX, prevY] = 0


            # I AM NOT COMMENTING YEIELDS ANYMORE I QUIT
            yield maze.copy(), (currentX, currentY)
            yield maze.copy(), (prevX, prevY)

            # add all adjacent walls to the frontier
            for deltaX, deltaY in [(-2, 0), (2, 0), (0, -2), (0, 2)]:
                nextX, nextY = currentX + deltaX, currentY + deltaY
                if 0 <= nextX < height and 0 <= nextY < width and maze[nextX, nextY] == 1:
                    frontier.append(((nextX, nextY), ((currentX + nextX) // 2, (currentY + nextY) // 2)))


def generateMazePrims(height, width):
    maze = np.ones((height, width), dtype=np.uint8)

    # choose a random starting position in the maze, ensuring it's an even index
    start = (
        random.choice(range(0, height, 2)),
        random.choice(range(0, width, 2))
    )

    return prims(maze, start)
