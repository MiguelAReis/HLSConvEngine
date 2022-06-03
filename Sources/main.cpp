#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "engineParameters.h"

//#define DEBUG


typedef ap_int<WWidth> filterDataType;
typedef ap_int<AWidth> actDataType;


typedef ap_axis<32, 0, 0, 0> axisStream;

void shiftOnce(filterDataType featureMap[MapMaxN][MapMaxYSize][MapMaxXSize], int kernelN){

	for(int i=0; i<kernelN; i++){
		for(int y=0; y<MapMaxYSize; y++){
			for(int x=0; x<MapMaxXSize; x++){
				if(x == MapMaxXSize-1){
					if(y == MapMaxYSize-1){
						featureMap[i][y][x] = 0;
					}else{
						featureMap[i][y][x] =featureMap[i][y+1][0];
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
	actDataType accum;
	filterDataType filter[KernelMaxN][KernelMaxSize][KernelMaxSize];
	filterDataType featureMap[MapMaxN][MapMaxYSize][MapMaxXSize];
	axisStream tmp,tmpo;

	tmp = strm_in.read(); //Save Bias
	bias = tmp.data;

	#ifdef DEBUG
		printf("*** Bias Value Received is %d\n",(int)bias);
	#endif


	for(int n=0;n<kernelN;n++){ // Save Filter
		for(int y=0;y<KernelMaxSize;y++){
			for(int x=0;x<KernelMaxSize;x++){
				#pragma HLS PIPELINE
				if( !(x<kernelSize) || !(y<kernelSize)) filter[n][y][x] = 0;
				else{
					tmp = strm_in.read();
					filter[n][y][x] = tmp.data;
				}
			}
		}
	}

#ifdef DEBUG
	printf("*** Received Filter with dimension %d * %d * %d\n",(int)kernelN,(int)kernelSize,(int)kernelSize);
	printf("*** First Filter Value Received is %d\n",(int)filter[0][0][0]);
	printf("*** Last Filter Value Received is %d\n",(int)filter[kernelN-1][kernelSize-1][kernelSize-1]);
#endif

	for(int n=0;n<kernelN;n++){ // Save Map
		for(int y=0;y<MapMaxYSize;y++){
			for(int x=0;x<MapMaxXSize;x++){
				#pragma HLS PIPELINE
				if(x>mapSizeX-1 || y >mapSizeY-1) featureMap[n][y][x] = 0;
				else{
					tmp = strm_in.read();
					featureMap[n][y][x] = tmp.data;
				}
			}
		}
	}

#ifdef DEBUG
	printf("*** First Input Map Value Received is %d\n",(int)featureMap[0][0][0]);
	printf("*** Last Input Map Value Received is %d\n",(int)featureMap[kernelN-1][mapSizeY-1][mapSizeX-1]);
#endif

	int outMapYSize = mapSizeY-kernelSize+1;
	int outMapXSize = mapSizeX-kernelSize+1;

	for(int y=0;y<outMapYSize;y++){
		for(int x=0;x<outMapXSize;x++){
			accum = bias;
			for(int kn=0; kn<kernelN; kn++){
				for(int ky=0; ky<KernelMaxSize; ky++){
					#pragma HLS unroll
					for(int kx=0; kx<KernelMaxSize; kx++){
						#pragma HLS unroll
						accum+=featureMap[kn][ky][kx]*filter[kn][ky][kx];
#ifdef DEBUG
	printf("*** Filter %d %d %d  Input Map %d %d %d accum = %d\n",kn,ky,kx,kn,y+ky,x+kx,(int)accum-32);
	printf("*** %d*%d \n",(int)filter[kn][ky][kx],(int)featureMap[kn][y+ky][x+kx]);
#endif
					}
				}
			}
			if(relu && (accum < 0)) accum =0;
#ifdef DEBUG
	printf("*** Output Map Pixel is %d\n",(int)accum);
#endif
			tmpo.data=accum;
			tmpo.keep =0xF;
			tmpo.strb = 0xF;
			if(!(y<outMapYSize) && !(x<outMapXSize)) tmpo.last = 1;
			else tmpo.last = 0;
			strm_out.write(tmpo);

			shiftOnce(featureMap,kernelN);
		}
		for(int i= 1; i<kernelSize+(MapMaxXSize-mapSizeX); i++){
			shiftOnce(featureMap,kernelN);
		}
	}



}
