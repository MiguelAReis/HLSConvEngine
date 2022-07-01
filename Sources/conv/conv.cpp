#include <ap_int.h>
#include <stdint.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "convParameters.h"

//#define DEBUG


typedef ap_int<64> filterDataType;
typedef ap_int<64> actDataType;
typedef ap_int<AWidth+WWidth> accDataType;


typedef ap_axis<64, 0, 0, 0> axisStream;

void readWeights(filterDataType filter[FilterMaxN*KernelMaxSize*KernelMaxSize*KernelMaxN], uint address, hls::stream<axisStream> &strm_in)
{
	axisStream tmp = strm_in.read(); //Save Bias
	//for(uint i=0;i<weightsPerStream;i++){
		//#pragma HLS unroll
		filter[address/*+1*/]=tmp.data;//.range( (i+1)*WWidth-1, (i*WWidth) );
	//}
		//printf("New stream\n");
	//for(uint i=weightsPerStream;i>0;i--){
		//printf("%d,%d\n",(i+1)*WWidth-1,(i*WWidth));
	//	printf("%d\n",(uint)filter[address].range( (i)*WWidth-1, ((i-1)*WWidth)) );
	//}

}

void readInitialWeights(filterDataType filter[FilterMaxN*KernelMaxSize*KernelMaxSize*KernelMaxN],uint totalWeights, hls::stream<axisStream> &strm_in){

	SaveFilterLOOP:for(int i=0;i<totalWeights;i++){
		#pragma HLS loop_tripcount min=(5*3*3*3) max=(5*3*3*3)
		#pragma HLS pipeline II=1
		readWeights(filter,i,strm_in);
	}
}

void readActs(actDataType featureMap[MapMaxN*(MapMaxYSize+1)*MapMaxXSize], uint address, hls::stream<axisStream> &strm_in){
	axisStream tmp = strm_in.read(); //Save Bias
	featureMap[address]=tmp.data;
	for(uint i=actsPerStream;i>0;i--){
		//printf("%d\n",(uint)featureMap[address].range( (i)*WWidth-1, ((i-1)*WWidth)) );
	}
}

void readInitialFeatureMap(actDataType featureMap[MapMaxN*(MapMaxYSize+1)*MapMaxXSize], uint totalActs, hls::stream<axisStream> &strm_in){

	SaveMapLOOP:for(int i=0;i<totalActs;i++){
		#pragma HLS loop_tripcount min=(3*5*3) max=(3*5*3)
		#pragma HLS PIPELINE II=1
		readActs(featureMap,i,strm_in);
	}
}

