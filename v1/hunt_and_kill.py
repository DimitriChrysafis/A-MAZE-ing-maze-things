import numpy as np
import random

def huntAndKill(maze):
    height, width = maze.shape

    # start at a random position on an even index within bounds
    x, y = random.choice(range(0, height, 2)), random.choice(range(0, width, 2))
    # mark the start
    maze[x, y] = 0

    while True:
        # find all unvisited neighbors (cells with walls)
        neighbors = [
            (x + dx, y + dy)
            for dx, dy in [(-2, 0), (2, 0), (0, -2), (0, 2)]
            if 0 <= x + dx < height and 0 <= y + dy < width and maze[x + dx, y + dy] == 1
        ]

        if neighbors:
            # choose a random neighbor
            nextX, nextY = random.choice(neighbors)

            # remove the wall between the current cell and the chosen neighbor
            maze[(x + nextX) // 2, (y + nextY) // 2] = 0
            # mark the neighbor cell as part of the maze
            maze[nextX, nextY] = 0

            # yield state after removing the wall
            yield maze.copy(), ((x + nextX) // 2, (y + nextY) // 2)
            # yield state after moving to the neighbor
            yield maze.copy(), (nextX, nextY)

            # move to the chosen neighbor
            x, y = nextX, nextY
        else:
            # HUNTING phase: look for an unvisited cell with a visited neighbor
            found = False
            for huntX in range(0, height, 2):
                for huntY in range(0, width, 2):
                    # unvisited cell found
                    if maze[huntX, huntY] == 1:
                        # check if it has any visited neighbors
                        neighbors = [
                            (huntX + dx, huntY + dy)
                            for dx, dy in [(-2, 0), (2, 0), (0, -2), (0, 2)]
                            if 0 <= huntX + dx < height and 0 <= huntY + dy < width and maze[huntX + dx, huntY + dy] == 0
                        ]
                        if neighbors:
                            # connect the unvisited cell to a visited neighbor
                            maze[huntX, huntY] = 0
                            nextX, nextY = random.choice(neighbors)
                            maze[(huntX + nextX) // 2, (huntY + nextY) // 2] = 0

                            # yield state after connecting to visited cell
                            yield maze.copy(), (huntX, huntY)
                            yield maze.copy(), ((huntX + nextX) // 2, (huntY + nextY) // 2)

                            x, y = huntX, huntY
                            found = True
                            break
                if found:
                    break
            if not found:
                break

def generateMazeHuntAndKill(height, width):
    maze = np.ones((height, width), dtype=np.uint8)
    return huntAndKill(maze)

