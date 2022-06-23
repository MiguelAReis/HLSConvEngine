
log_wave -r /
set designtopgroup [add_wave_group "Design Top Signals"]
set coutputgroup [add_wave_group "C Outputs" -into $designtopgroup]
set strm_out_group [add_wave_group strm_out(axis) -into $coutputgroup]
add_wave /apatb_depthwise_top/AESL_inst_depthwise/strm_out_TLAST -into $strm_out_group -color #ffff00 -radix hex
add_wave /apatb_depthwise_top/AESL_inst_depthwise/strm_out_TSTRB -into $strm_out_group -radix hex
add_wave /apatb_depthwise_top/AESL_inst_depthwise/strm_out_TKEEP -into $strm_out_group -radix hex
add_wave /apatb_depthwise_top/AESL_inst_depthwise/strm_out_TREADY -into $strm_out_group -color #ffff00 -radix hex
add_wave /apatb_depthwise_top/AESL_inst_depthwise/strm_out_TVALID -into $strm_out_group -color #ffff00 -radix hex
add_wave /apatb_depthwise_top/AESL_inst_depthwise/strm_out_TDATA -into $strm_out_group -radix hex
set cinputgroup [add_wave_group "C Inputs" -into $designtopgroup]
set relu_group [add_wave_group relu(wire) -into $cinputgroup]
add_wave /apatb_depthwise_top/AESL_inst_depthwise/relu -into $relu_group -radix hex
set mapSizeY_group [add_wave_group mapSizeY(wire) -into $cinputgroup]
add_wave /apatb_depthwise_top/AESL_inst_depthwise/mapSizeY -into $mapSizeY_group -radix hex
set mapSizeX_group [add_wave_group mapSizeX(wire) -into $cinputgroup]
add_wave /apatb_depthwise_top/AESL_inst_depthwise/mapSizeX -into $mapSizeX_group -radix hex
set kernelSize_group [add_wave_group kernelSize(wire) -into $cinputgroup]
add_wave /apatb_depthwise_top/AESL_inst_depthwise/kernelSize -into $kernelSize_group -radix hex
set kernelN_group [add_wave_group kernelN(wire) -into $cinputgroup]
add_wave /apatb_depthwise_top/AESL_inst_depthwise/kernelN -into $kernelN_group -radix hex
set strm_in_group [add_wave_group strm_in(axis) -into $cinputgroup]
add_wave /apatb_depthwise_top/AESL_inst_depthwise/strm_in_TLAST -into $strm_in_group -color #ffff00 -radix hex
add_wave /apatb_depthwise_top/AESL_inst_depthwise/strm_in_TSTRB -into $strm_in_group -radix hex
add_wave /apatb_depthwise_top/AESL_inst_depthwise/strm_in_TKEEP -into $strm_in_group -radix hex
add_wave /apatb_depthwise_top/AESL_inst_depthwise/strm_in_TREADY -into $strm_in_group -color #ffff00 -radix hex
add_wave /apatb_depthwise_top/AESL_inst_depthwise/strm_in_TVALID -into $strm_in_group -color #ffff00 -radix hex
add_wave /apatb_depthwise_top/AESL_inst_depthwise/strm_in_TDATA -into $strm_in_group -radix hex
set blocksiggroup [add_wave_group "Block-level IO Handshake" -into $designtopgroup]
add_wave /apatb_depthwise_top/AESL_inst_depthwise/ap_start -into $blocksiggroup
add_wave /apatb_depthwise_top/AESL_inst_depthwise/ap_done -into $blocksiggroup
add_wave /apatb_depthwise_top/AESL_inst_depthwise/ap_idle -into $blocksiggroup
add_wave /apatb_depthwise_top/AESL_inst_depthwise/ap_ready -into $blocksiggroup
set resetgroup [add_wave_group "Reset" -into $designtopgroup]
add_wave /apatb_depthwise_top/AESL_inst_depthwise/ap_rst_n -into $resetgroup
set clockgroup [add_wave_group "Clock" -into $designtopgroup]
add_wave /apatb_depthwise_top/AESL_inst_depthwise/ap_clk -into $clockgroup
set testbenchgroup [add_wave_group "Test Bench Signals"]
set tbinternalsiggroup [add_wave_group "Internal Signals" -into $testbenchgroup]
set tb_simstatus_group [add_wave_group "Simulation Status" -into $tbinternalsiggroup]
set tb_portdepth_group [add_wave_group "Port Depth" -into $tbinternalsiggroup]
add_wave /apatb_depthwise_top/AUTOTB_TRANSACTION_NUM -into $tb_simstatus_group -radix hex
add_wave /apatb_depthwise_top/ready_cnt -into $tb_simstatus_group -radix hex
add_wave /apatb_depthwise_top/done_cnt -into $tb_simstatus_group -radix hex
add_wave /apatb_depthwise_top/LENGTH_strm_in_V_data_V -into $tb_portdepth_group -radix hex
add_wave /apatb_depthwise_top/LENGTH_strm_in_V_keep_V -into $tb_portdepth_group -radix hex
add_wave /apatb_depthwise_top/LENGTH_strm_in_V_strb_V -into $tb_portdepth_group -radix hex
add_wave /apatb_depthwise_top/LENGTH_strm_in_V_last_V -into $tb_portdepth_group -radix hex
add_wave /apatb_depthwise_top/LENGTH_strm_out_V_data_V -into $tb_portdepth_group -radix hex
add_wave /apatb_depthwise_top/LENGTH_strm_out_V_keep_V -into $tb_portdepth_group -radix hex
add_wave /apatb_depthwise_top/LENGTH_strm_out_V_strb_V -into $tb_portdepth_group -radix hex
add_wave /apatb_depthwise_top/LENGTH_strm_out_V_last_V -into $tb_portdepth_group -radix hex
add_wave /apatb_depthwise_top/LENGTH_kernelN -into $tb_portdepth_group -radix hex
add_wave /apatb_depthwise_top/LENGTH_kernelSize -into $tb_portdepth_group -radix hex
add_wave /apatb_depthwise_top/LENGTH_mapSizeX -into $tb_portdepth_group -radix hex
add_wave /apatb_depthwise_top/LENGTH_mapSizeY -into $tb_portdepth_group -radix hex
add_wave /apatb_depthwise_top/LENGTH_relu -into $tb_portdepth_group -radix hex
set tbcoutputgroup [add_wave_group "C Outputs" -into $testbenchgroup]
set tb_strm_out_group [add_wave_group strm_out(axis) -into $tbcoutputgroup]
add_wave /apatb_depthwise_top/strm_out_TLAST -into $tb_strm_out_group -color #ffff00 -radix hex
add_wave /apatb_depthwise_top/strm_out_TSTRB -into $tb_strm_out_group -radix hex
add_wave /apatb_depthwise_top/strm_out_TKEEP -into $tb_strm_out_group -radix hex
add_wave /apatb_depthwise_top/strm_out_TREADY -into $tb_strm_out_group -color #ffff00 -radix hex
add_wave /apatb_depthwise_top/strm_out_TVALID -into $tb_strm_out_group -color #ffff00 -radix hex
add_wave /apatb_depthwise_top/strm_out_TDATA -into $tb_strm_out_group -radix hex
set tbcinputgroup [add_wave_group "C Inputs" -into $testbenchgroup]
set tb_relu_group [add_wave_group relu(wire) -into $tbcinputgroup]
add_wave /apatb_depthwise_top/relu -into $tb_relu_group -radix hex
set tb_mapSizeY_group [add_wave_group mapSizeY(wire) -into $tbcinputgroup]
add_wave /apatb_depthwise_top/mapSizeY -into $tb_mapSizeY_group -radix hex
set tb_mapSizeX_group [add_wave_group mapSizeX(wire) -into $tbcinputgroup]
add_wave /apatb_depthwise_top/mapSizeX -into $tb_mapSizeX_group -radix hex
set tb_kernelSize_group [add_wave_group kernelSize(wire) -into $tbcinputgroup]
add_wave /apatb_depthwise_top/kernelSize -into $tb_kernelSize_group -radix hex
set tb_kernelN_group [add_wave_group kernelN(wire) -into $tbcinputgroup]
add_wave /apatb_depthwise_top/kernelN -into $tb_kernelN_group -radix hex
set tb_strm_in_group [add_wave_group strm_in(axis) -into $tbcinputgroup]
add_wave /apatb_depthwise_top/strm_in_TLAST -into $tb_strm_in_group -color #ffff00 -radix hex
add_wave /apatb_depthwise_top/strm_in_TSTRB -into $tb_strm_in_group -radix hex
add_wave /apatb_depthwise_top/strm_in_TKEEP -into $tb_strm_in_group -radix hex
add_wave /apatb_depthwise_top/strm_in_TREADY -into $tb_strm_in_group -color #ffff00 -radix hex
add_wave /apatb_depthwise_top/strm_in_TVALID -into $tb_strm_in_group -color #ffff00 -radix hex
add_wave /apatb_depthwise_top/strm_in_TDATA -into $tb_strm_in_group -radix hex
save_wave_config depthwise.wcfg
run all

