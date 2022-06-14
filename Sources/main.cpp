#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "engineParameters.h"

//#define DEBUG

#define DONTCARE 999


typedef ap_int<WWidth> filterDataType;
typedef ap_int<AWidth> actDataType;
typedef ap_int<AWidth+WWidth> accDataType;


typedef ap_axis<32, 0, 0, 0> axisStream;


void shiftOnce(filterDataType featureMap[MapMaxN][MapMaxYSize][MapMaxXSize], filterDataType incoming[MapMaxN], int kernelN){

	for(int i=0; i<kernelN; i++){
		for(int y=0; y<MapMaxYSize; y++){
			for(int x=0; x<MapMaxXSize; x++){
				if(x == MapMaxXSize-1){
					if(y == MapMaxYSize-1){
						featureMap[i][y][x] = incoming[i];
					}else{
						featureMap[i][y][x] = featureMap[i][y+1][0];
					}
				}else{
					featureMap[i][y][x] = featureMap[i][y][x+1];
				}

			}
		}
	}
}



void conv(hls::stream<axisStream> &strm_in,
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
	filterDataType filter[KernelMaxN][KernelMaxSize][KernelMaxSize];
	filterDataType featureMap[MapMaxN][MapMaxYSize][MapMaxXSize];
	filterDataType incomingMap[MapMaxN];
	accDataType accum[KernelMaxN];


	axisStream tmp,tmpo;

	tmp = strm_in.read(); //Save Bias
	bias = tmp.data;


	SaveKernelNLOOP:for(int n=0;n<kernelN;n++){ // Save Filter
		SaveKernelYLOOP:for(int y=0;y<KernelMaxSize;y++){
			SaveKernelXLOOP:for(int x=0;x<KernelMaxSize;x++){
				#pragma HLS PIPELINE
				if( !(x<kernelSize) || !(y<kernelSize)) filter[n][y][x] = 0;
				else{
					tmp = strm_in.read();
					filter[n][y][x] = tmp.data;
				}
			}
		}
	}


	SaveMapYLOOP:for(int y=0;y<MapMaxYSize;y++){ // Save Map
		SaveMapXLOOP:for(int x=0;x<MapMaxXSize;x++){
			SaveMapNLOOP:for(int n=0;n<kernelN;n++){
				#pragma HLS PIPELINE
				if(x>mapSizeX-1 || y >mapSizeY-1 || tmp.last) featureMap[n][y][x] = DONTCARE;
				else{
					tmp = strm_in.read();
					featureMap[n][y][x] = tmp.data;
				}
			}
		}
	}


	int outMapYSize = mapSizeY-kernelSize+1;
	int outMapXSize = mapSizeX-kernelSize+1;

	DWChannelLOOP:for(int y=0;y<outMapYSize;y++){
		DWOutYLOOP:for(int x=0;x<outMapXSize;x++){
			DWOutXLOOP: for(int kn=0; kn<kernelN; kn++){
				#pragma HLS unroll factor=2
				accum[kn] = bias;
				DWKernelYLOOP:for(int ky=0; ky<KernelMaxSize; ky++){
					#pragma HLS unroll
					DWKernelXLOOP: for(int kx=0; kx<KernelMaxSize; kx++){
						#pragma HLS unroll
						accum[kn]+=featureMap[kn][ky][kx]*filter[kn][ky][kx];

					}
				}
				if(relu && (accum[kn] < 0)) accum[kn] =0;
				tmpo.data=accum[kn];
				tmpo.keep =0xF;
				tmpo.strb = 0xF;
				if(!(y<outMapYSize) && !(x<outMapXSize)) tmpo.last = 1;
				else tmpo.last = 0;
				strm_out.write(tmpo);
			}

			for(int i=0;i<kernelN;i++){
				#pragma HLS PIPELINE
				if(tmp.last) incomingMap[i] = DONTCARE;
				else{
					tmp = strm_in.read();
					incomingMap[i] = tmp.data;
				}
			}
			shiftOnce(featureMap,incomingMap,kernelN);

		}
		for(int i= 1; i<kernelSize+(MapMaxXSize-mapSizeX); i++){
			if(i<=(mapSizeX-kernelSize)){
				for(int i=0;i<kernelN;i++){
					#pragma HLS PIPELINE
					if(tmp.last) incomingMap[i] = DONTCARE;
					else{
						tmp = strm_in.read();
						incomingMap[i] = tmp.data;
					}
				}
			}else {
				for(int i=0;i<kernelN;i++){
					#pragma HLS PIPELINE
					incomingMap[i] = DONTCARE;
				}
			}

			shiftOnce(featureMap,incomingMap,kernelN);
		}
	}



}
