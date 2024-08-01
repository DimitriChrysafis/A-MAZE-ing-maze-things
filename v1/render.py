import numpy as np
import cv2
import os


# Clear the images folder
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

    # precompute the mask for maze cells
    maze_mask = (maze == 1).astype(np.uint8)

    # upsample the maze mask using efficient array operations
    # create a larger array where each cell in the original maze is expanded by scale
    maze_img = np.repeat(np.repeat(maze_mask, scale, axis=0), scale, axis=1) * 255

    # assign the upsampled maze mask to the image
    img[:, :, 0] = maze_img
    img[:, :, 1] = maze_img
    img[:, :, 2] = maze_img

    if updatedcells:
        # convert updated cells list to NumPy array for faster indexing
        updatedcells = np.array(updatedcells)
        for (x, y) in updatedcells:
            top_left = (y * scale, x * scale)
            bottom_right = ((y + 1) * scale, (x + 1) * scale)
            # draw rectangles for updated cells
            cv2.rectangle(img, top_left, bottom_right, (0, 255, 0), -1)

    return img
