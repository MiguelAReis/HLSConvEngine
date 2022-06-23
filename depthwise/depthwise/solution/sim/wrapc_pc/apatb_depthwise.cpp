#include <systemc>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <stdint.h>
#include "SysCFileHandler.h"
#include "ap_int.h"
#include "ap_fixed.h"
#include <complex>
#include <stdbool.h>
#include "autopilot_cbe.h"
#include "hls_stream.h"
#include "hls_half.h"
#include "hls_signal_handler.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

// wrapc file define:
#define AUTOTB_TVIN_strm_in_V_data_V "../tv/cdatafile/c.depthwise.autotvin_strm_in_V_data_V.dat"
#define AUTOTB_TVOUT_strm_in_V_data_V "../tv/cdatafile/c.depthwise.autotvout_strm_in_V_data_V.dat"
#define AUTOTB_TVIN_strm_in_V_keep_V "../tv/cdatafile/c.depthwise.autotvin_strm_in_V_keep_V.dat"
#define AUTOTB_TVOUT_strm_in_V_keep_V "../tv/cdatafile/c.depthwise.autotvout_strm_in_V_keep_V.dat"
#define AUTOTB_TVIN_strm_in_V_strb_V "../tv/cdatafile/c.depthwise.autotvin_strm_in_V_strb_V.dat"
#define AUTOTB_TVOUT_strm_in_V_strb_V "../tv/cdatafile/c.depthwise.autotvout_strm_in_V_strb_V.dat"
#define AUTOTB_TVIN_strm_in_V_last_V "../tv/cdatafile/c.depthwise.autotvin_strm_in_V_last_V.dat"
#define AUTOTB_TVOUT_strm_in_V_last_V "../tv/cdatafile/c.depthwise.autotvout_strm_in_V_last_V.dat"
#define WRAPC_STREAM_SIZE_IN_strm_in_V_data_V "../tv/stream_size/stream_size_in_strm_in_V_data_V.dat"
#define WRAPC_STREAM_INGRESS_STATUS_strm_in_V_data_V "../tv/stream_size/stream_ingress_status_strm_in_V_data_V.dat"
#define WRAPC_STREAM_SIZE_IN_strm_in_V_keep_V "../tv/stream_size/stream_size_in_strm_in_V_keep_V.dat"
#define WRAPC_STREAM_INGRESS_STATUS_strm_in_V_keep_V "../tv/stream_size/stream_ingress_status_strm_in_V_keep_V.dat"
#define WRAPC_STREAM_SIZE_IN_strm_in_V_strb_V "../tv/stream_size/stream_size_in_strm_in_V_strb_V.dat"
#define WRAPC_STREAM_INGRESS_STATUS_strm_in_V_strb_V "../tv/stream_size/stream_ingress_status_strm_in_V_strb_V.dat"
#define WRAPC_STREAM_SIZE_IN_strm_in_V_last_V "../tv/stream_size/stream_size_in_strm_in_V_last_V.dat"
#define WRAPC_STREAM_INGRESS_STATUS_strm_in_V_last_V "../tv/stream_size/stream_ingress_status_strm_in_V_last_V.dat"
// wrapc file define:
#define AUTOTB_TVIN_strm_out_V_data_V "../tv/cdatafile/c.depthwise.autotvin_strm_out_V_data_V.dat"
#define AUTOTB_TVOUT_strm_out_V_data_V "../tv/cdatafile/c.depthwise.autotvout_strm_out_V_data_V.dat"
#define AUTOTB_TVIN_strm_out_V_keep_V "../tv/cdatafile/c.depthwise.autotvin_strm_out_V_keep_V.dat"
#define AUTOTB_TVOUT_strm_out_V_keep_V "../tv/cdatafile/c.depthwise.autotvout_strm_out_V_keep_V.dat"
#define AUTOTB_TVIN_strm_out_V_strb_V "../tv/cdatafile/c.depthwise.autotvin_strm_out_V_strb_V.dat"
#define AUTOTB_TVOUT_strm_out_V_strb_V "../tv/cdatafile/c.depthwise.autotvout_strm_out_V_strb_V.dat"
#define AUTOTB_TVIN_strm_out_V_last_V "../tv/cdatafile/c.depthwise.autotvin_strm_out_V_last_V.dat"
#define AUTOTB_TVOUT_strm_out_V_last_V "../tv/cdatafile/c.depthwise.autotvout_strm_out_V_last_V.dat"
#define WRAPC_STREAM_SIZE_OUT_strm_out_V_data_V "../tv/stream_size/stream_size_out_strm_out_V_data_V.dat"
#define WRAPC_STREAM_EGRESS_STATUS_strm_out_V_data_V "../tv/stream_size/stream_egress_status_strm_out_V_data_V.dat"
#define WRAPC_STREAM_SIZE_OUT_strm_out_V_keep_V "../tv/stream_size/stream_size_out_strm_out_V_keep_V.dat"
#define WRAPC_STREAM_EGRESS_STATUS_strm_out_V_keep_V "../tv/stream_size/stream_egress_status_strm_out_V_keep_V.dat"
#define WRAPC_STREAM_SIZE_OUT_strm_out_V_strb_V "../tv/stream_size/stream_size_out_strm_out_V_strb_V.dat"
#define WRAPC_STREAM_EGRESS_STATUS_strm_out_V_strb_V "../tv/stream_size/stream_egress_status_strm_out_V_strb_V.dat"
#define WRAPC_STREAM_SIZE_OUT_strm_out_V_last_V "../tv/stream_size/stream_size_out_strm_out_V_last_V.dat"
#define WRAPC_STREAM_EGRESS_STATUS_strm_out_V_last_V "../tv/stream_size/stream_egress_status_strm_out_V_last_V.dat"
// wrapc file define:
#define AUTOTB_TVIN_kernelN "../tv/cdatafile/c.depthwise.autotvin_kernelN.dat"
#define AUTOTB_TVOUT_kernelN "../tv/cdatafile/c.depthwise.autotvout_kernelN.dat"
// wrapc file define:
#define AUTOTB_TVIN_kernelSize "../tv/cdatafile/c.depthwise.autotvin_kernelSize.dat"
#define AUTOTB_TVOUT_kernelSize "../tv/cdatafile/c.depthwise.autotvout_kernelSize.dat"
// wrapc file define:
#define AUTOTB_TVIN_mapSizeX "../tv/cdatafile/c.depthwise.autotvin_mapSizeX.dat"
#define AUTOTB_TVOUT_mapSizeX "../tv/cdatafile/c.depthwise.autotvout_mapSizeX.dat"
// wrapc file define:
#define AUTOTB_TVIN_mapSizeY "../tv/cdatafile/c.depthwise.autotvin_mapSizeY.dat"
#define AUTOTB_TVOUT_mapSizeY "../tv/cdatafile/c.depthwise.autotvout_mapSizeY.dat"
// wrapc file define:
#define AUTOTB_TVIN_relu "../tv/cdatafile/c.depthwise.autotvin_relu.dat"
#define AUTOTB_TVOUT_relu "../tv/cdatafile/c.depthwise.autotvout_relu.dat"

