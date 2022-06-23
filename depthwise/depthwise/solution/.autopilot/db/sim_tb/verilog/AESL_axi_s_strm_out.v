// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================

`timescale 1 ns / 1 ps

`define TV_OUT_strm_out_TDATA "./impl_rtl.depthwise.autotvout_strm_out_V_data_V.dat"
`define EGRESS_STATUS_strm_out_TDATA "./stream_egress_status_strm_out_V_data_V.dat"
`define TV_OUT_strm_out_TKEEP "./impl_rtl.depthwise.autotvout_strm_out_V_keep_V.dat"
`define EGRESS_STATUS_strm_out_TKEEP "./stream_egress_status_strm_out_V_keep_V.dat"
`define TV_OUT_strm_out_TSTRB "./impl_rtl.depthwise.autotvout_strm_out_V_strb_V.dat"
`define EGRESS_STATUS_strm_out_TSTRB "./stream_egress_status_strm_out_V_strb_V.dat"
`define TV_OUT_strm_out_TLAST "./impl_rtl.depthwise.autotvout_strm_out_V_last_V.dat"
`define EGRESS_STATUS_strm_out_TLAST "./stream_egress_status_strm_out_V_last_V.dat"

`define AUTOTB_TRANSACTION_NUM 1

module AESL_axi_s_strm_out (
    input clk,
    input reset,
    input [32 - 1:0] TRAN_strm_out_TDATA,
    input [4 - 1:0] TRAN_strm_out_TKEEP,
    input [4 - 1:0] TRAN_strm_out_TSTRB,
    input TRAN_strm_out_TLAST,
    input TRAN_strm_out_TVALID,
    output TRAN_strm_out_TREADY,
    input ready,
    input done,
    output [31:0] transaction);

    wire TRAN_strm_out_TVALID_temp;
    wire strm_out_TDATA_full;
    wire strm_out_TDATA_empty;
    reg strm_out_TDATA_write_en;
    reg [32 - 1:0] strm_out_TDATA_write_data;
    reg strm_out_TDATA_read_en;
    wire [32 - 1:0] strm_out_TDATA_read_data;
    
    fifo #(27, 32) fifo_strm_out_TDATA (
        .reset(1'b0),
        .write_clock(clk),
        .write_en(strm_out_TDATA_write_en),
        .write_data(strm_out_TDATA_write_data),
        .read_clock(clk),
        .read_en(strm_out_TDATA_read_en),
        .read_data(strm_out_TDATA_read_data),
        .full(strm_out_TDATA_full),
        .empty(strm_out_TDATA_empty));
    
    always @ (*) begin
        strm_out_TDATA_write_en <= TRAN_strm_out_TVALID;
        strm_out_TDATA_write_data <= TRAN_strm_out_TDATA;
        strm_out_TDATA_read_en <= 0;
    end
    wire strm_out_TKEEP_full;
    wire strm_out_TKEEP_empty;
    reg strm_out_TKEEP_write_en;
    reg [4 - 1:0] strm_out_TKEEP_write_data;
    reg strm_out_TKEEP_read_en;
    wire [4 - 1:0] strm_out_TKEEP_read_data;
    
    fifo #(27, 4) fifo_strm_out_TKEEP (
        .reset(1'b0),
        .write_clock(clk),
        .write_en(strm_out_TKEEP_write_en),
        .write_data(strm_out_TKEEP_write_data),
        .read_clock(clk),
        .read_en(strm_out_TKEEP_read_en),
        .read_data(strm_out_TKEEP_read_data),
        .full(strm_out_TKEEP_full),
        .empty(strm_out_TKEEP_empty));
    
    always @ (*) begin
        strm_out_TKEEP_write_en <= TRAN_strm_out_TVALID;
        strm_out_TKEEP_write_data <= TRAN_strm_out_TKEEP;
        strm_out_TKEEP_read_en <= 0;
    end
    wire strm_out_TSTRB_full;
    wire strm_out_TSTRB_empty;
    reg strm_out_TSTRB_write_en;
    reg [4 - 1:0] strm_out_TSTRB_write_data;
    reg strm_out_TSTRB_read_en;
    wire [4 - 1:0] strm_out_TSTRB_read_data;
    
    fifo #(27, 4) fifo_strm_out_TSTRB (
        .reset(1'b0),
        .write_clock(clk),
        .write_en(strm_out_TSTRB_write_en),
        .write_data(strm_out_TSTRB_write_data),
        .read_clock(clk),
        .read_en(strm_out_TSTRB_read_en),
        .read_data(strm_out_TSTRB_read_data),
        .full(strm_out_TSTRB_full),
        .empty(strm_out_TSTRB_empty));
    
    always @ (*) begin
        strm_out_TSTRB_write_en <= TRAN_strm_out_TVALID;
        strm_out_TSTRB_write_data <= TRAN_strm_out_TSTRB;
        strm_out_TSTRB_read_en <= 0;
    end
    wire strm_out_TLAST_full;
    wire strm_out_TLAST_empty;
    reg strm_out_TLAST_write_en;
    reg [1 - 1:0] strm_out_TLAST_write_data;
    reg strm_out_TLAST_read_en;
    wire [1 - 1:0] strm_out_TLAST_read_data;
    
    fifo #(27, 1) fifo_strm_out_TLAST (
        .reset(1'b0),
        .write_clock(clk),
        .write_en(strm_out_TLAST_write_en),
        .write_data(strm_out_TLAST_write_data),
        .read_clock(clk),
        .read_en(strm_out_TLAST_read_en),
        .read_data(strm_out_TLAST_read_data),
        .full(strm_out_TLAST_full),
        .empty(strm_out_TLAST_empty));
    
    always @ (*) begin
        strm_out_TLAST_write_en <= TRAN_strm_out_TVALID;
        strm_out_TLAST_write_data <= TRAN_strm_out_TLAST;
        strm_out_TLAST_read_en <= 0;
    end
    assign TRAN_strm_out_TVALID = TRAN_strm_out_TVALID_temp;

    
    assign TRAN_strm_out_TREADY = ~(strm_out_TDATA_full || strm_out_TKEEP_full || strm_out_TSTRB_full || strm_out_TLAST_full);
    
    function is_blank_char(input [7:0] in_char);
        if (in_char == " " || in_char == "\011" || in_char == "\012" || in_char == "\015") begin
            is_blank_char = 1;
        end else begin
            is_blank_char = 0;
        end
    endfunction
    
    function [143:0] read_token(input integer fp);
        integer ret;
        begin
            read_token = "";
                    ret = 0;
                    ret = $fscanf(fp,"%s",read_token);
        end
    endfunction
    
    function [143:0] rm_0x(input [143:0] token);
        reg [143:0] token_tmp;
        integer i;
        begin
            token_tmp = "";
            for (i = 0; token[15:0] != "0x"; token = token >> 8) begin
                token_tmp = (token[7:0] << (8 * i)) | token_tmp;
                i = i + 1;
            end
            rm_0x = token_tmp;
        end
    endfunction
    
    reg done_1;
    
    always @ (posedge clk or reset) begin
        if (~reset) begin
            done_1 <= 0;
        end else begin
            done_1 <= done;
        end
    end
    
    reg [31:0] transaction_save_strm_out_TDATA;
    
    assign transaction = transaction_save_strm_out_TDATA;
    
    initial begin : AXI_stream_receiver_strm_out_TDATA
        integer fp;
        reg [32 - 1:0] data;
        reg [8 * 5:1] str;
        
        transaction_save_strm_out_TDATA = 0;
        fifo_strm_out_TDATA.clear();
        wait (reset === 1);
        forever begin
            @ (negedge clk);
            if (done_1 == 1) begin
                fp = $fopen(`TV_OUT_strm_out_TDATA, "a");
                if (fp == 0) begin // Failed to open file
                    $display("ERROR: Failed to open file \"%s\"!", `TV_OUT_strm_out_TDATA);
                    $finish;
                end
                $fdisplay(fp, "[[transaction]] %d", transaction_save_strm_out_TDATA);
                while (~fifo_strm_out_TDATA.empty) begin
                    fifo_strm_out_TDATA.pop(data);
                    $fdisplay(fp, "0x%x", data);
                end
                $fdisplay(fp, "[[/transaction]]");
                transaction_save_strm_out_TDATA = transaction_save_strm_out_TDATA + 1;
                fifo_strm_out_TDATA.clear();
                $fclose(fp);
            end
        end
    end
    
    reg [31:0] transaction_save_strm_out_TKEEP;
    
    initial begin : AXI_stream_receiver_strm_out_TKEEP
        integer fp;
        reg [4 - 1:0] data;
        reg [8 * 5:1] str;
        
        transaction_save_strm_out_TKEEP = 0;
        fifo_strm_out_TKEEP.clear();
        wait (reset === 1);
        forever begin
            @ (negedge clk);
            if (done_1 == 1) begin
                fp = $fopen(`TV_OUT_strm_out_TKEEP, "a");
                if (fp == 0) begin // Failed to open file
                    $display("ERROR: Failed to open file \"%s\"!", `TV_OUT_strm_out_TKEEP);
                    $finish;
                end
                $fdisplay(fp, "[[transaction]] %d", transaction_save_strm_out_TKEEP);
                while (~fifo_strm_out_TKEEP.empty) begin
                    fifo_strm_out_TKEEP.pop(data);
                    $fdisplay(fp, "0x%x", data);
                end
                $fdisplay(fp, "[[/transaction]]");
                transaction_save_strm_out_TKEEP = transaction_save_strm_out_TKEEP + 1;
                fifo_strm_out_TKEEP.clear();
                $fclose(fp);
            end
        end
    end
    
    reg [31:0] transaction_save_strm_out_TSTRB;
    
    initial begin : AXI_stream_receiver_strm_out_TSTRB
        integer fp;
        reg [4 - 1:0] data;
        reg [8 * 5:1] str;
        
        transaction_save_strm_out_TSTRB = 0;
        fifo_strm_out_TSTRB.clear();
        wait (reset === 1);
        forever begin
            @ (negedge clk);
            if (done_1 == 1) begin
                fp = $fopen(`TV_OUT_strm_out_TSTRB, "a");
                if (fp == 0) begin // Failed to open file
                    $display("ERROR: Failed to open file \"%s\"!", `TV_OUT_strm_out_TSTRB);
                    $finish;
                end
                $fdisplay(fp, "[[transaction]] %d", transaction_save_strm_out_TSTRB);
                while (~fifo_strm_out_TSTRB.empty) begin
                    fifo_strm_out_TSTRB.pop(data);
                    $fdisplay(fp, "0x%x", data);
                end
                $fdisplay(fp, "[[/transaction]]");
                transaction_save_strm_out_TSTRB = transaction_save_strm_out_TSTRB + 1;
                fifo_strm_out_TSTRB.clear();
                $fclose(fp);
            end
        end
    end
    
    reg [31:0] transaction_save_strm_out_TLAST;
    
    initial begin : AXI_stream_receiver_strm_out_TLAST
        integer fp;
        reg [1 - 1:0] data;
        reg [8 * 5:1] str;
        
        transaction_save_strm_out_TLAST = 0;
        fifo_strm_out_TLAST.clear();
        wait (reset === 1);
        forever begin
            @ (negedge clk);
            if (done_1 == 1) begin
                fp = $fopen(`TV_OUT_strm_out_TLAST, "a");
                if (fp == 0) begin // Failed to open file
                    $display("ERROR: Failed to open file \"%s\"!", `TV_OUT_strm_out_TLAST);
                    $finish;
                end
                $fdisplay(fp, "[[transaction]] %d", transaction_save_strm_out_TLAST);
                while (~fifo_strm_out_TLAST.empty) begin
                    fifo_strm_out_TLAST.pop(data);
                    $fdisplay(fp, "0x%x", data);
                end
                $fdisplay(fp, "[[/transaction]]");
                transaction_save_strm_out_TLAST = transaction_save_strm_out_TLAST + 1;
                fifo_strm_out_TLAST.clear();
                $fclose(fp);
            end
        end
    end

endmodule
