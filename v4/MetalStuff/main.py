import subprocess
import Metal
import ctypes
import numpy as np
import cv2
import time

binfile = '/Users/dimitrichrysafis/desktop/maze.bin'
exec = '/Users/dimitrichrysafis/CLionProjects/Minimaze/cmake-build-debug/Minimaze'


def metalstuff(fileOfTheMaze, widthOfTheMaze, heightOfTheMaze):
    kernelSourceFile = 'maze_kernel.metal'
    with open(kernelSourceFile, 'r') as kernelFile:
        kernelSourceCode = kernelFile.read().replace("{WIDTH}", str(widthOfTheMaze))

    metalDevice = Metal.MTLCreateSystemDefaultDevice()
    metalLibrary = metalDevice.newLibraryWithSource_options_error_(kernelSourceCode, None, None)[0]
    metalKernelFunction = metalLibrary.newFunctionWithName_("maze_kernel")

    bufferLength = widthOfTheMaze * heightOfTheMaze

    inputBuffer = metalDevice.newBufferWithLength_options_(bufferLength, Metal.MTLResourceStorageModeShared)
    outputBuffer = metalDevice.newBufferWithLength_options_(bufferLength, Metal.MTLResourceStorageModeShared)

    mazeArray = np.fromfile(fileOfTheMaze, dtype=np.uint8).reshape((heightOfTheMaze, widthOfTheMaze))
    inputDataBuffer = (ctypes.c_ubyte * (widthOfTheMaze * heightOfTheMaze)).from_buffer(inputBuffer.contents().as_buffer(bufferLength))
    inputDataBuffer[:] = mazeArray.flatten()

    renderStartTime = time.time()

    commandQueue = metalDevice.newCommandQueue()
    commandBuffer = commandQueue.commandBuffer()
    pipelineState = metalDevice.newComputePipelineStateWithFunction_error_(metalKernelFunction, None)[0]
    computeEncoder = commandBuffer.computeCommandEncoder()
    computeEncoder.setComputePipelineState_(pipelineState)
    computeEncoder.setBuffer_offset_atIndex_(inputBuffer, 0, 0)
    computeEncoder.setBuffer_offset_atIndex_(outputBuffer, 0, 1)

    threadsPerGrid = Metal.MTLSizeMake(widthOfTheMaze, heightOfTheMaze, 1)
    threadsPerThreadgroup = Metal.MTLSizeMake(16, 16, 1)

    computeEncoder.dispatchThreads_threadsPerThreadgroup_(threadsPerGrid, threadsPerThreadgroup)
    computeEncoder.endEncoding()
    commandBuffer.commit()
    commandBuffer.waitUntilCompleted()

    renderEndTime = time.time()
    print(f"Rendering time: {renderEndTime - renderStartTime:.2f} seconds")

    outputDataBuffer = (ctypes.c_ubyte * (widthOfTheMaze * heightOfTheMaze)).from_buffer(outputBuffer.contents().as_buffer(bufferLength))
    outputArray = np.ctypeslib.as_array(outputDataBuffer).reshape((heightOfTheMaze, widthOfTheMaze))

    cv2.imwrite(f"maze_{widthOfTheMaze}x{heightOfTheMaze}_color.png", outputArray)

    print(f"Maze image saved as 'maze_{widthOfTheMaze}x{heightOfTheMaze}_color.png'")

width = 1000
height = 1000

startTotalTime = time.time()

startGenTime = time.time()
subprocess.run([exec])
endGenTime = time.time()

print(f"Executable run time: {endGenTime - startGenTime:.2f} seconds")

metalstuff(binfile, width, height)

endTotalTime = time.time()
print(f"Total time taken: {endTotalTime - startTotalTime:.2f} seconds")