#define INTER_TCL "../tv/cdatafile/ref.tcl"

// tvout file define:
#define AUTOTB_TVOUT_PC_strm_in_V_data_V "../tv/rtldatafile/rtl.depthwise.autotvout_strm_in_V_data_V.dat"
#define AUTOTB_TVOUT_PC_strm_in_V_keep_V "../tv/rtldatafile/rtl.depthwise.autotvout_strm_in_V_keep_V.dat"
#define AUTOTB_TVOUT_PC_strm_in_V_strb_V "../tv/rtldatafile/rtl.depthwise.autotvout_strm_in_V_strb_V.dat"
#define AUTOTB_TVOUT_PC_strm_in_V_last_V "../tv/rtldatafile/rtl.depthwise.autotvout_strm_in_V_last_V.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_strm_out_V_data_V "../tv/rtldatafile/rtl.depthwise.autotvout_strm_out_V_data_V.dat"
#define AUTOTB_TVOUT_PC_strm_out_V_keep_V "../tv/rtldatafile/rtl.depthwise.autotvout_strm_out_V_keep_V.dat"
#define AUTOTB_TVOUT_PC_strm_out_V_strb_V "../tv/rtldatafile/rtl.depthwise.autotvout_strm_out_V_strb_V.dat"
#define AUTOTB_TVOUT_PC_strm_out_V_last_V "../tv/rtldatafile/rtl.depthwise.autotvout_strm_out_V_last_V.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_kernelN "../tv/rtldatafile/rtl.depthwise.autotvout_kernelN.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_kernelSize "../tv/rtldatafile/rtl.depthwise.autotvout_kernelSize.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_mapSizeX "../tv/rtldatafile/rtl.depthwise.autotvout_mapSizeX.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_mapSizeY "../tv/rtldatafile/rtl.depthwise.autotvout_mapSizeY.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_relu "../tv/rtldatafile/rtl.depthwise.autotvout_relu.dat"

