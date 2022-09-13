Vitis HLS Engines

These Engines work with AXI Stream 64bit bus width

Conv: 

Convolution engine with (at compilation time) variable bit-widths for weights,activations and bias.

Parameters:

Number of Filters, Number of Channels, Map Size, Kernel Size, Stride, Padding, Scale

Depthwise: 

Depthwise Convolution engine for 3x3 Kernel with (at compilation time) variable bit-widths for weights,activations and bias .

Parameters:

Number of Filters, Number of Channels, Map Size, Stride, Padding, Scale

PreProcessing:

Engine capable of averaging a map to a 1x1xn Map

PostProcessing:

Engine capable of adding 2 maps and interpolating one map to a bigger size using bilinear interpolation.
