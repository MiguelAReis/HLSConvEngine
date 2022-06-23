// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================

`timescale 1 ns / 1 ps

`define TV_IN_strm_in_TDATA "./c.depthwise.autotvin_strm_in_V_data_V.dat"
`define INGRESS_STATUS_strm_in_TDATA "./stream_ingress_status_strm_in_V_data_V.dat"
`define TV_IN_strm_in_TKEEP "./c.depthwise.autotvin_strm_in_V_keep_V.dat"
`define INGRESS_STATUS_strm_in_TKEEP "./stream_ingress_status_strm_in_V_keep_V.dat"
`define TV_IN_strm_in_TSTRB "./c.depthwise.autotvin_strm_in_V_strb_V.dat"
`define INGRESS_STATUS_strm_in_TSTRB "./stream_ingress_status_strm_in_V_strb_V.dat"
`define TV_IN_strm_in_TLAST "./c.depthwise.autotvin_strm_in_V_last_V.dat"
`define INGRESS_STATUS_strm_in_TLAST "./stream_ingress_status_strm_in_V_last_V.dat"

`define AUTOTB_TRANSACTION_NUM 1

module AESL_axi_s_strm_in (
    input clk,
    input reset,
    output [32 - 1:0] TRAN_strm_in_TDATA,
    output [4 - 1:0] TRAN_strm_in_TKEEP,
    output [4 - 1:0] TRAN_strm_in_TSTRB,
    output TRAN_strm_in_TLAST,
    output TRAN_strm_in_TVALID,
    input TRAN_strm_in_TREADY,
    input ready,
    input done,
    output [31:0] transaction);

    wire TRAN_strm_in_TVALID_temp;
    wire strm_in_TDATA_full;
    wire strm_in_TDATA_empty;
    reg strm_in_TDATA_write_en;
    reg [32 - 1:0] strm_in_TDATA_write_data;
    reg strm_in_TDATA_read_en;
    wire [32 - 1:0] strm_in_TDATA_read_data;
    
    fifo #(103, 32) fifo_strm_in_TDATA (
        .reset(1'b0),
        .write_clock(clk),
        .write_en(strm_in_TDATA_write_en),
        .write_data(strm_in_TDATA_write_data),
        .read_clock(clk),
        .read_en(strm_in_TDATA_read_en),
        .read_data(strm_in_TDATA_read_data),
        .full(strm_in_TDATA_full),
        .empty(strm_in_TDATA_empty));
    
    always @ (*) begin
        strm_in_TDATA_write_en <= 0;
        strm_in_TDATA_read_en <= TRAN_strm_in_TREADY & TRAN_strm_in_TVALID;
    end
    
    assign TRAN_strm_in_TDATA = strm_in_TDATA_read_data;
    wire strm_in_TKEEP_full;
    wire strm_in_TKEEP_empty;
    reg strm_in_TKEEP_write_en;
    reg [4 - 1:0] strm_in_TKEEP_write_data;
    reg strm_in_TKEEP_read_en;
    wire [4 - 1:0] strm_in_TKEEP_read_data;
    
    fifo #(103, 4) fifo_strm_in_TKEEP (
        .reset(1'b0),
        .write_clock(clk),
        .write_en(strm_in_TKEEP_write_en),
        .write_data(strm_in_TKEEP_write_data),
        .read_clock(clk),
        .read_en(strm_in_TKEEP_read_en),
        .read_data(strm_in_TKEEP_read_data),
        .full(strm_in_TKEEP_full),
        .empty(strm_in_TKEEP_empty));
    
    always @ (*) begin
        strm_in_TKEEP_write_en <= 0;
        strm_in_TKEEP_read_en <= TRAN_strm_in_TREADY & TRAN_strm_in_TVALID;
    end
    
    assign TRAN_strm_in_TKEEP = strm_in_TKEEP_read_data;
    wire strm_in_TSTRB_full;
    wire strm_in_TSTRB_empty;
    reg strm_in_TSTRB_write_en;
    reg [4 - 1:0] strm_in_TSTRB_write_data;
    reg strm_in_TSTRB_read_en;
    wire [4 - 1:0] strm_in_TSTRB_read_data;
    
    fifo #(103, 4) fifo_strm_in_TSTRB (
        .reset(1'b0),
        .write_clock(clk),
        .write_en(strm_in_TSTRB_write_en),
        .write_data(strm_in_TSTRB_write_data),
        .read_clock(clk),
        .read_en(strm_in_TSTRB_read_en),
        .read_data(strm_in_TSTRB_read_data),
        .full(strm_in_TSTRB_full),
        .empty(strm_in_TSTRB_empty));
    
    always @ (*) begin
        strm_in_TSTRB_write_en <= 0;
        strm_in_TSTRB_read_en <= TRAN_strm_in_TREADY & TRAN_strm_in_TVALID;
    end
    
    assign TRAN_strm_in_TSTRB = strm_in_TSTRB_read_data;
    wire strm_in_TLAST_full;
    wire strm_in_TLAST_empty;
    reg strm_in_TLAST_write_en;
    reg [1 - 1:0] strm_in_TLAST_write_data;
    reg strm_in_TLAST_read_en;
    wire [1 - 1:0] strm_in_TLAST_read_data;
    
    fifo #(103, 1) fifo_strm_in_TLAST (
        .reset(1'b0),
        .write_clock(clk),
        .write_en(strm_in_TLAST_write_en),
        .write_data(strm_in_TLAST_write_data),
        .read_clock(clk),
        .read_en(strm_in_TLAST_read_en),
        .read_data(strm_in_TLAST_read_data),
        .full(strm_in_TLAST_full),
        .empty(strm_in_TLAST_empty));
    
    always @ (*) begin
        strm_in_TLAST_write_en <= 0;
        strm_in_TLAST_read_en <= TRAN_strm_in_TREADY & TRAN_strm_in_TVALID;
    end
    
    assign TRAN_strm_in_TLAST = strm_in_TLAST_read_data;
    assign TRAN_strm_in_TVALID = TRAN_strm_in_TVALID_temp;

    
    reg strm_in_TDATA_valid = 0; // ingress buffer indicator: strm_in_TDATA
    reg strm_in_TKEEP_valid = 0; // ingress buffer indicator: strm_in_TKEEP
    reg strm_in_TSTRB_valid = 0; // ingress buffer indicator: strm_in_TSTRB
    reg strm_in_TLAST_valid = 0; // ingress buffer indicator: strm_in_TLAST
    
    assign TRAN_strm_in_TVALID_temp = ~(strm_in_TDATA_empty || strm_in_TKEEP_empty || strm_in_TSTRB_empty || strm_in_TLAST_empty) || (strm_in_TDATA_valid && strm_in_TKEEP_valid && strm_in_TSTRB_valid && strm_in_TLAST_valid);
    
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
    
    reg [31:0] transaction_load_strm_in_TDATA;
    
    assign transaction = transaction_load_strm_in_TDATA;
    
    initial begin : AXI_stream_driver_strm_in_TDATA
        integer fp;
        reg [143:0] token;
        reg [32 - 1:0] data;
        reg [143:0] data_integer;
        integer fp_ingress_status;
        reg [143:0] token_ingress_status;
        reg [31:0] ingress_status;
        reg [8 * 5:1] str;
        integer ret;
        
        transaction_load_strm_in_TDATA = 0;
        fifo_strm_in_TDATA.clear();
        wait (reset === 1);
        fp = $fopen(`TV_IN_strm_in_TDATA, "r");
        if (fp == 0) begin // Failed to open file
            $display("ERROR: Failed to open file \"%s\"!", `TV_IN_strm_in_TDATA);
            $finish;
        end
        fp_ingress_status = $fopen(`INGRESS_STATUS_strm_in_TDATA, "r");
        if (fp_ingress_status == 0) begin // Failed to open file
            $display("ERROR: Failed to open file \"%s\"!", `INGRESS_STATUS_strm_in_TDATA);
            $finish;
        end
        token = read_token(fp);
        token_ingress_status = read_token(fp_ingress_status);
        if (token != "[[[runtime]]]") begin
            $display("ERROR: token %s != [[[runtime]]]", token);
            $finish;
        end
        token = read_token(fp); // read 1st "[[transaction]]"
        token_ingress_status = read_token(fp_ingress_status);
        forever begin
            @ (negedge clk);
            if (ready == 1) begin
                if (token != "[[[/runtime]]]") begin
                    if (token != "[[transaction]]") begin
                        $display("ERROR: token %s != [[[transaction]]]", token);
                        $finish;
                    end
                    token = read_token(fp); // skip transaction number
                    token_ingress_status = read_token(fp_ingress_status); // skip transaction number
                    token_ingress_status = read_token(fp_ingress_status); // load ingress status at beginning of transaction
                    fifo_strm_in_TDATA.clear();
                    token = read_token(fp);
                    ret = $sscanf(token_ingress_status, "%d", ingress_status);
                    token_ingress_status = read_token(fp_ingress_status);
                    while (token != "[[/transaction]]") begin
                        if (fifo_strm_in_TDATA.full) begin
                            $display("ERROR: FIFO is full");
                            $finish;
                        end
                        ret = $sscanf(rm_0x(token), "%x", data_integer);
                        data = data_integer;
                        fifo_strm_in_TDATA.push(data);
                        token = read_token(fp);
                        ret = $sscanf(token_ingress_status, "%d", ingress_status);
                        token_ingress_status = read_token(fp_ingress_status);
                    end
                    strm_in_TDATA_valid = (ingress_status > 0);
                    token = read_token(fp);
                    token_ingress_status = read_token(fp_ingress_status);
                end else begin
                    if (fp != 0) begin
                        $fclose(fp);
                        fp = 0;
                    end
                    if (fp_ingress_status != 0) begin
                        $fclose(fp_ingress_status);
                        fp_ingress_status = 0;
                    end
                end
                transaction_load_strm_in_TDATA = transaction_load_strm_in_TDATA + 1;
            end
        end
    end
    
    reg [31:0] transaction_load_strm_in_TKEEP;
    
    initial begin : AXI_stream_driver_strm_in_TKEEP
        integer fp;
        reg [143:0] token;
        reg [4 - 1:0] data;
        reg [143:0] data_integer;
        integer fp_ingress_status;
        reg [143:0] token_ingress_status;
        reg [31:0] ingress_status;
        reg [8 * 5:1] str;
        integer ret;
        
        transaction_load_strm_in_TKEEP = 0;
        fifo_strm_in_TKEEP.clear();
        wait (reset === 1);
        fp = $fopen(`TV_IN_strm_in_TKEEP, "r");
        if (fp == 0) begin // Failed to open file
            $display("ERROR: Failed to open file \"%s\"!", `TV_IN_strm_in_TKEEP);
            $finish;
        end
        fp_ingress_status = $fopen(`INGRESS_STATUS_strm_in_TKEEP, "r");
        if (fp_ingress_status == 0) begin // Failed to open file
            $display("ERROR: Failed to open file \"%s\"!", `INGRESS_STATUS_strm_in_TKEEP);
            $finish;
        end
        token = read_token(fp);
        token_ingress_status = read_token(fp_ingress_status);
        if (token != "[[[runtime]]]") begin
            $display("ERROR: token %s != [[[runtime]]]", token);
            $finish;
        end
        token = read_token(fp); // read 1st "[[transaction]]"
        token_ingress_status = read_token(fp_ingress_status);
        forever begin
            @ (negedge clk);
            if (ready == 1) begin
                if (token != "[[[/runtime]]]") begin
                    if (token != "[[transaction]]") begin
                        $display("ERROR: token %s != [[[transaction]]]", token);
                        $finish;
                    end
                    token = read_token(fp); // skip transaction number
                    token_ingress_status = read_token(fp_ingress_status); // skip transaction number
                    token_ingress_status = read_token(fp_ingress_status); // load ingress status at beginning of transaction
                    fifo_strm_in_TKEEP.clear();
                    token = read_token(fp);
                    ret = $sscanf(token_ingress_status, "%d", ingress_status);
                    token_ingress_status = read_token(fp_ingress_status);
                    while (token != "[[/transaction]]") begin
                        if (fifo_strm_in_TKEEP.full) begin
                            $display("ERROR: FIFO is full");
                            $finish;
                        end
                        ret = $sscanf(rm_0x(token), "%x", data_integer);
                        data = data_integer;
                        fifo_strm_in_TKEEP.push(data);
                        token = read_token(fp);
                        ret = $sscanf(token_ingress_status, "%d", ingress_status);
                        token_ingress_status = read_token(fp_ingress_status);
                    end
                    strm_in_TKEEP_valid = (ingress_status > 0);
                    token = read_token(fp);
                    token_ingress_status = read_token(fp_ingress_status);
                end else begin
                    if (fp != 0) begin
                        $fclose(fp);
                        fp = 0;
                    end
                    if (fp_ingress_status != 0) begin
                        $fclose(fp_ingress_status);
                        fp_ingress_status = 0;
                    end
                end
                transaction_load_strm_in_TKEEP = transaction_load_strm_in_TKEEP + 1;
            end
        end
    end
    
    reg [31:0] transaction_load_strm_in_TSTRB;
    
    initial begin : AXI_stream_driver_strm_in_TSTRB
        integer fp;
        reg [143:0] token;
        reg [4 - 1:0] data;
        reg [143:0] data_integer;
        integer fp_ingress_status;
        reg [143:0] token_ingress_status;
        reg [31:0] ingress_status;
        reg [8 * 5:1] str;
        integer ret;
        
        transaction_load_strm_in_TSTRB = 0;
        fifo_strm_in_TSTRB.clear();
        wait (reset === 1);
        fp = $fopen(`TV_IN_strm_in_TSTRB, "r");
        if (fp == 0) begin // Failed to open file
            $display("ERROR: Failed to open file \"%s\"!", `TV_IN_strm_in_TSTRB);
            $finish;
        end
        fp_ingress_status = $fopen(`INGRESS_STATUS_strm_in_TSTRB, "r");
        if (fp_ingress_status == 0) begin // Failed to open file
            $display("ERROR: Failed to open file \"%s\"!", `INGRESS_STATUS_strm_in_TSTRB);
            $finish;
        end
        token = read_token(fp);
        token_ingress_status = read_token(fp_ingress_status);
        if (token != "[[[runtime]]]") begin
            $display("ERROR: token %s != [[[runtime]]]", token);
            $finish;
        end
        token = read_token(fp); // read 1st "[[transaction]]"
        token_ingress_status = read_token(fp_ingress_status);
        forever begin
            @ (negedge clk);
            if (ready == 1) begin
                if (token != "[[[/runtime]]]") begin
                    if (token != "[[transaction]]") begin
                        $display("ERROR: token %s != [[[transaction]]]", token);
                        $finish;
                    end
                    token = read_token(fp); // skip transaction number
                    token_ingress_status = read_token(fp_ingress_status); // skip transaction number
                    token_ingress_status = read_token(fp_ingress_status); // load ingress status at beginning of transaction
                    fifo_strm_in_TSTRB.clear();
                    token = read_token(fp);
                    ret = $sscanf(token_ingress_status, "%d", ingress_status);
                    token_ingress_status = read_token(fp_ingress_status);
                    while (token != "[[/transaction]]") begin
                        if (fifo_strm_in_TSTRB.full) begin
                            $display("ERROR: FIFO is full");
                            $finish;
                        end
                        ret = $sscanf(rm_0x(token), "%x", data_integer);
                        data = data_integer;
                        fifo_strm_in_TSTRB.push(data);
                        token = read_token(fp);
                        ret = $sscanf(token_ingress_status, "%d", ingress_status);
                        token_ingress_status = read_token(fp_ingress_status);
                    end
                    strm_in_TSTRB_valid = (ingress_status > 0);
                    token = read_token(fp);
                    token_ingress_status = read_token(fp_ingress_status);
                end else begin
                    if (fp != 0) begin
                        $fclose(fp);
                        fp = 0;
                    end
                    if (fp_ingress_status != 0) begin
                        $fclose(fp_ingress_status);
                        fp_ingress_status = 0;
                    end
                end
                transaction_load_strm_in_TSTRB = transaction_load_strm_in_TSTRB + 1;
            end
        end
    end
    
    reg [31:0] transaction_load_strm_in_TLAST;
    
    initial begin : AXI_stream_driver_strm_in_TLAST
        integer fp;
        reg [143:0] token;
        reg [1 - 1:0] data;
        reg [143:0] data_integer;
        integer fp_ingress_status;
        reg [143:0] token_ingress_status;
        reg [31:0] ingress_status;
        reg [8 * 5:1] str;
        integer ret;
        
        transaction_load_strm_in_TLAST = 0;
        fifo_strm_in_TLAST.clear();
        wait (reset === 1);
        fp = $fopen(`TV_IN_strm_in_TLAST, "r");
        if (fp == 0) begin // Failed to open file
            $display("ERROR: Failed to open file \"%s\"!", `TV_IN_strm_in_TLAST);
            $finish;
        end
        fp_ingress_status = $fopen(`INGRESS_STATUS_strm_in_TLAST, "r");
        if (fp_ingress_status == 0) begin // Failed to open file
            $display("ERROR: Failed to open file \"%s\"!", `INGRESS_STATUS_strm_in_TLAST);
            $finish;
        end
        token = read_token(fp);
        token_ingress_status = read_token(fp_ingress_status);
        if (token != "[[[runtime]]]") begin
            $display("ERROR: token %s != [[[runtime]]]", token);
            $finish;
        end
        token = read_token(fp); // read 1st "[[transaction]]"
        token_ingress_status = read_token(fp_ingress_status);
        forever begin
            @ (negedge clk);
            if (ready == 1) begin
                if (token != "[[[/runtime]]]") begin
                    if (token != "[[transaction]]") begin
                        $display("ERROR: token %s != [[[transaction]]]", token);
                        $finish;
                    end
                    token = read_token(fp); // skip transaction number
                    token_ingress_status = read_token(fp_ingress_status); // skip transaction number
                    token_ingress_status = read_token(fp_ingress_status); // load ingress status at beginning of transaction
                    fifo_strm_in_TLAST.clear();
                    token = read_token(fp);
                    ret = $sscanf(token_ingress_status, "%d", ingress_status);
                    token_ingress_status = read_token(fp_ingress_status);
                    while (token != "[[/transaction]]") begin
                        if (fifo_strm_in_TLAST.full) begin
                            $display("ERROR: FIFO is full");
                            $finish;
                        end
                        ret = $sscanf(rm_0x(token), "%x", data_integer);
                        data = data_integer;
                        fifo_strm_in_TLAST.push(data);
                        token = read_token(fp);
                        ret = $sscanf(token_ingress_status, "%d", ingress_status);
                        token_ingress_status = read_token(fp_ingress_status);
                    end
                    strm_in_TLAST_valid = (ingress_status > 0);
                    token = read_token(fp);
                    token_ingress_status = read_token(fp_ingress_status);
                end else begin
                    if (fp != 0) begin
                        $fclose(fp);
                        fp = 0;
                    end
                    if (fp_ingress_status != 0) begin
                        $fclose(fp_ingress_status);
                        fp_ingress_status = 0;
                    end
                end
                transaction_load_strm_in_TLAST = transaction_load_strm_in_TLAST + 1;
            end
        end
    end

endmodule
