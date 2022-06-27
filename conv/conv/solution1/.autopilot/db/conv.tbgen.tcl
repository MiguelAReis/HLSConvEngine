set moduleName conv
set isTopModule 1
set isCombinational 0
set isDatapathOnly 0
set isPipelined 0
set pipeline_type none
set FunctionProtocol ap_ctrl_none
set isOneStateSeq 0
set ProfileFlag 0
set StallSigGenFlag 0
set isEnableWaveformDebug 1
set C_modelName {conv}
set C_modelType { void 0 }
set C_modelArgList {
	{ strm_in_V_data_V int 32 regular {axi_s 0 volatile  { strm_in Data } }  }
	{ strm_in_V_keep_V int 4 regular {axi_s 0 volatile  { strm_in Keep } }  }
	{ strm_in_V_strb_V int 4 regular {axi_s 0 volatile  { strm_in Strb } }  }
	{ strm_in_V_last_V int 1 regular {axi_s 0 volatile  { strm_in Last } }  }
	{ strm_out_V_data_V int 32 regular {axi_s 1 volatile  { strm_out Data } }  }
	{ strm_out_V_keep_V int 4 regular {axi_s 1 volatile  { strm_out Keep } }  }
	{ strm_out_V_strb_V int 4 regular {axi_s 1 volatile  { strm_out Strb } }  }
	{ strm_out_V_last_V int 1 regular {axi_s 1 volatile  { strm_out Last } }  }
	{ filterN int 32 regular  }
	{ kernelN int 32 regular  }
	{ kernelSize int 32 regular  }
	{ mapSizeX int 32 regular  }
	{ mapSizeY int 32 regular  }
	{ relu uint 1 regular  }
}
set C_modelArgMapList {[ 
	{ "Name" : "strm_in_V_data_V", "interface" : "axis", "bitwidth" : 32, "direction" : "READONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "strm_in.V.data.V","cData": "int","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} , 
 	{ "Name" : "strm_in_V_keep_V", "interface" : "axis", "bitwidth" : 4, "direction" : "READONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "strm_in.V.keep.V","cData": "int4","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} , 
 	{ "Name" : "strm_in_V_strb_V", "interface" : "axis", "bitwidth" : 4, "direction" : "READONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "strm_in.V.strb.V","cData": "int4","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} , 
 	{ "Name" : "strm_in_V_last_V", "interface" : "axis", "bitwidth" : 1, "direction" : "READONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "strm_in.V.last.V","cData": "int1","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} , 
 	{ "Name" : "strm_out_V_data_V", "interface" : "axis", "bitwidth" : 32, "direction" : "WRITEONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "strm_out.V.data.V","cData": "int","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} , 
 	{ "Name" : "strm_out_V_keep_V", "interface" : "axis", "bitwidth" : 4, "direction" : "WRITEONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "strm_out.V.keep.V","cData": "int4","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} , 
 	{ "Name" : "strm_out_V_strb_V", "interface" : "axis", "bitwidth" : 4, "direction" : "WRITEONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "strm_out.V.strb.V","cData": "int4","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} , 
 	{ "Name" : "strm_out_V_last_V", "interface" : "axis", "bitwidth" : 1, "direction" : "WRITEONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "strm_out.V.last.V","cData": "int1","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} , 
 	{ "Name" : "filterN", "interface" : "wire", "bitwidth" : 32, "direction" : "READONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "filterN","cData": "int","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} , 
 	{ "Name" : "kernelN", "interface" : "wire", "bitwidth" : 32, "direction" : "READONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "kernelN","cData": "int","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} , 
 	{ "Name" : "kernelSize", "interface" : "wire", "bitwidth" : 32, "direction" : "READONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "kernelSize","cData": "int","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} , 
 	{ "Name" : "mapSizeX", "interface" : "wire", "bitwidth" : 32, "direction" : "READONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "mapSizeX","cData": "int","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} , 
 	{ "Name" : "mapSizeY", "interface" : "wire", "bitwidth" : 32, "direction" : "READONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "mapSizeY","cData": "int","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} , 
 	{ "Name" : "relu", "interface" : "wire", "bitwidth" : 1, "direction" : "READONLY", "bitSlice":[{"low":0,"up":0,"cElement": [{"cName": "relu","cData": "int1","bit_use": { "low": 0,"up": 0},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}]} ]}
# RTL Port declarations: 
set portNum 20
set portList { 
	{ ap_clk sc_in sc_logic 1 clock -1 } 
	{ ap_rst_n sc_in sc_logic 1 reset -1 active_low_sync } 
	{ strm_in_TDATA sc_in sc_lv 32 signal 0 } 
	{ strm_in_TVALID sc_in sc_logic 1 invld 3 } 
	{ strm_in_TREADY sc_out sc_logic 1 inacc 3 } 
	{ strm_in_TKEEP sc_in sc_lv 4 signal 1 } 
	{ strm_in_TSTRB sc_in sc_lv 4 signal 2 } 
	{ strm_in_TLAST sc_in sc_lv 1 signal 3 } 
	{ strm_out_TDATA sc_out sc_lv 32 signal 4 } 
	{ strm_out_TVALID sc_out sc_logic 1 outvld 7 } 
	{ strm_out_TREADY sc_in sc_logic 1 outacc 7 } 
	{ strm_out_TKEEP sc_out sc_lv 4 signal 5 } 
	{ strm_out_TSTRB sc_out sc_lv 4 signal 6 } 
	{ strm_out_TLAST sc_out sc_lv 1 signal 7 } 
	{ filterN sc_in sc_lv 32 signal 8 } 
	{ kernelN sc_in sc_lv 32 signal 9 } 
	{ kernelSize sc_in sc_lv 32 signal 10 } 
	{ mapSizeX sc_in sc_lv 32 signal 11 } 
	{ mapSizeY sc_in sc_lv 32 signal 12 } 
	{ relu sc_in sc_logic 1 signal 13 } 
}
set NewPortList {[ 
	{ "name": "ap_clk", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "clock", "bundle":{"name": "ap_clk", "role": "default" }} , 
 	{ "name": "ap_rst_n", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "reset", "bundle":{"name": "ap_rst_n", "role": "default" }} , 
 	{ "name": "strm_in_TDATA", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "strm_in_V_data_V", "role": "default" }} , 
 	{ "name": "strm_in_TVALID", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "invld", "bundle":{"name": "strm_in_V_last_V", "role": "default" }} , 
 	{ "name": "strm_in_TREADY", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "inacc", "bundle":{"name": "strm_in_V_last_V", "role": "default" }} , 
 	{ "name": "strm_in_TKEEP", "direction": "in", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "strm_in_V_keep_V", "role": "default" }} , 
 	{ "name": "strm_in_TSTRB", "direction": "in", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "strm_in_V_strb_V", "role": "default" }} , 
 	{ "name": "strm_in_TLAST", "direction": "in", "datatype": "sc_lv", "bitwidth":1, "type": "signal", "bundle":{"name": "strm_in_V_last_V", "role": "default" }} , 
 	{ "name": "strm_out_TDATA", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "strm_out_V_data_V", "role": "default" }} , 
 	{ "name": "strm_out_TVALID", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "outvld", "bundle":{"name": "strm_out_V_last_V", "role": "default" }} , 
 	{ "name": "strm_out_TREADY", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "outacc", "bundle":{"name": "strm_out_V_last_V", "role": "default" }} , 
 	{ "name": "strm_out_TKEEP", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "strm_out_V_keep_V", "role": "default" }} , 
 	{ "name": "strm_out_TSTRB", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "strm_out_V_strb_V", "role": "default" }} , 
 	{ "name": "strm_out_TLAST", "direction": "out", "datatype": "sc_lv", "bitwidth":1, "type": "signal", "bundle":{"name": "strm_out_V_last_V", "role": "default" }} , 
 	{ "name": "filterN", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "filterN", "role": "default" }} , 
 	{ "name": "kernelN", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "kernelN", "role": "default" }} , 
 	{ "name": "kernelSize", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "kernelSize", "role": "default" }} , 
 	{ "name": "mapSizeX", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "mapSizeX", "role": "default" }} , 
 	{ "name": "mapSizeY", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "mapSizeY", "role": "default" }} , 
 	{ "name": "relu", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "relu", "role": "default" }}  ]}

