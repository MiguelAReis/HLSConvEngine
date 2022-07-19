 #include <ap_int.h>
#include <stdint.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "convParameters.h"

//#define DEBUG


typedef ap_int<itersPerStream*WWidth> filterDataType;
typedef ap_int<itersPerStream*AWidth> actDataType;
typedef ap_int<AWidth+WWidth> accDataType;


typedef ap_axis<64, 0, 0, 0> axisStream;

void readBias(actDataType bias[FilterMaxN],uint totalbias, hls::stream<axisStream> &strm_in){

	SaveBiasLOOP:for(int i=0;i<totalbias;i++){
			#pragma HLS loop_tripcount min=2 max=2
			#pragma HLS pipeline II=1
			axisStream tmp = strm_in.read(); //Save Bias
			bias[i]=tmp.data;
			//printf("bias %d\n",(int)bias[i]);
		}

}

void readWeights(filterDataType filter[FilterMaxNPerPE*KernelMaxSize*KernelMaxSize*KernelMaxN], uint address, hls::stream<axisStream> &strm_in)
{
	axisStream tmp = strm_in.read();
	filter[address]=tmp.data.range(DMAWidth-1,WRemainder);


}

void readInitialWeights(filterDataType filter[numPEs][FilterMaxNPerPE*KernelMaxSize*KernelMaxSize*KernelMaxN],uint totalWeights, uint filterSize, hls::stream<axisStream> &strm_in){


	unsigned int arrayIndex=0;
	unsigned int jSupplement=0;
	unsigned int j;
	SaveFilterLOOP:for(int i=0;i<totalWeights;i++){
		#pragma HLS loop_tripcount min=(5*3*3*3) max=(5*3*3*3)
		#pragma HLS pipeline II=1
		readWeights(filter[arrayIndex],j,strm_in);
		j++;
		if(j==jSupplement+filterSize){
			arrayIndex++;
			if(arrayIndex == numPEs){
				arrayIndex=0;
				jSupplement+=filterSize;
			}else{
				j=jSupplement;
			}
		}


	}
}

void readActs(actDataType featureMap[MapMaxN*(MapMaxYSize+1)*MapMaxXSize], uint address, hls::stream<axisStream> &strm_in){
	axisStream tmp = strm_in.read(); //Save Bias
	featureMap[address]=tmp.data.range(DMAWidth-1,ARemainder);
}

void readInitialFeatureMap(actDataType featureMap[MapMaxN*(MapMaxYSize+1)*MapMaxXSize], uint totalActs, hls::stream<axisStream> &strm_in){

	SaveMapLOOP:for(int i=0;i<totalActs;i++){
		#pragma HLS loop_tripcount min=(3*5*3) max=(3*5*3)
		#pragma HLS PIPELINE II=1
		readActs(featureMap,i,strm_in);
	}
}

void PE( actDataType featureMapValue,filterDataType filterValue, accDataType *accum){

	//printf("NEW SIMD\n");
	PELOOP:for(int w=0;w<itersPerStream;w++){
		#pragma HLS inline
		#pragma HLS unroll
		*accum+=featureMapValue.range((w+1)*AWidth-1,(w*AWidth))* filterValue.range((w+1)*WWidth-1,(w*WWidth));
		//printf("Doing mac map %d  filter %d  result %d.\n\n",(int)featureMapValue.range((w+1)*AWidth-1,(w*AWidth)),(int)filterValue.range((w+1)*WWidth-1,(w*WWidth)),(int)*accum);
	}

	//printf("\t\tfilter %d featureMap %d accum %d\n",(uint)filterValue,(uint)featureMapValue,(int)*accum);
}

