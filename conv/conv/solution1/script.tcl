############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project conv
set_top conv
add_files ../Sources/conv/conv.cpp
add_files ../Sources/conv/convParameters.h
add_files -tb ../Sources/conv/tb_conv.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1" -flow_target vivado
set_part {xc7z020-clg400-1}
create_clock -period 20 -name default
set_clock_uncertainty 1.25
source "./conv/solution1/directives.tcl"
csim_design -profile
csynth_design
cosim_design
export_design -format ip_catalog
