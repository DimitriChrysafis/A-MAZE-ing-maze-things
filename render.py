import os
import cv2
import numpy as np


# I used this to make sure it doesn't layer and stack so u get frames from previous runs
def CLEARIMAGESFOLDER(folder):
    if os.path.exists(folder):
        for file in os.listdir(folder):
            file_path = os.path.join(folder, file)
            if os.path.isfile(file_path):
                os.unlink(file_path)
    else:
        os.makedirs(folder)


def actuallyRenderMaze(maze, updatedcells=None, scale=10):
    height, width = maze.shape
    img = np.ones((height * scale, width * scale, 3), dtype=np.uint8) * 255

    # precompute rectangle points to minimize computation inside the loop
    rectangles = []
    for y in range(height):
        for x in range(width):
            if maze[y, x] == 1:
                rectangles.append(((x * scale, y * scale), ((x + 1) * scale, (y + 1) * scale)))

    # draw all rectangles at once
    for pt1, pt2 in rectangles:
        cv2.rectangle(img, pt1, pt2, (0, 0, 0), -1)

    if updatedcells:
        for (x, y) in updatedcells:
            cv2.rectangle(img, (y * scale, x * scale), ((y + 1) * scale, (x + 1) * scale), (0, 255, 0), -1)

    return img
