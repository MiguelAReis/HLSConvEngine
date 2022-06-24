#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "convParameters.h"

//#define DEBUG

#define DONTCARE 999


typedef ap_int<WWidth> filterDataType;
typedef ap_int<AWidth> actDataType;
typedef ap_int<AWidth+WWidth> accDataType;


typedef ap_axis<32, 0, 0, 0> axisStream;


void conv(hls::stream<axisStream> &strm_in,
		hls::stream<axisStream> &strm_out,
		int filterN,
		int kernelN,
		int kernelSize,
		int mapSizeX,
		int mapSizeY,
		bool relu){

	#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS INTERFACE axis port=strm_in
	#pragma HLS INTERFACE axis port=strm_out

	actDataType bias;
	filterDataType filter[FilterMaxN*KernelMaxN*KernelMaxSize*KernelMaxSize];
	//#pragma HLS array_partition variable=filter complete
	actDataType featureMap[MapMaxN*(MapMaxYSize+1)*MapMaxXSize];
	//#pragma HLS array_partition variable=featureMap complete
	accDataType accum;


	axisStream tmp,tmpo;



	tmp = strm_in.read(); //Save Bias
	bias = tmp.data;
	int count =0;

	SaveFilterNLOOP:for(int f=0;f<LOOPFilterMaxN;f++){
	#pragma HLS loop_tripcount min=2 max=2
		SaveKernelNLOOP:for(int n=0;n<LOOPKernelMaxN;n++){ // Save Filter
		#pragma HLS loop_tripcount min=3 max=3
			SaveKernelYLOOP:for(int y=0;y<LOOPKernelMaxSize;y++){
				#pragma HLS loop_tripcount min=3 max=3
				SaveKernelXLOOP:for(int x=0;x<LOOPKernelMaxSize;x++){
					#pragma HLS loop_tripcount min=3 max=3
					#pragma HLS PIPELINE
					if(f>=filterN)f=LOOPFilterMaxN;
					else if(n>=kernelN) n=LOOPFilterMaxN;
					else if(y>=kernelSize) y=LOOPKernelMaxSize;
					else if(x>=kernelSize) x=LOOPKernelMaxSize;
					else{
						tmp = strm_in.read();
						filter[count++] = tmp.data;//(f*kernelN*kernelSize*kernelSize)+(n*kernelSize*kernelSize)+(y*kernelSize)+x
					}
				}
			}
		}
	}



	SaveMapYLOOP:for(int y=0;y<LOOPKernelMaxSize;y++){ // Save Map
		#pragma HLS loop_tripcount min=3 max=3
		SaveMapXLOOP:for(int x=0;x<LOOPMapMaxXSize;x++){
		#pragma HLS loop_tripcount min=5 max=5
			SaveMapNLOOP:for(int n=0;n<LOOPKernelMaxN;n++){
			#pragma HLS loop_tripcount min=3 max=3
				#pragma HLS PIPELINE
				if(x>=mapSizeX) x=LOOPMapMaxXSize;
				else if(n>=kernelN) n=LOOPKernelMaxN;
				else{
					tmp = strm_in.read();
					featureMap[(n*mapSizeX*mapSizeY)+(y*mapSizeX)+x] = tmp.data; //(n*mapSizeX*mapSizeY)+(y*mapSizeX)+x
				}
			}
		}
	}


	short outMapYSize = mapSizeY-kernelSize+1;
	short outMapXSize = mapSizeX-kernelSize+1;

	OutYLOOP:for(int y=0;y<LOOPMapMaxYSize;y++){
		#pragma HLS loop_tripcount min=17 max=17
		OutXLOOP:for(int x=0;x<LOOPMapMaxXSize;x++){
			#pragma HLS loop_tripcount min=17 max=17
			FilterLOOP:for(int f=0; f<LOOPFilterMaxN; f++){
				KernelYLOOP:for(int ky=0; ky<LOOPKernelMaxSize; ky++){
					#pragma HLS loop_tripcount min=3 max=3

					KernelXLOOP: for(int kx=0; kx<LOOPKernelMaxSize; kx++){

						#pragma HLS loop_tripcount min=3 max=3
						ChannelLOOP:for(int kn=0; kn<LOOPKernelMaxN; kn++){
							#pragma HLS pipeline
							#pragma HLS unroll factor=1

							if(y>=outMapYSize) y=LOOPMapMaxYSize;
							else if(x>=mapSizeX) x=LOOPMapMaxXSize;
							else if(f>=filterN) f=LOOPFilterMaxN;
							else if(ky>=kernelSize) ky=LOOPKernelMaxSize;
							else if(kx>=kernelSize) kx=LOOPKernelMaxSize;
							else if(kn>=kernelN) kn=LOOPKernelMaxN;
							else{
								if(ky==0 && kx==0 && kn == 0 && x < outMapXSize) accum = bias;
								if(x < outMapXSize) accum+=featureMap[(kn*mapSizeX*mapSizeY)+(((y+ky)&0x03)*mapSizeX)+(x+kx)]*filter[(f*kernelN*kernelSize*kernelSize)+(kn*kernelSize*kernelSize)+(ky*kernelSize)+kx];


								if(ky==kernelSize-1 && kx==kernelSize-1){
									if(y<outMapYSize-1 && f==0){
										tmp = strm_in.read();
										featureMap[(kn*mapSizeX*mapSizeY)+(((y+3)&0x03)*mapSizeX)+x]=tmp.data;

									}
									if(kn==kernelN-1 && x < outMapXSize){
										if(relu && (accum < 0)) accum =0;
											tmpo.data = accum;
											tmpo.keep = 0xF;
											tmpo.strb = 0xF;
											tmpo.last = (!(y<outMapYSize-1) && !(x<outMapXSize-1));
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
}
