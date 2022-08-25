#include <ap_int.h>
#include <stdint.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "convParameters.h"



typedef ap_int<itersPerStream*WWidth> filterDataType;
typedef ap_int<itersPerStream*AWidth> actDataType;
typedef ap_int<biasPerStream*BWidth> biasStreamDataType;
typedef ap_int<accumBitWidth> accDataType;
typedef ap_int<BWidth> biasDataType;
typedef ap_uint<AWidth> PEAct;
typedef ap_int<AWidth+1> PESignedAct;
typedef ap_int<WWidth> PEWeight;
typedef ap_int<WWidth+AWidth> PEWAccum;
typedef ap_uint<AWidth+1> addAct;
typedef ap_axis<64, 0, 0, 0> axisStream;

void readBias(biasStreamDataType bias[256],unsigned int totalbias, hls::stream<axisStream> &strm_in){

	//printf("totalBias is %d\n",totalbias);
	SaveBiasLOOP:for(int i=0;i<totalbias;i++){
			#pragma HLS loop_tripcount min=2 max=2
			#pragma HLS pipeline II=1
			axisStream tmp = strm_in.read(); //Save Bias
			bias[i]=tmp.data.range(DMAWidth-1,BRemainder);
			//printf("bias %d\n",(int)bias[i]);
		}

}

void readWeights(filterDataType filter[4096], unsigned int address, hls::stream<axisStream> &strm_in)
{
	axisStream tmp = strm_in.read();
	filter[address]=tmp.data.range(DMAWidth-1,WRemainder);


}

