############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project Engine
add_files Sources/engineParameters.h
add_files Sources/main.cpp
add_files -tb Sources/tb_main.cpp
open_solution "solution1" -flow_target vivado
set_part {xc7z020clg400-1}
create_clock -period 10 -name default
#source "./Engine/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