void conv(hls::stream<axisStream> &strm_in,
		hls::stream<axisStream> &strm_out,
		int filterN,
		int kernelN,
		int kernelSize,
		int mapSizeX,
		int mapSizeY,
		int stride,
		int padding,
		bool relu){

	#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS INTERFACE axis port=strm_in
	#pragma HLS INTERFACE axis port=strm_out

	actDataType bias[FilterMaxN];
	PRAGMA_HLS(HLS array_partition variable=bias cyclic factor=numPEs dim=1);
	filterDataType filter[numPEs][FilterMaxNPerPE*KernelMaxSize*KernelMaxSize*KernelMaxN];
	PRAGMA_HLS(HLS array_partition variable=filter block factor=numPEs dim=1);
	actDataType featureMap[MapMaxN*(MapMaxYSize+1)*MapMaxXSize];

	accDataType accum[numPEs];
	#pragma HLS array_partition variable=accum complete


	axisStream tmp,tmpo;

	unsigned short commonDiv=((kernelN-1)/itersPerStream);
	short outMapYSize = mapSizeY-kernelSize+1;
	short outMapXSize = mapSizeX-kernelSize+1;
	ap_int<64> featureMapPacked, filterPacked, outValues=0;
	int filterAddressMax=kernelSize*kernelSize*(commonDiv+1);
	int filterAddressMaxSuplement=0;


	//tmp = strm_in.read(); //Save Bias
	//bias = tmp.data;

	//printf("Reading %d values\n",filterN*kernelSize*kernelSize*(kernelN/weightsPerStream+1));
	readBias(bias,filterN,strm_in);
	readInitialWeights(filter,filterN*kernelSize*kernelSize*(commonDiv+1),filterAddressMax,strm_in);

	readInitialFeatureMap(featureMap,3*mapSizeX*(commonDiv+1),strm_in);








	int filterAddress=0;
	int filterAddress_=0;
	int featureMapAddress=0;
	int featureMapAddressSuplement0=0;
	int featureMapAddressSuplement1=0;
	int yLine[4] ={0,mapSizeX*(commonDiv+1),2*mapSizeX*(commonDiv+1),3*mapSizeX*(commonDiv+1)};
	#pragma HLS array_partition variable=yLine complete
	int featureMapSaveAdddress=0;
	int biasSupplement=0;
	bool active=1;
	unsigned short itersInactiveX=99;
	unsigned short itersInactiveY=99;


	OutYLOOP:for(int y=0;y<LOOPMapMaxYSize;y++){
		#pragma HLS loop_tripcount min=5 max=5
		OutXLOOP:for(int x=0;x<LOOPMapMaxXSize;x++){
			#pragma HLS loop_tripcount min=5 max=5
			FilterLOOP:for(int f=0; f<LOOPFilterMaxN; f++){
				PRAGMA_HLS(HLS loop_tripcount min=64/numPEs max=64/numPEs);
				KernelYLOOP:for(int ky=0; ky<LOOPKernelMaxSize; ky++){
					#pragma HLS loop_tripcount min=3 max=3
					KernelXLOOP: for(int kx=0; kx<LOOPKernelMaxSize; kx++){
						#pragma HLS loop_tripcount min=3 max=3
						ChannelLOOP:for(int kn=0; kn<LOOPKernelMaxN; kn+=itersPerStream){
						PRAGMA_HLS(HLS loop_tripcount min=512/itersPerStream max=512/itersPerStream);
							PRAGMA_HLS(HLS pipeline II=IIValue);
							if(y>=outMapYSize){  y=LOOPMapMaxYSize;}
							else if(x>=mapSizeX){ x=LOOPMapMaxXSize;}
							else if(f>=(filterN-1)/numPEs+1){  f=LOOPFilterMaxN;}
							else if(ky>=kernelSize){ky=LOOPKernelMaxSize;}
							else if(kx>=kernelSize){kx=LOOPKernelMaxSize;}
							else if(kn>=kernelN){kn=LOOPKernelMaxN;}
							else{
								//Address Generation

								if(filterAddress>= filterAddressMax+filterAddressMaxSuplement){
									if(ky==0 && kx==0 && kn ==0){
										if(f==0) {
											filterAddressMaxSuplement=0;
											biasSupplement=0;
										}
										else{
											filterAddressMaxSuplement+=filterAddressMax;
											biasSupplement+=numPEs;
										}
									}
									filterAddress=filterAddressMaxSuplement;
								}
								if(kn==0){
									if(f==0 && ky==0 && kx==0 && kn==0 && x!=0)featureMapAddressSuplement0+=(commonDiv+1);
									else if(f==0 && ky==0 && kx==0 && kn==0)featureMapAddressSuplement0=0;
									if(kn==0 && kx!=0)featureMapAddressSuplement1+=(commonDiv+1);
									else if(kn==0) featureMapAddressSuplement1=0;
								}
								if(kx==0 && kn==0) featureMapAddress=yLine[(y+ky)&0x03]+featureMapAddressSuplement0+featureMapAddressSuplement1;//(kx+x)*(commonDiv+1);
								if(f==0 && kn==0) featureMapSaveAdddress=yLine[(y+3)&0x03]+featureMapAddressSuplement0;//x*(commonDiv+1);
								featureMapPacked = featureMap[featureMapAddress++]; //(((y+ky)&0x03)*mapSizeX*(kernelN/actsPerStream+1))+(kx+x)*(kernelN/actsPerStream+1)+kn/actsPerStream
								filterAddress_=filterAddress++;
								//filterPacked = filter[filterAddress++];
								//printf("filter address is %d\n",filterAddress);
								//printf("Map address is %d\n",featureMapAddress-1);
								if(ky==0 && kx==0 && kn==0 && f==0){
									if(!itersInactiveY){
										if(itersInactiveX>=stride-1){
											itersInactiveX=0;
										}
										else{
											itersInactiveX++;
										}
									}

									if(x==0){
										if(itersInactiveY>=stride-1){
											itersInactiveY=0;
											itersInactiveX=0;
										}
										else{
											itersInactiveY++;
										}
									}
									active = !itersInactiveX & !itersInactiveY;
									//printf("x is %d y is %d itersInactiveX is %d itersInactiveY is %d active is %d\n",x,y,itersInactiveX,itersInactiveY,(int)active);

								}
								if(!active){
									ky=kernelSize-1;
									kx=kernelSize-1;
									filterAddress=filterAddressMax+filterAddressMaxSuplement;
									//kn=LOOPKernelMaxN;
								}


								if(ky==0 && kx==0 && kn == 0 && x < outMapXSize && active){ //Accum Reset with bias
									for(short pes=0; pes<numPEs; pes++){
										#pragma HLS unroll
										if(biasSupplement+pes<filterN) accum[pes]=bias[biasSupplement+pes];
										else accum[pes]=0;
										//printf("accum %d index %d f %d pes %d filterN %d \n",(int)bias[f*numPEs+pes],f*numPEs+pes,f,pes,filterN);
									}
								}

								if(x < outMapXSize && kn<kernelN && f<filterN && y<outMapYSize && active){ //Dot producti
									//accum+= featureMapPacked.range((w+1)*AWidth-1,(w*AWidth)) * filterPacked.range((w+1)*WWidth-1,(w*WWidth));
									PEDeploymentLOOP:for(short pes=0;pes<numPEs;pes++){
										#pragma HLS unroll
										//if(f+pes<filterN) printf("pes is %d filter addres is %d f is %d\n",pes,filterAddress_,f);
										if(f+pes<filterN ) PE(featureMapPacked, filter[pes][filterAddress_],&accum[pes]);
									}
								}
								if(y<outMapYSize-1 && f==0 && ky==kernelSize-1 && kx==kernelSize-1 ){
									readActs(featureMap,featureMapSaveAdddress++,strm_in);
									//printf("Reading one value\n");
								}
								if(ky==kernelSize-1 && kx==kernelSize-1 && active){ //Read Next Values

									if(kn+itersPerStream>=kernelN && x < outMapXSize){
										for(short pes=0;pes<numPEs;pes++){
											#pragma HLS unroll
											if(relu && (accum[pes]< 0)) accum[pes] =0;
										}
										short peIndex=0;
										short limit=0;
										OutStreamLoop:for(short streamIters=0;streamIters<streamItersBound;streamIters++){

											if(!(f+limit>=filterN)){
												OutWordLOOP:for(short pes=0;pes<actsPerStream;pes++){
													#pragma HLS unroll
													//printf("\t\tpe %d out is %d\n",peIndex,(int)accum[peIndex].range(AWidth-1,0));
													outValues=(outValues<<AWidth)+accum[peIndex].range(AWidth-1,0);		//mudar range com escala
													peIndex++;
												}
												//printf("\t\tsent stream %lu\n\n",(unsigned long)outValues);
												tmpo.data = outValues<<ARemainder;
												tmpo.keep = 0xF;
												tmpo.strb = 0xF;
												tmpo.last = (!(y<outMapYSize-1) && !(x<outMapXSize-1));
												strm_out.write(tmpo);
												outValues=0;

												limit+=actsPerStream;

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