void readInitialWeights(filterDataType filter[numPEs][4096],unsigned int totalWeights, unsigned int filterSize, hls::stream<axisStream> &strm_in){


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

void readActs(actDataType featureMap[9728], unsigned int address, hls::stream<axisStream> &strm_in){
	axisStream tmp = strm_in.read(); //Save Bias
	featureMap[address]=tmp.data.range(DMAWidth-1,ARemainder);
}

void readInitialFeatureMap(actDataType featureMap[9728], unsigned int lineN,unsigned int paddingN, unsigned int padding,int mapSizeY,  hls::stream<axisStream> &strm_in){

	int xnIters=0;
	int yIters=0;
	int padXUpperBound=lineN-paddingN;
	//printf("lower bound is %d upper bound is %d\n",paddingN,padXUpperBound);

	SaveMapLOOP:for(int i=0;i<(mapSizeY>=3 ? 3 : mapSizeY)*lineN;i++){
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
		fmValue=featureMapValue.range((w+1)*AWidth-1,(w*AWidth));
		if(isMapSigned && fmValue.test(AWidth-1)) fmValue.set(AWidth);

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
		int mapSizeX,
		int mapSizeY,
		int ctrl// 0-1: kernelSize //2-3: stride  4-6: padding 7:isMapSigned 8-11: biasScale 12-14: scale 15: relu 16:tlast 17:loadWeights
		){

	//#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS INTERFACE axis port=strm_in
	#pragma HLS INTERFACE axis port=strm_out
	#pragma HLS INTERFACE s_axilite port=return bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=filterN bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=kernelN bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=mapSizeX bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=mapSizeY bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=ctrl bundle=BUS1

	ap_uint<2> kernelSize= (ctrl&0x03)>>0;
	ap_uint<2> stride= (ctrl&0x0C)>>2;
	ap_uint<3> padding = (ctrl&0x70)>>4;
	bool isMapSigned=(ctrl&0x80);
	ap_int<4> biasScale= (ctrl&0xF00)>>8;
	ap_uint<3> scale= (ctrl&0x7000)>>12;
	bool relu= ((ctrl&0x8000));
	bool lastPixel= ((ctrl&0x10000));
	bool loadWeights=((ctrl&0x20000));
/*
	printf("kernelSize %d\n", kernelSize.to_uint());
	printf("stride %d\n", stride.to_uint());
	printf("padding %d\n", padding.to_uint());
	printf("isMapSigned %d\n", isMapSigned);
	printf("biasScale %d\n", biasScale.to_int());
	printf("scale %d\n", scale.to_uint());
	printf("relu %d\n", relu);
	printf("lastPixel %d\n", lastPixel);
*/
	//biasStreamDataType bias[(BiasMaxN-1/biasPerStream+1)];
	biasStreamDataType bias[256];
	//PRAGMA_HLS(HLS array_partition variable=bias cyclic factor=biasIterFactor dim=1);
	//filterDataType filter[numPEs][FilterMaxNPerPE*KernelMaxSize*KernelMaxSize*((KernelMaxN-1)/itersPerStream+1)];
	filterDataType filter[numPEs][4096];
	PRAGMA_HLS(HLS array_partition variable=filter block factor=numPEs dim=1);
	//actDataType featureMap[((MapMaxN-1)/itersPerStream+1)*(MapMaxYSize+1)*MapMaxXSize];
	actDataType featureMap[9728];


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
	int yPadlowerBound=(int)padding-3<0 ? 0 : (int)padding-3;
	int xPadlowerBound=padding;
	int xPadUpperBound=mapSizeX-padding;
	int yPadUpperBound=mapSizeY-padding-3;


	if(loadWeights){
		//printf("Reading %d values\n",filterN*kernelSize*kernelSize*(kernelN/weightsPerStream+1));
		readBias(bias,((filterN-1)/biasPerStream+1),strm_in); //meters varios bias por palavra de 64 bits
		//printf("Reading filter\n",filterN*kernelSize*kernelSize*(kernelN/weightsPerStream+1));
		readInitialWeights(filter,filterN*(unsigned int)kernelSize*(unsigned int)kernelSize*(commonDiv+1),filterAddressMax,strm_in);
	}
	//printf("Reading fm\n",filterN*kernelSize*kernelSize*(kernelN/weightsPerStream+1));
	readInitialFeatureMap(featureMap,((mapSizeX)*(commonDiv+1)),padding*(commonDiv+1),padding,mapSizeY,strm_in);








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



	OutYLOOP:for(ap_uint<10> y=0;y<(ap_uint<10>)outMapYSize;y++){
		#pragma HLS loop_tripcount min=5 max=5
		OutXLOOP:for(ap_uint<10> x=0;x<(ap_uint<10>)mapSizeX;x++){
			#pragma HLS loop_tripcount min=5 max=5
			FilterLOOP:for(ap_uint<10> f=0; f<(ap_uint<10>)flimit; f++){
				PRAGMA_HLS(HLS loop_tripcount min=64/numPEs max=64/numPEs);
				KernelYLOOP:for(ap_uint<2> ky=0; ky<(ap_uint<2>)kernelSize; ky++){
					#pragma HLS loop_tripcount min=3 max=3
					KernelXLOOP: for(ap_uint<2> kx=0; kx<(ap_uint<2>)kernelSize; kx++){
						#pragma HLS loop_tripcount min=3 max=3
						ChannelLOOP:for(int kn=0; kn<kernelN; kn+=itersPerStream){
						PRAGMA_HLS(HLS loop_tripcount min=512/itersPerStream max=512/itersPerStream);
							PRAGMA_HLS(HLS pipeline II=IIValue);
							if(1){
								//Address Generation

								if(filterAddress>= filterAddressMax+filterAddressMaxSuplement){
									if(ky==0 && kx==0 && kn ==0){
										if(f==0) {
											filterAddressMaxSuplement=0;
											biasSupplement=0;
										}
										else{
											filterAddressMaxSuplement+=filterAddressMax;
											biasSupplement+=biasJump;
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
									//ky=LOOPKernelMaxSize;
									//kx=LOOPKernelMaxSize;
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
										if(f+pes<filterN ){
											//printf(" bias addr is biasSupplement %d + j %d = %d for pe %d\n",biasSupplement,j,biasSupplement+j,pes);
											biasDataType biasVal=bias[biasSupplement+j].range((((biasPerStream-1)-i)+1)*BWidth-1,(((biasPerStream-1)-i)*BWidth));
											accum[pes]+=biasVal;
											//printf("pes %d bias is %d from %ld from address %d\n",pes,accum[pes].to_int(),bias[biasSupplement+j],biasSupplement+j);
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
											if(!(f+limit>=flimit)){
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
													limit++;
													if(lastPixel) tmpo.last = (f+limit>=flimit); //At the end of each pixel z-wise
													else tmpo.last = !(y<outMapYSize-1-(stride-1)) && !(x<outMapXSize-1-(stride-1)) && (f+limit>=flimit) ;
													//printf("last is %d f is %d limit is %d filterN is %d\n",tmpo.last,f,limit,flimit);
													strm_out.write(tmpo);
													//if(tmpo.last) return;


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
