############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project depthwise
set_top depthwise
add_files ../Sources/depthwise/depthwiseParameters.h
add_files ../Sources/depthwise/depthwise.cpp
add_files -tb ../Sources/depthwise/tb_depthwise.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution" -flow_target vivado
set_part {xc7z020-clg400-1}
create_clock -period 10 -name default
source "./depthwise/solution/directives.tcl"
cosim_design -wave_debug -enable_dataflow_profiling -trace_level all
