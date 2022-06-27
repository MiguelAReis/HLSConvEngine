#include "hls_design_meta.h"
const Port_Property HLS_Design_Meta::port_props[]={
	Port_Property("ap_clk", 1, hls_in, -1, "", "", 1),
	Port_Property("ap_rst_n", 1, hls_in, -1, "", "", 1),
	Port_Property("strm_in_TDATA", 32, hls_in, 0, "axis", "in_data", 1),
	Port_Property("strm_in_TVALID", 1, hls_in, 3, "axis", "in_vld", 1),
	Port_Property("strm_in_TREADY", 1, hls_out, 3, "axis", "in_acc", 1),
	Port_Property("strm_in_TKEEP", 4, hls_in, 1, "axis", "in_data", 1),
	Port_Property("strm_in_TSTRB", 4, hls_in, 2, "axis", "in_data", 1),
	Port_Property("strm_in_TLAST", 1, hls_in, 3, "axis", "in_data", 1),
	Port_Property("strm_out_TDATA", 32, hls_out, 4, "axis", "out_data", 1),
	Port_Property("strm_out_TVALID", 1, hls_out, 7, "axis", "out_vld", 1),
	Port_Property("strm_out_TREADY", 1, hls_in, 7, "axis", "out_acc", 1),
	Port_Property("strm_out_TKEEP", 4, hls_out, 5, "axis", "out_data", 1),
	Port_Property("strm_out_TSTRB", 4, hls_out, 6, "axis", "out_data", 1),
	Port_Property("strm_out_TLAST", 1, hls_out, 7, "axis", "out_data", 1),
	Port_Property("filterN", 32, hls_in, 8, "ap_none", "in_data", 1),
	Port_Property("kernelN", 32, hls_in, 9, "ap_none", "in_data", 1),
	Port_Property("kernelSize", 32, hls_in, 10, "ap_none", "in_data", 1),
	Port_Property("mapSizeX", 32, hls_in, 11, "ap_none", "in_data", 1),
	Port_Property("mapSizeY", 32, hls_in, 12, "ap_none", "in_data", 1),
	Port_Property("relu", 1, hls_in, 13, "ap_none", "in_data", 1),
};
const char* HLS_Design_Meta::dut_name = "conv";