class INTER_TCL_FILE {
  public:
INTER_TCL_FILE(const char* name) {
  mName = name; 
  strm_in_V_data_V_depth = 0;
  strm_in_V_keep_V_depth = 0;
  strm_in_V_strb_V_depth = 0;
  strm_in_V_last_V_depth = 0;
  strm_out_V_data_V_depth = 0;
  strm_out_V_keep_V_depth = 0;
  strm_out_V_strb_V_depth = 0;
  strm_out_V_last_V_depth = 0;
  kernelN_depth = 0;
  kernelSize_depth = 0;
  mapSizeX_depth = 0;
  mapSizeY_depth = 0;
  relu_depth = 0;
  trans_num =0;
}
~INTER_TCL_FILE() {
  mFile.open(mName);
  if (!mFile.good()) {
    cout << "Failed to open file ref.tcl" << endl;
    exit (1); 
  }
  string total_list = get_depth_list();
  mFile << "set depth_list {\n";
  mFile << total_list;
  mFile << "}\n";
  mFile << "set trans_num "<<trans_num<<endl;
  mFile.close();
}
string get_depth_list () {
  stringstream total_list;
  total_list << "{strm_in_V_data_V " << strm_in_V_data_V_depth << "}\n";
  total_list << "{strm_in_V_keep_V " << strm_in_V_keep_V_depth << "}\n";
  total_list << "{strm_in_V_strb_V " << strm_in_V_strb_V_depth << "}\n";
  total_list << "{strm_in_V_last_V " << strm_in_V_last_V_depth << "}\n";
  total_list << "{strm_out_V_data_V " << strm_out_V_data_V_depth << "}\n";
  total_list << "{strm_out_V_keep_V " << strm_out_V_keep_V_depth << "}\n";
  total_list << "{strm_out_V_strb_V " << strm_out_V_strb_V_depth << "}\n";
  total_list << "{strm_out_V_last_V " << strm_out_V_last_V_depth << "}\n";
  total_list << "{kernelN " << kernelN_depth << "}\n";
  total_list << "{kernelSize " << kernelSize_depth << "}\n";
  total_list << "{mapSizeX " << mapSizeX_depth << "}\n";
  total_list << "{mapSizeY " << mapSizeY_depth << "}\n";
  total_list << "{relu " << relu_depth << "}\n";
  return total_list.str();
}
void set_num (int num , int* class_num) {
  (*class_num) = (*class_num) > num ? (*class_num) : num;
}
void set_string(std::string list, std::string* class_list) {
  (*class_list) = list;
}
  public:
    int strm_in_V_data_V_depth;
    int strm_in_V_keep_V_depth;
    int strm_in_V_strb_V_depth;
    int strm_in_V_last_V_depth;
    int strm_out_V_data_V_depth;
    int strm_out_V_keep_V_depth;
    int strm_out_V_strb_V_depth;
    int strm_out_V_last_V_depth;
    int kernelN_depth;
    int kernelSize_depth;
    int mapSizeX_depth;
    int mapSizeY_depth;
    int relu_depth;
    int trans_num;
  private:
    ofstream mFile;
    const char* mName;
};

static void RTLOutputCheckAndReplacement(std::string &AESL_token, std::string PortName) {
  bool no_x = false;
  bool err = false;

  no_x = false;
  // search and replace 'X' with '0' from the 3rd char of token
  while (!no_x) {
    size_t x_found = AESL_token.find('X', 0);
    if (x_found != string::npos) {
      if (!err) { 
        cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'X' on port" 
             << PortName << ", possible cause: There are uninitialized variables in the C design."
             << endl; 
        err = true;
      }
      AESL_token.replace(x_found, 1, "0");
    } else
      no_x = true;
  }
  no_x = false;
  // search and replace 'x' with '0' from the 3rd char of token
  while (!no_x) {
    size_t x_found = AESL_token.find('x', 2);
    if (x_found != string::npos) {
      if (!err) { 
        cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' on port" 
             << PortName << ", possible cause: There are uninitialized variables in the C design."
             << endl; 
        err = true;
      }
      AESL_token.replace(x_found, 1, "0");
    } else
      no_x = true;
  }
}
struct __cosim_sC__ { char data[12]; };
extern "C" void depthwise_hw_stub_wrapper(volatile void *, volatile void *, int, int, int, int, char);

