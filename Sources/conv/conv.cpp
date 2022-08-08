 #include <ap_int.h>
#include <stdint.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "convParameters.h"



typedef ap_int<itersPerStream*WWidth> filterDataType;
typedef ap_int<itersPerStream*AWidth> actDataType;
typedef ap_int<biasPerStream*BWidth> biasStreamDataType;
typedef ap_int<DMAWidth> accDataType;
typedef ap_int<BWidth> biasDataType;
typedef ap_uint<AWidth> PEAct;
typedef ap_int<AWidth+1> PESignedAct;
typedef ap_int<WWidth> PEWeight;
typedef ap_int<WWidth+AWidth> PEWAccum;
typedef ap_uint<AWidth+1> addAct;
typedef ap_axis<64, 0, 0, 0> axisStream;

void readBias(biasStreamDataType bias[FilterMaxN],unsigned int totalbias, hls::stream<axisStream> &strm_in){

	SaveBiasLOOP:for(int i=0;i<totalbias;i+=biasPerStream){
			#pragma HLS loop_tripcount min=2 max=2
			#pragma HLS pipeline II=1
			axisStream tmp = strm_in.read(); //Save Bias
			bias[i]=tmp.data.range(DMAWidth-1,BRemainder);
			//printf("bias %d\n",(int)bias[i]);
		}

}

void readWeights(filterDataType filter[FilterMaxNPerPE*KernelMaxSize*KernelMaxSize*KernelMaxN], unsigned int address, hls::stream<axisStream> &strm_in)
{
	axisStream tmp = strm_in.read();
	filter[address]=tmp.data.range(DMAWidth-1,WRemainder);


}