void dotProduct( actDataType featureMapValue,filterDataType filterValue, accDataType *accum){
	*accum+=filterValue*featureMapValue;
	//printf("\t\tfilter %d featureMap %d accum %d\n",(uint)filterValue,(uint)featureMapValue,(int)*accum);
}

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
	filterDataType filter[FilterMaxN*KernelMaxSize*KernelMaxSize*KernelMaxN];
	//#pragma HLS array_partition variable=filter complete
	actDataType featureMap[MapMaxN*(MapMaxYSize+1)*MapMaxXSize];
	//#pragma HLS array_partition variable=featureMap complete
	//#pragma HLS array_partition variable=featureMap cyclic factor=64
	accDataType accum;


	axisStream tmp,tmpo;



	tmp = strm_in.read(); //Save Bias
	bias = tmp.data;
	int count =0;//             (tbFilterN*tbFilterSize*tbFilterSize*(tbKernelN/weightsPerStream+1))
	//printf("Reading %d values\n",filterN*kernelSize*kernelSize*(kernelN/weightsPerStream+1));
	readInitialWeights(filter,filterN*kernelSize*kernelSize*(kernelN/weightsPerStream+1),strm_in);

	readInitialFeatureMap(featureMap,3*mapSizeX*(kernelN/actsPerStream+1),strm_in);




	short outMapYSize = mapSizeY-kernelSize+1;
	short outMapXSize = mapSizeX-kernelSize+1;
	int address0,address1, address2=0;
	int addressSuplement = mapSizeX*kernelN;
	int address2Suplement=kernelN*kernelSize*kernelSize;
	ap_int<64> featureMapPacked, filterPacked, outValues=0;

	OutYLOOP:for(int y=0;y<LOOPMapMaxYSize;y++){
		#pragma HLS loop_tripcount min=5 max=5
		OutXLOOP:for(int x=0;x<LOOPMapMaxXSize;x++){
			#pragma HLS loop_tripcount min=5 max=5
			FilterLOOP:for(int f=0; f<LOOPFilterMaxN; f++){
				#pragma HLS loop_tripcount min=2 max=2
				KernelYLOOP:for(int ky=0; ky<LOOPKernelMaxSize; ky++){
					#pragma HLS loop_tripcount min=3 max=3
					KernelXLOOP: for(int kx=0; kx<LOOPKernelMaxSize; kx++){
						#pragma HLS loop_tripcount min=3 max=3
						ChannelLOOP:for(int kn=0; kn<LOOPKernelMaxN; kn+=itersPerStream){
						#pragma HLS loop_tripcount min=64 max=64
							for(int w=0;w<itersPerStream;w++){
								#pragma HLS pipeline
								#pragma HLS unroll
								if(y>=outMapYSize) y=LOOPMapMaxYSize;
								else if(x>=mapSizeX) x=LOOPMapMaxXSize;
								else if(f>=filterN) f=LOOPFilterMaxN;
								else if(ky>=kernelSize) ky=LOOPKernelMaxSize;
								else if(kx>=kernelSize) kx=LOOPKernelMaxSize;
								else if(kn>=kernelN) kn=LOOPKernelMaxN;
								else{
									if(w==0){
										//printf("kn=%d\n",kn);
										featureMapPacked = featureMap[(((y+ky)&0x03)*mapSizeX*(kernelN/actsPerStream+1))+(kx+x)*(kernelN/actsPerStream+1)+kn/actsPerStream];
										filterPacked = filter[f*kernelSize*kernelSize*(kernelN/weightsPerStream+1)+ky*kernelSize*(kernelN/weightsPerStream+1)+kx*(kernelN/weightsPerStream+1)+kn/actsPerStream];
										//for(uint i=actsPerStream;i>0;i--){
										//	printf("feature map %d\n",(uint)featureMapPacked.range( (i)*WWidth-1, ((i-1)*WWidth)) );
										//}
										//for(uint i=actsPerStream;i>0;i--){
										//	printf("filter %d\n",(uint)filterPacked.range( (i)*WWidth-1, ((i-1)*WWidth)) );
										//}
										//printf("addr = %d\n",(((y+ky)&0x03)*mapSizeX*(kernelN/actsPerStream+1))+(kx+x)*(kernelN/actsPerStream+1)+kn/actsPerStream);
									}
									if(ky==0 && kx==0 && kn == 0 && w==0 && x < outMapXSize){
										accum = bias;
										if(f==0) address2=0;
									}
									if(kx==0 && kn==0){
										address0=(((y+ky)&0x03)*addressSuplement)+x*kernelN;
										if(f==0) address1=(((y+3)&0x03)*addressSuplement)+x*kernelN;

									}


									if(x < outMapXSize){
										//accum+=featureMap[address0++]*filter[address2++];
										dotProduct(featureMapPacked.range((w+1)*AWidth-1,(w*AWidth)), filterPacked.range( (w+1)*WWidth-1, (w*WWidth) ), &accum);
									}


									if(ky==kernelSize-1 && kx==kernelSize-1){
										if(y<outMapYSize-1 && f==0 && w== 0){
											readActs(featureMap,(((y+3)&0x03)*mapSizeX*(kernelN/actsPerStream+1))+x*(kernelN/actsPerStream+1)+kn/actsPerStream,strm_in);
											//printf("writing to addr =%d\n",(((y+3)&0x03)*mapSizeX*(kernelN/actsPerStream+1))+x*(kernelN/actsPerStream+1));

										}
										if(kn+w>=kernelN-1 && w== itersPerStream-1 && x < outMapXSize){
											if(relu && (accum < 0)) accum =0;
											//printf("\t\tOut is %u\n",(ushort)accum);
											outValues=(outValues<<AWidth)+accum.range(AWidth-1,0);
											if(f==filterN-1 || (f+1)%itersPerStream==0){
												//printf("sent %lu\n",(long)outValues);
												if(f==filterN-1){
													//printf("f %d\n",f);
													//printf("shifted last value %d\n",(AWidth*(~f&(itersPerStream)-1)));
													//printf("before %lu\n",(long)outValues);
													outValues=(outValues<<(AWidth*(~f&(itersPerStream)-1)));
													//printf("after %lu\n",(long)outValues);
												}
												tmpo.data = outValues;
												tmpo.keep = 0xF;
												tmpo.strb = 0xF;
												tmpo.last = (!(y<outMapYSize-1) && !(x<outMapXSize-1));
												strm_out.write(tmpo);
												outValues=0;
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
	}
}