extern "C" void apatb_depthwise_hw(volatile void * __xlx_apatb_param_strm_in, volatile void * __xlx_apatb_param_strm_out, int __xlx_apatb_param_kernelN, int __xlx_apatb_param_kernelSize, int __xlx_apatb_param_mapSizeX, int __xlx_apatb_param_mapSizeY, char __xlx_apatb_param_relu) {
  refine_signal_handler();
  fstream wrapc_switch_file_token;
  wrapc_switch_file_token.open(".hls_cosim_wrapc_switch.log");
  int AESL_i;
  if (wrapc_switch_file_token.good())
  {

    CodeState = ENTER_WRAPC_PC;
    static unsigned AESL_transaction_pc = 0;
    string AESL_token;
    string AESL_num;long __xlx_apatb_param_strm_in_stream_buf_final_size;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(WRAPC_STREAM_SIZE_IN_strm_in_V_data_V);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){__xlx_apatb_param_strm_in_stream_buf_final_size = atoi(AESL_token.c_str());

            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  for (long i = 0; i < __xlx_apatb_param_strm_in_stream_buf_final_size; ++i)((hls::stream<__cosim_sC__>*)__xlx_apatb_param_strm_in)->read();
{sc_bv<96> xlx_stream_strm_out_pc_buffer;
unsigned xlx_stream_strm_out_size = 0;

          std::vector<sc_bv<32> > strm_out_V_data_V_pc_buffer_Copy;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_strm_out_V_data_V);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<32> > strm_out_V_data_V_pc_buffer;
          int i = 0;

          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            RTLOutputCheckAndReplacement(AESL_token, "strm_out");
  
            // push token into output port buffer
            if (AESL_token != "") {
              strm_out_V_data_V_pc_buffer.push_back(AESL_token.c_str());
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (i > 0) {xlx_stream_strm_out_size=strm_out_V_data_V_pc_buffer.size();
strm_out_V_data_V_pc_buffer_Copy=strm_out_V_data_V_pc_buffer;
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  
          std::vector<sc_bv<32> > strm_out_V_keep_V_pc_buffer_Copy;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_strm_out_V_keep_V);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<32> > strm_out_V_keep_V_pc_buffer;
          int i = 0;

          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            RTLOutputCheckAndReplacement(AESL_token, "strm_out");
  
            // push token into output port buffer
            if (AESL_token != "") {
              strm_out_V_keep_V_pc_buffer.push_back(AESL_token.c_str());
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (i > 0) {xlx_stream_strm_out_size=strm_out_V_keep_V_pc_buffer.size();
strm_out_V_keep_V_pc_buffer_Copy=strm_out_V_keep_V_pc_buffer;
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  
          std::vector<sc_bv<32> > strm_out_V_strb_V_pc_buffer_Copy;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_strm_out_V_strb_V);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<32> > strm_out_V_strb_V_pc_buffer;
          int i = 0;

          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            RTLOutputCheckAndReplacement(AESL_token, "strm_out");
  
            // push token into output port buffer
            if (AESL_token != "") {
              strm_out_V_strb_V_pc_buffer.push_back(AESL_token.c_str());
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (i > 0) {xlx_stream_strm_out_size=strm_out_V_strb_V_pc_buffer.size();
strm_out_V_strb_V_pc_buffer_Copy=strm_out_V_strb_V_pc_buffer;
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  
          std::vector<sc_bv<32> > strm_out_V_last_V_pc_buffer_Copy;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_strm_out_V_last_V);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<32> > strm_out_V_last_V_pc_buffer;
          int i = 0;

          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            RTLOutputCheckAndReplacement(AESL_token, "strm_out");
  
            // push token into output port buffer
            if (AESL_token != "") {
              strm_out_V_last_V_pc_buffer.push_back(AESL_token.c_str());
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (i > 0) {xlx_stream_strm_out_size=strm_out_V_last_V_pc_buffer.size();
strm_out_V_last_V_pc_buffer_Copy=strm_out_V_last_V_pc_buffer;
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  for (int j = 0, e = xlx_stream_strm_out_size; j != e; ++j) {
xlx_stream_strm_out_pc_buffer.range(31, 0) = strm_out_V_data_V_pc_buffer_Copy[j];
xlx_stream_strm_out_pc_buffer.range(39, 32) = strm_out_V_keep_V_pc_buffer_Copy[j];
xlx_stream_strm_out_pc_buffer.range(47, 40) = strm_out_V_strb_V_pc_buffer_Copy[j];
xlx_stream_strm_out_pc_buffer.range(63, 56) = strm_out_V_last_V_pc_buffer_Copy[j];
__cosim_sC__ xlx_stream_elt;
((long long*)&xlx_stream_elt)[0*1+0] = xlx_stream_strm_out_pc_buffer.range(63,0).to_int64();
((long long*)&xlx_stream_elt)[0*1+1] = xlx_stream_strm_out_pc_buffer.range(95,64).to_int64();
((hls::stream<__cosim_sC__>*)__xlx_apatb_param_strm_out)->write(xlx_stream_elt);
}}
    AESL_transaction_pc++;
    return ;
  }
static unsigned AESL_transaction;
static AESL_FILE_HANDLER aesl_fh;
static INTER_TCL_FILE tcl_file(INTER_TCL);
std::vector<char> __xlx_sprintf_buffer(1024);
CodeState = ENTER_WRAPC;
//strm_in
aesl_fh.touch(AUTOTB_TVIN_strm_in_V_data_V);
aesl_fh.touch(AUTOTB_TVOUT_strm_in_V_data_V);
aesl_fh.touch(AUTOTB_TVIN_strm_in_V_keep_V);
aesl_fh.touch(AUTOTB_TVOUT_strm_in_V_keep_V);
aesl_fh.touch(AUTOTB_TVIN_strm_in_V_strb_V);
aesl_fh.touch(AUTOTB_TVOUT_strm_in_V_strb_V);
aesl_fh.touch(AUTOTB_TVIN_strm_in_V_last_V);
aesl_fh.touch(AUTOTB_TVOUT_strm_in_V_last_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_IN_strm_in_V_data_V);
aesl_fh.touch(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_data_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_IN_strm_in_V_keep_V);
aesl_fh.touch(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_keep_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_IN_strm_in_V_strb_V);
aesl_fh.touch(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_strb_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_IN_strm_in_V_last_V);
aesl_fh.touch(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_last_V);
//strm_out
aesl_fh.touch(AUTOTB_TVIN_strm_out_V_data_V);
aesl_fh.touch(AUTOTB_TVOUT_strm_out_V_data_V);
aesl_fh.touch(AUTOTB_TVIN_strm_out_V_keep_V);
aesl_fh.touch(AUTOTB_TVOUT_strm_out_V_keep_V);
aesl_fh.touch(AUTOTB_TVIN_strm_out_V_strb_V);
aesl_fh.touch(AUTOTB_TVOUT_strm_out_V_strb_V);
aesl_fh.touch(AUTOTB_TVIN_strm_out_V_last_V);
aesl_fh.touch(AUTOTB_TVOUT_strm_out_V_last_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_OUT_strm_out_V_data_V);
aesl_fh.touch(WRAPC_STREAM_EGRESS_STATUS_strm_out_V_data_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_OUT_strm_out_V_keep_V);
aesl_fh.touch(WRAPC_STREAM_EGRESS_STATUS_strm_out_V_keep_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_OUT_strm_out_V_strb_V);
aesl_fh.touch(WRAPC_STREAM_EGRESS_STATUS_strm_out_V_strb_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_OUT_strm_out_V_last_V);
aesl_fh.touch(WRAPC_STREAM_EGRESS_STATUS_strm_out_V_last_V);
//kernelN
aesl_fh.touch(AUTOTB_TVIN_kernelN);
aesl_fh.touch(AUTOTB_TVOUT_kernelN);
//kernelSize
aesl_fh.touch(AUTOTB_TVIN_kernelSize);
aesl_fh.touch(AUTOTB_TVOUT_kernelSize);
//mapSizeX
aesl_fh.touch(AUTOTB_TVIN_mapSizeX);
aesl_fh.touch(AUTOTB_TVOUT_mapSizeX);
//mapSizeY
aesl_fh.touch(AUTOTB_TVIN_mapSizeY);
aesl_fh.touch(AUTOTB_TVOUT_mapSizeY);
//relu
aesl_fh.touch(AUTOTB_TVIN_relu);
aesl_fh.touch(AUTOTB_TVOUT_relu);
CodeState = DUMP_INPUTS;
std::vector<__cosim_sC__> __xlx_apatb_param_strm_in_stream_buf;
{
  while (!((hls::stream<__cosim_sC__>*)__xlx_apatb_param_strm_in)->empty())
    __xlx_apatb_param_strm_in_stream_buf.push_back(((hls::stream<__cosim_sC__>*)__xlx_apatb_param_strm_in)->read());
  for (int i = 0; i < __xlx_apatb_param_strm_in_stream_buf.size(); ++i)
    ((hls::stream<__cosim_sC__>*)__xlx_apatb_param_strm_in)->write(__xlx_apatb_param_strm_in_stream_buf[i]);
  }
long __xlx_apatb_param_strm_in_stream_buf_size = ((hls::stream<__cosim_sC__>*)__xlx_apatb_param_strm_in)->size();
std::vector<__cosim_sC__> __xlx_apatb_param_strm_out_stream_buf;
long __xlx_apatb_param_strm_out_stream_buf_size = ((hls::stream<__cosim_sC__>*)__xlx_apatb_param_strm_out)->size();
// print kernelN Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_kernelN, __xlx_sprintf_buffer.data());
  {
    sc_bv<32> __xlx_tmp_lv = *((int*)&__xlx_apatb_param_kernelN);

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_kernelN, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.kernelN_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_kernelN, __xlx_sprintf_buffer.data());
}
// print kernelSize Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_kernelSize, __xlx_sprintf_buffer.data());
  {
    sc_bv<32> __xlx_tmp_lv = *((int*)&__xlx_apatb_param_kernelSize);

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_kernelSize, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.kernelSize_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_kernelSize, __xlx_sprintf_buffer.data());
}
// print mapSizeX Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_mapSizeX, __xlx_sprintf_buffer.data());
  {
    sc_bv<32> __xlx_tmp_lv = *((int*)&__xlx_apatb_param_mapSizeX);

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_mapSizeX, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.mapSizeX_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_mapSizeX, __xlx_sprintf_buffer.data());
}
// print mapSizeY Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_mapSizeY, __xlx_sprintf_buffer.data());
  {
    sc_bv<32> __xlx_tmp_lv = *((int*)&__xlx_apatb_param_mapSizeY);

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_mapSizeY, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.mapSizeY_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_mapSizeY, __xlx_sprintf_buffer.data());
}
// print relu Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_relu, __xlx_sprintf_buffer.data());
  {
    sc_bv<1> __xlx_tmp_lv = *((char*)&__xlx_apatb_param_relu);

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_relu, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.relu_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_relu, __xlx_sprintf_buffer.data());
}
CodeState = CALL_C_DUT;
depthwise_hw_stub_wrapper(__xlx_apatb_param_strm_in, __xlx_apatb_param_strm_out, __xlx_apatb_param_kernelN, __xlx_apatb_param_kernelSize, __xlx_apatb_param_mapSizeX, __xlx_apatb_param_mapSizeY, __xlx_apatb_param_relu);
CodeState = DUMP_OUTPUTS;
long __xlx_apatb_param_strm_in_stream_buf_final_size = __xlx_apatb_param_strm_in_stream_buf_size - ((hls::stream<__cosim_sC__>*)__xlx_apatb_param_strm_in)->size();

  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_strm_in_V_data_V, __xlx_sprintf_buffer.data());
  
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_strm_in_V_keep_V, __xlx_sprintf_buffer.data());
  
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_strm_in_V_strb_V, __xlx_sprintf_buffer.data());
  
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_strm_in_V_last_V, __xlx_sprintf_buffer.data());
  for (int j = 0, e = __xlx_apatb_param_strm_in_stream_buf_final_size; j != e; ++j) {
sc_bv<96> __xlx_tmp_lv;
__xlx_tmp_lv.range(63,0) = ((long long*)&__xlx_apatb_param_strm_in_stream_buf[j])[0*1+0];
__xlx_tmp_lv.range(95,64) = ((long long*)&__xlx_apatb_param_strm_in_stream_buf[j])[0*1+1];
sc_bv<32> __xlx_tmp_0_lv = __xlx_tmp_lv.range(31, 0);
sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_0_lv.to_string(SC_HEX).c_str());
aesl_fh.write(AUTOTB_TVIN_strm_in_V_data_V, __xlx_sprintf_buffer.data());
sc_bv<8> __xlx_tmp_1_lv = __xlx_tmp_lv.range(39, 32);
sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_1_lv.to_string(SC_HEX).c_str());
aesl_fh.write(AUTOTB_TVIN_strm_in_V_keep_V, __xlx_sprintf_buffer.data());
sc_bv<8> __xlx_tmp_2_lv = __xlx_tmp_lv.range(47, 40);
sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_2_lv.to_string(SC_HEX).c_str());
aesl_fh.write(AUTOTB_TVIN_strm_in_V_strb_V, __xlx_sprintf_buffer.data());
sc_bv<8> __xlx_tmp_4_lv = __xlx_tmp_lv.range(63, 56);
sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_4_lv.to_string(SC_HEX).c_str());
aesl_fh.write(AUTOTB_TVIN_strm_in_V_last_V, __xlx_sprintf_buffer.data());
}
tcl_file.set_num(__xlx_apatb_param_strm_in_stream_buf_final_size, &tcl_file.strm_in_V_data_V_depth);

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_strm_in_V_data_V, __xlx_sprintf_buffer.data());
tcl_file.set_num(__xlx_apatb_param_strm_in_stream_buf_final_size, &tcl_file.strm_in_V_keep_V_depth);

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_strm_in_V_keep_V, __xlx_sprintf_buffer.data());
tcl_file.set_num(__xlx_apatb_param_strm_in_stream_buf_final_size, &tcl_file.strm_in_V_strb_V_depth);

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_strm_in_V_strb_V, __xlx_sprintf_buffer.data());
tcl_file.set_num(__xlx_apatb_param_strm_in_stream_buf_final_size, &tcl_file.strm_in_V_last_V_depth);

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_strm_in_V_last_V, __xlx_sprintf_buffer.data());

