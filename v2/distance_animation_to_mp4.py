import imageio
import os
from pathlib import Path
import numpy as np

ImageDirectory = 'distance_animation'
OutputVideoPath = 'output_video.mp4'
FrameRate = 24

def GetImageFiles(directory):
    Files = os.listdir(directory)
    PngFiles = [f for f in Files if f.endswith('.png')]
    PngFiles.sort(key=lambda x: int(Path(x).stem))
    return PngFiles

def EnsureColorImage(image):
    if len(image.shape) == 2:
        return np.stack([image] * 3, axis=-1)
    return image

def CreateVideoFromImages(imageFiles, outputFile, frameRate):
    if not imageFiles:
        print("No images found to create the video.")
        return

    firstImagePath = os.path.join(ImageDirectory, imageFiles[0])
    firstImage = imageio.imread(firstImagePath)
    firstImage = EnsureColorImage(firstImage)
    height, width, layers = firstImage.shape

    Writer = imageio.get_writer(outputFile, format='mp4', fps=frameRate)

    for imageFile in imageFiles:
        imagePath = os.path.join(ImageDirectory, imageFile)
        frame = imageio.imread(imagePath)
        frame = EnsureColorImage(frame)
        if frame.shape[:2] != (height, width):
            print(f"Warning: {imageFile} size mismatch.")
            continue
        Writer.append_data(frame)

    Writer.close()
    print(f"{outputFile}")

def Main():
    imageFiles = GetImageFiles(ImageDirectory)
    CreateVideoFromImages(imageFiles, OutputVideoPath, FrameRate)

if __name__ == "__main__":
    Main()