// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================

extern "C" void AESL_WRAP_depthwise (
hls::stream<struct ap_axis<32, 0, 0, 0 > > (&strm_in),
hls::stream<struct ap_axis<32, 0, 0, 0 > > (&strm_out),
int kernelN,
int kernelSize,
int mapSizeX,
int mapSizeY,
char relu);