void readInitialWeights(filterDataType filter[numPEs][FilterMaxNPerPE*KernelMaxSize*KernelMaxSize*KernelMaxN],unsigned int totalWeights, unsigned int filterSize, hls::stream<axisStream> &strm_in){


	unsigned int arrayIndex=0;
	unsigned int jSupplement=0;
	unsigned int j=0;
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

void readActs(actDataType featureMap[MapMaxN*(MapMaxYSize+1)*MapMaxXSize], unsigned int address, hls::stream<axisStream> &strm_in){
	axisStream tmp = strm_in.read(); //Save Bias
	featureMap[address]=tmp.data.range(DMAWidth-1,ARemainder);
}

void readInitialFeatureMap(actDataType featureMap[MapMaxN*(MapMaxYSize+1)*MapMaxXSize], unsigned int lineN,unsigned int paddingN, unsigned int padding, hls::stream<axisStream> &strm_in){

	int xnIters=0;
	int yIters=0;
	int padXUpperBound=lineN-paddingN;
	//printf("lower bound is %d upper bound is %d\n",paddingN,padXUpperBound);

	SaveMapLOOP:for(int i=0;i<3*lineN;i++){
		#pragma HLS loop_tripcount min=(3*5*3) max=(3*5*3)
		#pragma HLS PIPELINE II=1
		if(xnIters<paddingN || xnIters >= padXUpperBound || yIters < padding) featureMap[i]=0;
		else readActs(featureMap,i,strm_in);
		//if(xnIters<paddingN || xnIters >= padXUpperBound || yIters < padding)printf("PADDING to index %d xnIters %d yIters %d \n",i,xnIters,yIters);
		//else printf("reading value to index %d xnIters %d yIters %d\n",i,xnIters,yIters);

		xnIters++;
		yIters = xnIters == lineN ? yIters+1: yIters;
		xnIters = xnIters == lineN ? 0 : xnIters;
	}
}

void PE( actDataType featureMapValue,filterDataType filterValue, accDataType *accum, int isMapSigned){

	//printf("NEW SIMD\n");
	PELOOP:for(int w=0;w<itersPerStream;w++){
		#pragma HLS inline
		#pragma HLS unroll
		PESignedAct fmValue=0;
		if(isMapSigned) fmValue=featureMapValue.range((w+1)*AWidth-1,(w*AWidth));
		else fmValue=featureMapValue.range((w+1)*AWidth-1,(w*AWidth))&((1<<AWidth)-1);
		PEWeight fValue=filterValue.range((w+1)*WWidth-1,(w*WWidth));
		PEWAccum MAC = fmValue * fValue;
		*accum+= MAC;
		accDataType accumValue = *accum;
		//printf("Doing mac map %d  filter %d  result %d accum is %d.\n\n",fmValue.to_int(),fValue.to_int(),MAC.to_int(),accumValue.to_int());
	}

	//printf("\t\tfilter %d featureMap %d accum %d\n",(unsigned int)filterValue,(unsigned int)featureMapValue,(int)*accum);
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
		int isMapSigned,
		int biasScale,
		int scale,
		int relu){

	//#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS INTERFACE axis port=strm_in
	#pragma HLS INTERFACE axis port=strm_out
	#pragma HLS INTERFACE s_axilite port=return bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=filterN bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=kernelN bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=kernelSize bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=mapSizeX bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=mapSizeY bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=stride bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=padding bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=isMapSigned bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=scale bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=relu bundle=BUS1


	biasStreamDataType bias[(FilterMaxN-1/biasPerStream+1)];
	PRAGMA_HLS(HLS array_partition variable=bias cyclic factor=biasIterFactor dim=1);
	filterDataType filter[numPEs][FilterMaxNPerPE*KernelMaxSize*KernelMaxSize*KernelMaxN];
	PRAGMA_HLS(HLS array_partition variable=filter block factor=numPEs dim=1);
	actDataType featureMap[MapMaxN*(MapMaxYSize+1)*MapMaxXSize];

	accDataType accum[numPEs];
	#pragma HLS array_partition variable=accum complete

	hls::stream<actDataType>internalStream0;
	#pragma HLS STREAM variable=internalStream0 depth=5
	hls::stream<actDataType>internalStream1;
	#pragma HLS STREAM variable=internalStream1 depth=5
	hls::stream<actDataType>addMapFifo;
	#pragma HLS STREAM variable=addMapFifo depth=5


	axisStream tmp,tmpo;

	unsigned short commonDiv=((kernelN-1)/itersPerStream);
	mapSizeX+=2*padding;
	mapSizeY+=2*padding;
	short outMapYSize = mapSizeY-kernelSize+1;
	short outMapXSize = mapSizeX-kernelSize+1;
	ap_int<64> featureMapPacked= 0, filterPacked= 0, outValues=0;
	int filterAddressMax=kernelSize*kernelSize*(commonDiv+1);
	int filterAddressMaxSuplement=0;
	int yPadlowerBound=padding-3<0 ? 0 : padding-3;
	int xPadlowerBound=padding;
	int xPadUpperBound=mapSizeX-padding;
	int yPadUpperBound=mapSizeY-padding-3;



	//printf("Reading %d values\n",filterN*kernelSize*kernelSize*(kernelN/weightsPerStream+1));
	readBias(bias,filterN,strm_in); //meters varios bias por palavra de 64 bits

	readInitialWeights(filter,filterN*kernelSize*kernelSize*(commonDiv+1),filterAddressMax,strm_in);

	readInitialFeatureMap(featureMap,((mapSizeX)*(commonDiv+1)),padding*(commonDiv+1),padding,strm_in);








	int filterAddress=0;
	int filterAddress_=0;
	int featureMapAddress=0;
	int featureMapAddressSuplement0=0;
	int featureMapAddressSuplement1=0;

	/*
	printf("mapSizeX = %d\n",mapSizeX);
	printf("mapSizeY = %d\n",mapSizeY);
	printf("yPadlowerBound = %d\n",yPadlowerBound);
	printf("yPadUpperBound = %d\n",yPadUpperBound);
	printf("xPadlowerBound = %d\n",xPadlowerBound);
	printf("xPadUpperBound = %d\n",xPadUpperBound);
	*/
	int yLine[4] ={0,mapSizeX*(commonDiv+1),2*mapSizeX*(commonDiv+1),3*mapSizeX*(commonDiv+1)};
	#pragma HLS array_partition variable=yLine complete
	int featureMapSaveAdddress=0;
	int biasSupplement=0;
	bool active=1,activey=1,activex=1;
	unsigned short xlimit=0,ylimit=0;
	unsigned short itersInactiveX=99;
	unsigned short itersInactiveY=99;
	unsigned short flimit=(filterN-1)/numPEs+1;
	//printf("flimit %d\n",flimit);



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
							else if(f>=flimit){  f=LOOPFilterMaxN;}
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
											biasSupplement++;
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

									if(x==0){
										xlimit=0;
										if(y==ylimit){
											activey=1;
											ylimit+=stride;
										}else{
											activey=0;
										}
									}
									if(x==xlimit){
										activex=1;
										xlimit+=stride;
									}else{
										activex=0;
									}

									active = activex && activey;
									//printf("x is %d y is %d activex is %d activey is %d active is %d\n",x,y,(int)activex,(int)activey,(int)active);

								}
								if(!active){
									ky=LOOPKernelMaxSize;
									kx=LOOPKernelMaxSize;
									filterAddress=filterAddressMax+filterAddressMaxSuplement;
								}


								if(ky==0 && kx==0 && kn == 0 && x < outMapXSize ){ //Accum Reset with bias
									for(short pes=0,i=0,j=0; pes<numPEs; pes++,i++){
										#pragma HLS unroll
										if(i==biasPerStream){
											i=0;
											j++;
										}
										accum[pes]=0;
										if(kn+pes<kernelN ){
											biasDataType biasVal=bias[kn+j].range((((biasPerStream-1)-i)+1)*BWidth-1,(((biasPerStream-1)-i)*BWidth));
											accum[pes]+=biasVal;
											accum[pes]=accum[pes]<<biasScale;
										}
									}
								}

								if(x < outMapXSize && kn<kernelN && f<filterN && y<outMapYSize && active){ //Dot producti
									//accum+= featureMapPacked.range((w+1)*AWidth-1,(w*AWidth)) * filterPacked.range((w+1)*WWidth-1,(w*WWidth));
									PEDeploymentLOOP:for(short pes=0;pes<numPEs;pes++){
										#pragma HLS unroll
										//if(f+pes<filterN) printf("pes is %d filter addres is %d f is %d\n",pes,filterAddress_,f);
										if(f+pes<filterN ) PE(featureMapPacked, filter[pes][filterAddress_],&accum[pes],isMapSigned);
									}
								}
								if(y<outMapYSize-1 && f==0 && ky>=kernelSize-1 && kx>=kernelSize-1  ){
									if(x<xPadlowerBound || x >= xPadUpperBound || y < yPadlowerBound || y>=yPadUpperBound) featureMap[featureMapSaveAdddress++]=0;
									else readActs(featureMap,featureMapSaveAdddress++,strm_in);
									//if(x<xPadlowerBound || x >= xPadUpperBound || y < yPadlowerBound || y>=yPadUpperBound) printf("PADDING value at x=%d and y=%d\n",x,y);
									//else printf("reading value at x=%d and y=%d\n",x,y);
									//printf("Reading one value\n");
								}

								if(ky==kernelSize-1 && kx==kernelSize-1 && active){ //Read Next Values
									if(kn+itersPerStream>=kernelN && x < outMapXSize){
										for(short pes=0,j=0,i=0;pes<numPEs;pes++){
											#pragma HLS unroll
											if(relu && (accum[pes]< 0)) accum[pes] =0;
											else accum[pes] =accum[pes]>>scale;
											//printf("relu is %d and accum is %d for pe %d\n",relu,accum[pes].to_int(),pes);
										}
										short peIndex=0;
										short limit=0;

										OutStreamLoop:for(short streamIters=0;streamIters<streamItersBound;streamIters++){
											if(!(f+limit>=filterN)){
												OutWordLOOP:for(short pes=0;pes<itersPerStream;pes++){
													#pragma HLS unroll
													//printf("\t\tpe %d out is %d from %d\n",peIndex,accum[peIndex].range(AWidth-1+scale,scale).to_int(),accum[peIndex].to_int());
													outValues=(outValues<<AWidth)+accum[peIndex].range(AWidth-1,0);		//mudar range com escala
													peIndex++;
												}
												//printf("\t\tsent stream %lu\n\n",outValues.to_int());

													tmpo.data = outValues<<ARemainder;
													tmpo.keep = 0xFF;
													tmpo.strb = 0xFF;
													tmpo.last = (f+limit>=filterN); //At the end of each pixel z-wise
													strm_out.write(tmpo);
													//if(tmpo.last) return;


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