// dump stream ingress status to file

// dump stream ingress status to file
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_data_V, __xlx_sprintf_buffer.data());
  if (__xlx_apatb_param_strm_in_stream_buf_final_size > 0) {
  long strm_in_V_data_V_stream_ingress_size = __xlx_apatb_param_strm_in_stream_buf_size;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", strm_in_V_data_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_data_V, __xlx_sprintf_buffer.data());
  for (int j = 0, e = __xlx_apatb_param_strm_in_stream_buf_final_size; j != e; j++) {
    strm_in_V_data_V_stream_ingress_size--;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", strm_in_V_data_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_data_V, __xlx_sprintf_buffer.data());
  }
} else {
  long strm_in_V_data_V_stream_ingress_size = 0;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", strm_in_V_data_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_data_V, __xlx_sprintf_buffer.data());
}

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_data_V, __xlx_sprintf_buffer.data());
}
// dump stream ingress status to file
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_keep_V, __xlx_sprintf_buffer.data());
  if (__xlx_apatb_param_strm_in_stream_buf_final_size > 0) {
  long strm_in_V_keep_V_stream_ingress_size = __xlx_apatb_param_strm_in_stream_buf_size;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", strm_in_V_keep_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_keep_V, __xlx_sprintf_buffer.data());
  for (int j = 0, e = __xlx_apatb_param_strm_in_stream_buf_final_size; j != e; j++) {
    strm_in_V_keep_V_stream_ingress_size--;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", strm_in_V_keep_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_keep_V, __xlx_sprintf_buffer.data());
  }
} else {
  long strm_in_V_keep_V_stream_ingress_size = 0;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", strm_in_V_keep_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_keep_V, __xlx_sprintf_buffer.data());
}

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_keep_V, __xlx_sprintf_buffer.data());
}
// dump stream ingress status to file
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_strb_V, __xlx_sprintf_buffer.data());
  if (__xlx_apatb_param_strm_in_stream_buf_final_size > 0) {
  long strm_in_V_strb_V_stream_ingress_size = __xlx_apatb_param_strm_in_stream_buf_size;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", strm_in_V_strb_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_strb_V, __xlx_sprintf_buffer.data());
  for (int j = 0, e = __xlx_apatb_param_strm_in_stream_buf_final_size; j != e; j++) {
    strm_in_V_strb_V_stream_ingress_size--;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", strm_in_V_strb_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_strb_V, __xlx_sprintf_buffer.data());
  }
} else {
  long strm_in_V_strb_V_stream_ingress_size = 0;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", strm_in_V_strb_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_strb_V, __xlx_sprintf_buffer.data());
}

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_strb_V, __xlx_sprintf_buffer.data());
}
// dump stream ingress status to file
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_last_V, __xlx_sprintf_buffer.data());
  if (__xlx_apatb_param_strm_in_stream_buf_final_size > 0) {
  long strm_in_V_last_V_stream_ingress_size = __xlx_apatb_param_strm_in_stream_buf_size;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", strm_in_V_last_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_last_V, __xlx_sprintf_buffer.data());
  for (int j = 0, e = __xlx_apatb_param_strm_in_stream_buf_final_size; j != e; j++) {
    strm_in_V_last_V_stream_ingress_size--;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", strm_in_V_last_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_last_V, __xlx_sprintf_buffer.data());
  }
} else {
  long strm_in_V_last_V_stream_ingress_size = 0;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", strm_in_V_last_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_last_V, __xlx_sprintf_buffer.data());
}

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_strm_in_V_last_V, __xlx_sprintf_buffer.data());
}{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(WRAPC_STREAM_SIZE_IN_strm_in_V_data_V, __xlx_sprintf_buffer.data());
  sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_strm_in_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_IN_strm_in_V_data_V, __xlx_sprintf_buffer.data());

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(WRAPC_STREAM_SIZE_IN_strm_in_V_data_V, __xlx_sprintf_buffer.data());
}{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(WRAPC_STREAM_SIZE_IN_strm_in_V_keep_V, __xlx_sprintf_buffer.data());
  sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_strm_in_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_IN_strm_in_V_keep_V, __xlx_sprintf_buffer.data());

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(WRAPC_STREAM_SIZE_IN_strm_in_V_keep_V, __xlx_sprintf_buffer.data());
}{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(WRAPC_STREAM_SIZE_IN_strm_in_V_strb_V, __xlx_sprintf_buffer.data());
  sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_strm_in_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_IN_strm_in_V_strb_V, __xlx_sprintf_buffer.data());

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(WRAPC_STREAM_SIZE_IN_strm_in_V_strb_V, __xlx_sprintf_buffer.data());
}{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(WRAPC_STREAM_SIZE_IN_strm_in_V_last_V, __xlx_sprintf_buffer.data());
  sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_strm_in_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_IN_strm_in_V_last_V, __xlx_sprintf_buffer.data());

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(WRAPC_STREAM_SIZE_IN_strm_in_V_last_V, __xlx_sprintf_buffer.data());
}
//********************** dump C output stream *******************
long __xlx_apatb_param_strm_out_stream_buf_final_size = ((hls::stream<__cosim_sC__>*)__xlx_apatb_param_strm_out)->size() - __xlx_apatb_param_strm_out_stream_buf_size;
{
  while (!((hls::stream<__cosim_sC__>*)__xlx_apatb_param_strm_out)->empty())
    __xlx_apatb_param_strm_out_stream_buf.push_back(((hls::stream<__cosim_sC__>*)__xlx_apatb_param_strm_out)->read());
  for (int i = 0; i < __xlx_apatb_param_strm_out_stream_buf.size(); ++i)
    ((hls::stream<__cosim_sC__>*)__xlx_apatb_param_strm_out)->write(__xlx_apatb_param_strm_out_stream_buf[i]);
  }

  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVOUT_strm_out_V_data_V, __xlx_sprintf_buffer.data());
  
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVOUT_strm_out_V_keep_V, __xlx_sprintf_buffer.data());
  
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVOUT_strm_out_V_strb_V, __xlx_sprintf_buffer.data());
  
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVOUT_strm_out_V_last_V, __xlx_sprintf_buffer.data());
  for (int j = 0, e = __xlx_apatb_param_strm_out_stream_buf_final_size; j != e; ++j) {
sc_bv<96> __xlx_tmp_lv;
__xlx_tmp_lv.range(63,0) = ((long long*)&__xlx_apatb_param_strm_out_stream_buf[__xlx_apatb_param_strm_out_stream_buf_size+j])[0*1+0];
__xlx_tmp_lv.range(95,64) = ((long long*)&__xlx_apatb_param_strm_out_stream_buf[__xlx_apatb_param_strm_out_stream_buf_size+j])[0*1+1];
sc_bv<32> __xlx_tmp_0_lv = __xlx_tmp_lv.range(31, 0);
sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_0_lv.to_string(SC_HEX).c_str());
aesl_fh.write(AUTOTB_TVOUT_strm_out_V_data_V, __xlx_sprintf_buffer.data());
sc_bv<8> __xlx_tmp_1_lv = __xlx_tmp_lv.range(39, 32);
sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_1_lv.to_string(SC_HEX).c_str());
aesl_fh.write(AUTOTB_TVOUT_strm_out_V_keep_V, __xlx_sprintf_buffer.data());
sc_bv<8> __xlx_tmp_2_lv = __xlx_tmp_lv.range(47, 40);
sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_2_lv.to_string(SC_HEX).c_str());
aesl_fh.write(AUTOTB_TVOUT_strm_out_V_strb_V, __xlx_sprintf_buffer.data());
sc_bv<8> __xlx_tmp_4_lv = __xlx_tmp_lv.range(63, 56);
sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_4_lv.to_string(SC_HEX).c_str());
aesl_fh.write(AUTOTB_TVOUT_strm_out_V_last_V, __xlx_sprintf_buffer.data());
}
tcl_file.set_num(__xlx_apatb_param_strm_out_stream_buf_final_size, &tcl_file.strm_out_V_data_V_depth);

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVOUT_strm_out_V_data_V, __xlx_sprintf_buffer.data());
tcl_file.set_num(__xlx_apatb_param_strm_out_stream_buf_final_size, &tcl_file.strm_out_V_keep_V_depth);

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVOUT_strm_out_V_keep_V, __xlx_sprintf_buffer.data());
tcl_file.set_num(__xlx_apatb_param_strm_out_stream_buf_final_size, &tcl_file.strm_out_V_strb_V_depth);

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVOUT_strm_out_V_strb_V, __xlx_sprintf_buffer.data());
tcl_file.set_num(__xlx_apatb_param_strm_out_stream_buf_final_size, &tcl_file.strm_out_V_last_V_depth);

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVOUT_strm_out_V_last_V, __xlx_sprintf_buffer.data());
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(WRAPC_STREAM_SIZE_OUT_strm_out_V_data_V, __xlx_sprintf_buffer.data());
  sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_strm_out_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_OUT_strm_out_V_data_V, __xlx_sprintf_buffer.data());

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(WRAPC_STREAM_SIZE_OUT_strm_out_V_data_V, __xlx_sprintf_buffer.data());
}{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(WRAPC_STREAM_SIZE_OUT_strm_out_V_keep_V, __xlx_sprintf_buffer.data());
  sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_strm_out_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_OUT_strm_out_V_keep_V, __xlx_sprintf_buffer.data());

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(WRAPC_STREAM_SIZE_OUT_strm_out_V_keep_V, __xlx_sprintf_buffer.data());
}{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(WRAPC_STREAM_SIZE_OUT_strm_out_V_strb_V, __xlx_sprintf_buffer.data());
  sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_strm_out_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_OUT_strm_out_V_strb_V, __xlx_sprintf_buffer.data());

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(WRAPC_STREAM_SIZE_OUT_strm_out_V_strb_V, __xlx_sprintf_buffer.data());
}{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(WRAPC_STREAM_SIZE_OUT_strm_out_V_last_V, __xlx_sprintf_buffer.data());
  sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_strm_out_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_OUT_strm_out_V_last_V, __xlx_sprintf_buffer.data());

  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(WRAPC_STREAM_SIZE_OUT_strm_out_V_last_V, __xlx_sprintf_buffer.data());
}CodeState = DELETE_CHAR_BUFFERS;
AESL_transaction++;
tcl_file.set_num(AESL_transaction , &tcl_file.trans_num);
}
