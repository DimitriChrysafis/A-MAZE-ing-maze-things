#include <metal_stdlib>
using namespace metal;

kernel void maze_kernel(device uchar *input [[ buffer(0) ]],
                        device uchar *output [[ buffer(1) ]],
                        uint2 gid [[ thread_position_in_grid ]]) {
    uint x = gid.x;
    uint y = gid.y;
    uchar value = input[y * {WIDTH} + x];
    output[y * {WIDTH} + x] = value;
}
