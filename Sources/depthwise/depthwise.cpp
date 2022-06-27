#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "depthwiseParameters.h"

//#define DEBUG

#define DONTCARE 999


typedef ap_int<WWidth> filterDataType;
typedef ap_int<AWidth> actDataType;
typedef ap_int<AWidth+WWidth> accDataType;


typedef ap_axis<32, 0, 0, 0> axisStream;


void depthwise(hls::stream<axisStream> &strm_in,
		hls::stream<axisStream> &strm_out,
		int kernelN,
		int kernelSize,
		int mapSizeX,
		int mapSizeY,
		bool relu){

	#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS INTERFACE axis port=strm_in
	#pragma HLS INTERFACE axis port=strm_out

	actDataType bias;
	filterDataType filter[KernelMaxN*KernelMaxSize*KernelMaxSize];
	//#pragma HLS array_partition variable=filter complete
	actDataType featureMap[MapMaxN*(MapMaxYSize+1)*MapMaxXSize];
	//#pragma HLS array_partition variable=featureMap complete
	accDataType accum;


	axisStream tmp,tmpo;



	tmp = strm_in.read(); //Save Bias
	bias = tmp.data;

	int count =0;
	SaveDWKernelNLOOP:for(int n=0;n<LOOPKernelMaxN;n++){ // Save Filter
	#pragma HLS loop_tripcount min=3 max=3
		SaveDWKernelYLOOP:for(int y=0;y<LOOPKernelMaxSize;y++){
			#pragma HLS loop_tripcount min=3 max=3
			SaveDWKernelXLOOP:for(int x=0;x<LOOPKernelMaxSize;x++){
				#pragma HLS loop_tripcount min=3 max=3
				#pragma HLS PIPELINE
				if(n>=kernelN) n=LOOPKernelMaxN;
				else if(y>=kernelSize) y=LOOPKernelMaxSize;
				else if(x>=kernelSize) x=LOOPKernelMaxSize;
				else{
					tmp = strm_in.read();
					filter[count++] = tmp.data;

				}
			}
		}
	}


	SaveMapYLOOP:for(int y=0;y<LOOPKernelMaxSize;y++){ // Save Map
		SaveMapXLOOP:for(int x=0;x<LOOPMapMaxXSize;x++){
		#pragma HLS loop_tripcount min=5 max=5
			SaveMapNLOOP:for(int n=0;n<LOOPKernelMaxN;n++){
			#pragma HLS loop_tripcount min=3 max=3
				#pragma HLS PIPELINE
				if(x>=mapSizeX) x=LOOPMapMaxXSize;
				else if(n>=kernelN) n=LOOPKernelMaxN;
				else{
					tmp = strm_in.read();
					featureMap[(n*mapSizeX*mapSizeY)+(y*mapSizeX)+x] = tmp.data;
				}
			}
		}
	}


	short outMapYSize = mapSizeY-kernelSize+1;
	short outMapXSize = mapSizeX-kernelSize+1;

	DWOutYLOOP:for(int y=0;y<LOOPMapMaxYSize;y++){
		#pragma HLS loop_tripcount min=5 max=5
		DWOutXLOOP:for(int x=0;x<LOOPMapMaxXSize;x++){
			#pragma HLS loop_tripcount min=5 max=5
			DWChannelLOOP:for(int kn=0; kn<LOOPKernelMaxN; kn++){
				#pragma HLS loop_tripcount min=3 max=3
				#pragma HLS pipeline II=9
				DWKernelYLOOP:for(int ky=0; ky<LOOPKernelMaxSize; ky++){
					#pragma HLS unroll factor=3
					#pragma HLS loop_tripcount min=3 max=3
					DWKernelXLOOP: for(int kx=0; kx<LOOPKernelMaxSize; kx++){
						#pragma HLS loop_tripcount min=3 max=3
						#pragma HLS unroll factor=3

						if(y>=outMapYSize) y=LOOPMapMaxYSize;
						else if(x>=mapSizeX) x=LOOPMapMaxXSize;
						else if(kn>=kernelN) kn=LOOPKernelMaxN;
						else if(ky>=kernelSize) ky=LOOPKernelMaxSize;
						else if(kx>=kernelSize) kx=LOOPKernelMaxSize;
						else{
							if(ky==0 && kx==0 && x < outMapXSize) accum = bias;
							if(x < outMapXSize){
								accum+=featureMap[(kn*mapSizeX*mapSizeY)+(((y+ky)&0x03)*mapSizeX)+(x+kx)]*filter[(kn*kernelSize*kernelSize)+(ky*kernelSize)+kx];
							}

							if(ky==KernelMaxSize-1 && kx==KernelMaxSize-1){
								if(y<outMapYSize-1){

									tmp = strm_in.read();
									featureMap[(kn*mapSizeX*mapSizeY)+(((y+3)&0x03)*mapSizeX)+x]=tmp.data;
								}

								if(x < outMapXSize){
									if(relu && (accum < 0)) accum =0;
									tmpo.data=accum;
									tmpo.keep =0xF;
									tmpo.strb = 0xF;
									tmpo.last = (!(y<outMapYSize) && !(x<outMapXSize));
									strm_out.write(tmpo);
								}
							}
						}
					}
				}
			}
		}
	}
}
