import os
import time
import imageio
import numpy as np
import prims
import render
import dfs
import hunt_and_kill
import kruskals
from plot_render_times import readfrominput, actyuallyplothtem

#for the prints that i have to show the percentage that we've rendered so far
def countTotalIterations(mazeGenerate, height, width):
    return sum(1 for _ in mazeGenerate(height, width))

#make video
def saveAndGo(generateMaze, height, width, filename, interval=1, scale=10):
    render.CLEARIMAGESFOLDER('temp_images')
    numOfFrames = 0

    totalSteps = countTotalIterations(generateMaze, height, width)
    totalFrames = (totalSteps + interval - 1) // interval
    print(f"TOTAL frames to be produced: {totalFrames}")

    writer = imageio.get_writer(filename, fps=20, codec='libx264')

    for i, (maze, changedCell) in enumerate(generateMaze(height, width)):
        if i % interval == 0:
            img = render.actuallyRenderMaze(maze, [changedCell], scale)
            writer.append_data(img)
            numOfFrames += 1
            progress = (numOfFrames / totalFrames) * 100
            print(f"Rendered frame {numOfFrames}/{totalFrames} ({progress:.2f}%)")

    writer.close()
    print("Rendering completed.")

def actuallyGenerate(name, height, width, frameInterval=10):
    algorithms = {
        'prims': prims.generateMazePrims,
        'dfs': dfs.generateMazeDfs,
        'hunt_and_kill': hunt_and_kill.generateMazeHuntAndKill,
        'kruskals': kruskals.generateMazeKruskals
    }

    algorithm = algorithms[name]
    folder = f'videos/{name}'

    if not os.path.exists(folder):
        os.makedirs(folder)

    path = os.path.join(folder, f'{height}x{width}.mp4')

    print(f"Generating maze using {name} algorithm for size {height}x{width}...")

    startTime = time.time()
    saveAndGo(algorithm, height, width, path, frameInterval)
    endTime = time.time()

    duration = endTime - startTime
    print(f"Video saved as {path}. Time taken: {duration:.2f} seconds.")

    return duration

def main():
     #actuallyGenerate('prims', 1500, 1500, frameInterval=15000)

    algorithms = ['hunt_and_kill', 'kruskals', 'dfs', 'prims']
    results = []

    for name in algorithms:
        for size in range(1, 201):
            duration = actuallyGenerate(name, size, size)
            results.append(f'{name},{size},{size},{duration:.2f}\n')

    with open('render_times.txt', 'w') as f:
        f.writelines(results)


    print("Generating plots...")
    data = readfrominput('render_times.txt')
    actyuallyplothtem(data)
    print("Plots have been saved.")

if __name__ == "__main__":
    main()