set RtlHierarchyInfo {[
	{"ID" : "0", "Level" : "0", "Path" : "`AUTOTB_DUT_INST", "Parent" : "", "Child" : ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19"],
		"CDFG" : "conv",
		"Protocol" : "ap_ctrl_none",
		"ControlExist" : "0", "ap_start" : "0", "ap_ready" : "0", "ap_done" : "0", "ap_continue" : "0", "ap_idle" : "0", "real_start" : "0",
		"Pipeline" : "None", "UnalignedPipeline" : "0", "RewindPipeline" : "0", "ProcessNetwork" : "0",
		"II" : "0",
		"VariableLatency" : "1", "ExactLatency" : "-1", "EstimateLatencyMin" : "8408", "EstimateLatencyMax" : "8408",
		"Combinational" : "0",
		"Datapath" : "0",
		"ClockEnable" : "0",
		"HasSubDataflow" : "0",
		"InDataflowNetwork" : "0",
		"HasNonBlockingOperation" : "0",
		"Port" : [
			{"Name" : "strm_in_V_data_V", "Type" : "Axis", "Direction" : "I",
				"BlockSignal" : [
					{"Name" : "strm_in_TDATA_blk_n", "Type" : "RtlSignal"}]},
			{"Name" : "strm_in_V_keep_V", "Type" : "Axis", "Direction" : "I"},
			{"Name" : "strm_in_V_strb_V", "Type" : "Axis", "Direction" : "I"},
			{"Name" : "strm_in_V_last_V", "Type" : "Axis", "Direction" : "I"},
			{"Name" : "strm_out_V_data_V", "Type" : "Axis", "Direction" : "O",
				"BlockSignal" : [
					{"Name" : "strm_out_TDATA_blk_n", "Type" : "RtlSignal"}]},
			{"Name" : "strm_out_V_keep_V", "Type" : "Axis", "Direction" : "O"},
			{"Name" : "strm_out_V_strb_V", "Type" : "Axis", "Direction" : "O"},
			{"Name" : "strm_out_V_last_V", "Type" : "Axis", "Direction" : "O"},
			{"Name" : "filterN", "Type" : "None", "Direction" : "I"},
			{"Name" : "kernelN", "Type" : "None", "Direction" : "I"},
			{"Name" : "kernelSize", "Type" : "None", "Direction" : "I"},
			{"Name" : "mapSizeX", "Type" : "None", "Direction" : "I"},
			{"Name" : "mapSizeY", "Type" : "None", "Direction" : "I"},
			{"Name" : "relu", "Type" : "None", "Direction" : "I"}]},
	{"ID" : "1", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.filter_V_U", "Parent" : "0"},
	{"ID" : "2", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.featureMap_V_U", "Parent" : "0"},
	{"ID" : "3", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.mul_7s_7s_7_1_1_U1", "Parent" : "0"},
	{"ID" : "4", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.mul_32s_32s_64_2_1_U2", "Parent" : "0"},
	{"ID" : "5", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.mul_mul_11s_11s_11_4_1_U3", "Parent" : "0"},
	{"ID" : "6", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.ama_addmuladd_11s_2ns_11s_11ns_11_4_1_U4", "Parent" : "0"},
	{"ID" : "7", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.mul_mul_11s_11s_11_4_1_U5", "Parent" : "0"},
	{"ID" : "8", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.mac_muladd_7s_7s_7ns_7_4_1_U6", "Parent" : "0"},
	{"ID" : "9", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.ama_addmuladd_11s_2ns_11s_11ns_11_4_1_U7", "Parent" : "0"},
	{"ID" : "10", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.ama_addmuladd_11s_2ns_11s_11ns_11_4_1_U8", "Parent" : "0"},
	{"ID" : "11", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.ama_addmuladd_7ns_7ns_7s_7ns_7_4_1_U9", "Parent" : "0"},
	{"ID" : "12", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.regslice_both_strm_in_V_data_V_U", "Parent" : "0"},
	{"ID" : "13", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.regslice_both_strm_in_V_keep_V_U", "Parent" : "0"},
	{"ID" : "14", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.regslice_both_strm_in_V_strb_V_U", "Parent" : "0"},
	{"ID" : "15", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.regslice_both_strm_in_V_last_V_U", "Parent" : "0"},
	{"ID" : "16", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.regslice_both_strm_out_V_data_V_U", "Parent" : "0"},
	{"ID" : "17", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.regslice_both_strm_out_V_keep_V_U", "Parent" : "0"},
	{"ID" : "18", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.regslice_both_strm_out_V_strb_V_U", "Parent" : "0"},
	{"ID" : "19", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.regslice_both_strm_out_V_last_V_U", "Parent" : "0"}]}


set ArgLastReadFirstWriteLatency {
	conv {
		strm_in_V_data_V {Type I LastRead 8 FirstWrite -1}
		strm_in_V_keep_V {Type I LastRead 8 FirstWrite -1}
		strm_in_V_strb_V {Type I LastRead 8 FirstWrite -1}
		strm_in_V_last_V {Type I LastRead 8 FirstWrite -1}
		strm_out_V_data_V {Type O LastRead -1 FirstWrite 19}
		strm_out_V_keep_V {Type O LastRead -1 FirstWrite 19}
		strm_out_V_strb_V {Type O LastRead -1 FirstWrite 19}
		strm_out_V_last_V {Type O LastRead -1 FirstWrite 19}
		filterN {Type I LastRead 0 FirstWrite -1}
		kernelN {Type I LastRead 0 FirstWrite -1}
		kernelSize {Type I LastRead 0 FirstWrite -1}
		mapSizeX {Type I LastRead 0 FirstWrite -1}
		mapSizeY {Type I LastRead 0 FirstWrite -1}
		relu {Type I LastRead 0 FirstWrite -1}}}

set hasDtUnsupportedChannel 0

set PerformanceInfo {[
	{"Name" : "Latency", "Min" : "8408", "Max" : "8408"}
	, {"Name" : "Interval", "Min" : "8409", "Max" : "8409"}
]}

set PipelineEnableSignalInfo {[
	{"Pipeline" : "1", "EnableSignal" : "ap_enable_pp1"}
	{"Pipeline" : "2", "EnableSignal" : "ap_enable_pp2"}
]}

set Spec2ImplPortList { 
	strm_in_V_data_V { axis {  { strm_in_TDATA in_data 0 32 } } }
	strm_in_V_keep_V { axis {  { strm_in_TKEEP in_data 0 4 } } }
	strm_in_V_strb_V { axis {  { strm_in_TSTRB in_data 0 4 } } }
	strm_in_V_last_V { axis {  { strm_in_TVALID in_vld 0 1 }  { strm_in_TREADY in_acc 1 1 }  { strm_in_TLAST in_data 0 1 } } }
	strm_out_V_data_V { axis {  { strm_out_TDATA out_data 1 32 } } }
	strm_out_V_keep_V { axis {  { strm_out_TKEEP out_data 1 4 } } }
	strm_out_V_strb_V { axis {  { strm_out_TSTRB out_data 1 4 } } }
	strm_out_V_last_V { axis {  { strm_out_TVALID out_vld 1 1 }  { strm_out_TREADY out_acc 0 1 }  { strm_out_TLAST out_data 1 1 } } }
	filterN { ap_none {  { filterN in_data 0 32 } } }
	kernelN { ap_none {  { kernelN in_data 0 32 } } }
	kernelSize { ap_none {  { kernelSize in_data 0 32 } } }
	mapSizeX { ap_none {  { mapSizeX in_data 0 32 } } }
	mapSizeY { ap_none {  { mapSizeY in_data 0 32 } } }
	relu { ap_none {  { relu in_data 0 1 } } }
}

set busDeadlockParameterList { 
}

# RTL port scheduling information:
set fifoSchedulingInfoList { 
}

# RTL bus port read request latency information:
set busReadReqLatencyList { 
}

# RTL bus port write response latency information:
set busWriteResLatencyList { 
}

# RTL array port load latency information:
set memoryLoadLatencyList { 
}
