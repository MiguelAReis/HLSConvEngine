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


void shiftOnce(filterDataType featureMap[MapMaxN][MapMaxYSize][MapMaxXSize], filterDataType incoming, int kernelN){



	Shift1LineLoop:for(int y=0; y<MapMaxYSize; y++){
		#pragma HLS unroll
		Shift1ValueLoop:for(int x=0; x<MapMaxXSize; x++){
			#pragma HLS unroll
			if(x == MapMaxXSize-1){
				if(y == MapMaxYSize-1){
					featureMap[kernelN][y][x] = incoming;
				}else{
					featureMap[kernelN][y][x] = featureMap[kernelN][y+1][0];
				}
			}else{
				featureMap[kernelN][y][x] = featureMap[kernelN][y][x+1];
			}

		}
	}

}
/*
void print_mat(actDataType x[MapMaxYSize][MapMaxXSize], int rows, int cols)
{
	int i;
	for (i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			printf("%5d ", (int)x[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}
*/

void conv(hls::stream<axisStream> &strm_in,
		hls::stream<axisStream> &strm_out,
		int kernelN,
		int kernelSize,
		int mapSizeX,
		int mapSizeY,
		int PWFilters,
		bool relu){

	#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS INTERFACE axis port=strm_in
	#pragma HLS INTERFACE axis port=strm_out

	actDataType bias;
	filterDataType filter[KernelMaxN][KernelMaxSize][KernelMaxSize];
	//#pragma HLS array_partition variable=filter complete
	actDataType featureMap[MapMaxN][MapMaxYSize][MapMaxXSize];
	actDataType OutputMap[MapMaxN];
	actDataType incomingMap[MapMaxN];
	accDataType accum[KernelMaxN];
	accDataType PWaccum;
	filterDataType PWKernel[PWMaxFilters][KernelMaxN];

	axisStream tmp,tmpo;

	hls::stream<actDataType> interStream;
	#pragma HLS STREAM variable=interStream depth=100

	tmp = strm_in.read(); //Save Bias
	bias = tmp.data;


	SaveDWKernelNLOOP:for(int n=0;n<kernelN;n++){ // Save Filter
		SaveDWKernelYLOOP:for(int y=0;y<KernelMaxSize;y++){
			SaveDWKernelXLOOP:for(int x=0;x<KernelMaxSize;x++){
				#pragma HLS PIPELINE
				if( !(x<kernelSize) || !(y<kernelSize)) filter[n][y][x] = 0;
				else{
					tmp = strm_in.read();
					filter[n][y][x] = tmp.data;
				}
			}
		}
	}

	SavePWKernelNLOOP:for(int n=0;n<PWFilters;n++){ // Save Filter
		SavePWKernelXLOOP:for(int x=0;x<kernelN;x++){
			#pragma HLS PIPELINE
			tmp = strm_in.read();
			PWKernel[n][x] = tmp.data;
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
					//printf(" n %d y %d x %d - %d\n",n,y,x,(int)featureMap[n][y][x]);
				}
			}
		}
	}


	int outMapYSize = mapSizeY-kernelSize+1;
	int outMapXSize = mapSizeX-kernelSize+1;

	DWOutYLOOP:for(int y=0;y<outMapYSize;y++){
		DWOutXLOOP:for(int x=0;x<outMapXSize;x++){
			DWChannelLOOP:for(int kn=0; kn<kernelN; kn++){
				#pragma HLS unroll factor=1
				DWKernelYLOOP:for(int ky=0; ky<KernelMaxSize; ky++){
					DWKernelXLOOP: for(int kx=0; kx<KernelMaxSize; kx++){
						#pragma HLS pipeline
						if(ky==0 && kx==0  ) accum[kn] = bias;
						accum[kn]+=featureMap[kn][ky][kx]*filter[kn][ky][kx];

						if(ky==KernelMaxSize-1 && kx==KernelMaxSize-1){
							if(relu && (accum[kn] < 0)) accum[kn] =0;
							interStream.write(accum[kn]);

							if(tmp.last) shiftOnce(featureMap,DONTCARE,kn);
							else{
								tmp = strm_in.read();
								shiftOnce(featureMap,tmp.data,kn);
							}
							if(x==outMapXSize-1 && kn==kernelN-1){
								DWGetNextLineOfValuesLOOP:for(int i= 1; i<kernelSize+(MapMaxXSize-mapSizeX); i++){
									for(int j=0;j<kernelN;j++){
										if(i<=(mapSizeX-kernelSize)){
											if(tmp.last) shiftOnce(featureMap,DONTCARE,j);
											else{
												tmp = strm_in.read();
												shiftOnce(featureMap,tmp.data,j);
											}
										}else {
											shiftOnce(featureMap,DONTCARE,j);
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

	PWYLOOP:for(int y=0; y<outMapYSize;y++){
		PWXLOOP:for(int x=0; x<outMapXSize;x++){
			PWChannelLOOP:for(int j=0; j<PWFilters;j++){
				PWZLOOP:for(int k=0; k<kernelN;k++){
					#pragma HLS pipeline
					if(j==0){
						OutputMap[k] = interStream.read();
					}
					if(k==0) PWaccum=0;
					PWaccum+=OutputMap[k]*PWKernel[j][k];
					if(k==kernelN-1){
						tmpo.data=PWaccum;
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
