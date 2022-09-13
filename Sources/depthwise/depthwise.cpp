#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "depthwiseParameters.h"

//#define DEBUG

#define DONTCARE 999


typedef ap_int<itersPerStream*WWidth> filterDataType;
typedef ap_int<itersPerStream*AWidth> actDataType;
typedef ap_int<biasPerStream*BWidth> biasStreamDataType;
typedef ap_int<BWidth> biasDataType;
typedef ap_int<accumBitWidth> accDataType;
typedef ap_uint<AWidth> PEAct;
typedef ap_int<AWidth+1> PESignedAct;
typedef ap_int<WWidth> PEWeight;
typedef ap_int<WWidth+AWidth> PEWAccum;

typedef ap_axis<DMAWidth, 0, 0, 0> axisStream;



void readBias(biasStreamDataType bias[KernelMaxN],unsigned int totalbias, hls::stream<axisStream> &strm_in){

	SaveBiasLOOP:for(int i=0;i<totalbias;i++){
			#pragma HLS loop_tripcount min=128 max=128
			#pragma HLS pipeline II=1
			axisStream tmp = strm_in.read(); //Save Bias
			bias[i]=tmp.data.range(DMAWidth-1,BRemainder);//.range((((biasPerStream-1)-j)+1)*BWidth-1,(((biasPerStream-1)-j)*BWidth));;
			//printf("bias %d\n",i);
		}

}

void readWeights(filterDataType filter[KernelSizeSquared], unsigned int address, hls::stream<axisStream> &strm_in)
{
	axisStream tmp = strm_in.read();
	filter[address]=tmp.data.range(DMAWidth-1,WRemainder);


}

void readInitialWeights(filterDataType filter[MaxBRAMValuesForKernelMaxN][KernelSizeSquared],unsigned int totalWeights,unsigned commonDiv, hls::stream<axisStream> &strm_in){


	unsigned int j=0;
	unsigned int BRAMIndex=0;

	//printf("MaxBRAMs is %d\n",MaxBRAMs);
	SaveFilterLOOP:for(int i=0;i<totalWeights;i++){
		#pragma HLS loop_tripcount min=(576) max=(576)
		#pragma HLS pipeline II=1
		readWeights(filter[j],BRAMIndex,strm_in);
		//printf("reading %ld on bram %d addr %d\n",filter[j][BRAMIndex].to_long(),BRAMIndex,j);
		j++;
		if(j==commonDiv){
			BRAMIndex++;
			j=0;

		}
	}
}

void readActs(actDataType featureMap[KernelSizeSquared+KernelSize], unsigned int address, hls::stream<axisStream> &strm_in){
	axisStream tmp = strm_in.read(); //Save Bias
	featureMap[address]=tmp.data.range(DMAWidth-1,ARemainder);
}

void readInitialFeatureMap(actDataType featureMap[MaxBRAMValuesForKernelMaxN*MapMaxXSize][KernelSizeSquared+KernelSize], unsigned int lineN,unsigned int padding, unsigned int numZPositions, hls::stream<axisStream> &strm_in){

	int xnIters=0;
	int yIters=0;
	int padXUpperBound=lineN-padding;
	unsigned int BRAMxIndex=0;
	unsigned int BRAMyIndex=0;
	unsigned int xIndex=0;
	unsigned int yIndex=0;
	unsigned int BRAMkIndex=0;
	unsigned int kIndexSupplement=0;
	unsigned int lineLimit=lineN*numZPositions;
	unsigned int lineLimitSupplement=0;
	//printf("lower bound is %d upper bound is %d\n",padding,padXUpperBound);
	//printf("lineLimit=%d lineN=%d\n",lineLimit,lineN);
	unsigned int limit= 3*lineN*numZPositions;
	SaveMapLOOP:for(int i=0;i<limit;i++){
		#pragma HLS loop_tripcount min=(150*3*64) max=(150*3*64)
		#pragma HLS PIPELINE II=1
		//printf("i is %d x is %d\n",i,xIndex);
		if(xIndex<padding || xIndex >= padXUpperBound || yIndex < padding) featureMap[BRAMkIndex][BRAMyIndex+BRAMxIndex]=0;
		else readActs(featureMap[BRAMkIndex],BRAMyIndex+BRAMxIndex,strm_in);
		//if(xIndex<padding || xIndex >= padXUpperBound || yIndex < padding) printf("padding on bram %d BRAMyIndex %d BRAMxIndex %d addr %d\n",BRAMyIndex+BRAMxIndex,BRAMyIndex,BRAMxIndex,BRAMkIndex);
		//else printf("reading value %ld on bram %d BRAMyIndex %d BRAMxIndex %d addr %d\n",featureMap[BRAMkIndex][BRAMyIndex+BRAMxIndex].to_long(),BRAMyIndex+BRAMxIndex,BRAMyIndex,BRAMxIndex,BRAMkIndex);

		BRAMkIndex++;

		if(BRAMkIndex==numZPositions+kIndexSupplement){
			if(i==lineLimit+lineLimitSupplement-1){
				//printf("Changed Line\n");
				lineLimitSupplement+=lineLimit;
				BRAMyIndex+=KernelSize;
				kIndexSupplement=0;
				BRAMxIndex=0;
				BRAMkIndex=0;
				yIndex++;
				xIndex=0;
			}else{
				BRAMkIndex=kIndexSupplement;
				BRAMxIndex++;
				xIndex++;

				if(BRAMxIndex==KernelSize){
					BRAMxIndex=0;
					kIndexSupplement+=numZPositions;
					BRAMkIndex=kIndexSupplement;
				}

			}

		}
	}
}

void PE( actDataType featureMapPE[KernelSizeSquared],filterDataType filterValue[KernelSizeSquared], accDataType *accum,unsigned int PE,int isMapSigned){

	//printf("NEW SIMD pe %d\n",PE);


	for(int i=0;i<KernelSizeSquared;i++){
		#pragma HLS inline
		#pragma HLS unroll
		//printf("fm is %ld\n",featureMapPE[i].to_long());
		PESignedAct fmValue=0;
		fmValue=featureMapPE[i].range((((itersPerStream-1)-PE)+1)*AWidth-1,(((itersPerStream-1)-PE)*AWidth));
		if(isMapSigned && fmValue.test(AWidth-1)) fmValue.set(AWidth);
		//printf("fmValue is %ld\n",fmValue.to_long());
		PEWeight fValue=filterValue[i].range((((itersPerStream-1)-PE)+1)*WWidth-1,(((itersPerStream-1)-PE)*WWidth));
		//printf("fValue is %ld\n",fValue.to_long());
		//printf("fValue is %d from %d to %d for pes %d\n",i,(((itersPerStream-1)-PE)+1)*WWidth-1,(((itersPerStream-1)-PE)*WWidth), PE);
		PEWAccum MAC = fmValue * fValue;

		*accum+= MAC;
		accDataType accumValue = *accum;
		//printf("Doing mac map %d  filter %d  result %d accum is %d.\n",fmValue.to_int(),fValue.to_int(),MAC.to_int(),accumValue.to_int());




	}

}


void depthwise(hls::stream<axisStream> &strm_in,
		hls::stream<axisStream> &strm_out,
		int kernelN,
		int mapSizeX,
		int mapSizeY,
		int ctrl //0-1: stride  2-4: padding  5: isMapSigned 6-9:biasScale 10-12: scale 13:relu 14:tlast 15: loadWeights
		){


	#pragma HLS INTERFACE axis port=strm_in
	#pragma HLS INTERFACE axis port=strm_out
	#pragma HLS INTERFACE s_axilite port=return bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=kernelN bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=mapSizeX bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=mapSizeY bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=ctrl bundle=BUS1


	ap_uint<2> stride= (ctrl&0x03)>>0;
	ap_uint<3> padding = (ctrl&0x1C)>>2;
	bool isMapSigned=(ctrl&0x20);
	ap_int<4> biasScale= (ctrl&0x3C0)>>6;
	ap_uint<3> scale= (ctrl&0x1C00)>>10;
	bool relu= ((ctrl&0x2000));
	bool lastPixel= ((ctrl&0x4000));
	bool loadWeights = ((ctrl&0x8000));
/*
	printf("stride %d\n", stride.to_uint());
	printf("padding %d\n", padding.to_uint());
	printf("isMapSigned %d\n", isMapSigned);
	printf("biasScale %d\n", biasScale.to_int());
	printf("scale %d\n", scale.to_uint());
	printf("relu %d\n", relu);
	printf("lastPixel %d\n", lastPixel);
*/
	//filterDataType filter[MaxBRAMValuesForKernelMaxN][KernelSizeSquared]; //9BRAM
	filterDataType filter[96][KernelSizeSquared]; //9BRAM
	PRAGMA_HLS(HLS array_partition variable=filter block factor=(KernelSizeSquared) dim=2);

	//#pragma HLS array_partition variable=filter complete
	//actDataType featureMap[MaxBRAMValuesForKernelMaxN*MapMaxXSize][KernelSizeSquaredPlus]; //12BRAM
	actDataType featureMap[591][KernelSizeSquaredPlus]; //12BRAM
	PRAGMA_HLS(HLS array_partition variable=featureMap block factor=KernelSizeSquaredPlus dim=2);
	actDataType featureMapPE[KernelSizeSquaredPlus];
	PRAGMA_HLS(HLS array_partition variable=featureMapPE complete);
	//biasStreamDataType bias[(KernelMaxN-1/biasPerStream+1)]; //12BRAM
	biasStreamDataType bias[192]; //12BRAM
	//PRAGMA_HLS(HLS array_partition variable=bias cyclic factor=biasIterFactor dim=1);
	//#pragma HLS array_partition variable=featureMap complete
	accDataType accum[numPEs];
	#pragma HLS array_partition variable=accum complete


	axisStream tmp,tmpo;
	unsigned int commonDiv =((kernelN-1)/itersPerStream);
	unsigned int totalWeights=0;
	for(int i=0;i<KernelSizeSquared;i++) totalWeights+=commonDiv+1;
	if(loadWeights){
		readBias(bias,((kernelN-1)/biasPerStream+1), strm_in);
		readInitialWeights(filter,totalWeights,commonDiv+1,strm_in);
	}



	//printf("Reading Filter totalWeights is %d\n",totalWeights);

	mapSizeX+=2*padding;
	mapSizeY+=2*padding;
	unsigned int featureMapLimit=mapSizeX;
	//printf("Reading Map featureMapLimit is %d\n",featureMapLimit);
	//printf("Reading Map padding is %d\n",padding);
	//printf("Reading Map commonDiv+1 is %d\n",commonDiv+1);
	readInitialFeatureMap( featureMap, featureMapLimit,padding,commonDiv+1, strm_in);




	unsigned int outMapYSize = mapSizeY-KernelSize+1;
	unsigned int outMapXSize = mapSizeX-KernelSize+1;

	unsigned int yIndex=0;
	unsigned int yIndexInitial=0;
	unsigned int xIndex=0;
	unsigned int xIndexInitial=0;
	ap_uint<16>  FMBRAMIndex[KernelSizeSquaredPlus][2];
	#pragma HLS array_partition variable=FMBRAMIndex complete
	ap_uint<16>  FMBRAMIndex_[KernelSizeSquaredPlus][2];
	#pragma HLS array_partition variable=FMBRAMIndex_ complete
	unsigned int addr =0;
	unsigned int addrValue=0;
	unsigned int iterCounter=0;
	unsigned int kernelSupplement=0;
	unsigned int kernelSupplementFM=0;

    ap_uint<4> BRAMIndexRef[4][3]={{1,2,3},
							{4,5,6},
							{7,8,9},
							{10,11,12}};
    ap_uint<4> BRAMIndex[4][3];
    ap_uint<4> BRAMIndex_[4][3];
    int BRAMAddr[4][3];
    int BRAMAddr_[4][3];

	#pragma HLS array_partition variable=BRAMIndexRef complete
	#pragma HLS array_partition variable=BRAMIndex complete
	#pragma HLS array_partition variable=BRAMAddr complete
	#pragma HLS array_partition variable=BRAMIndex_ complete
	#pragma HLS array_partition variable=BRAMAddr_ complete

    int BRAMIndexSupplement=0;
    int indexLimitBeginning=0;
	int yPadlowerBound=(int)padding-3<0 ? 0 : (int)padding-3;
	int xPadlowerBound=padding;
	int xPadUpperBound=mapSizeX-padding;
	int yPadUpperBound=mapSizeY-padding-3;
	long outValues=0;
	bool active=1,activey=1,activex=1;
	unsigned short xlimit=0,ylimit=0;
	unsigned short itersInactiveX=99;
	unsigned short itersInactiveY=99;


	OutYLOOP:for(ap_uint<10> y=0;y<(ap_uint<10>)outMapYSize;y++){
		#pragma HLS loop_tripcount min=147 max=147
		OutXLOOP:for(ap_uint<10> x=0;x<(ap_uint<10>)mapSizeX;x++){
			#pragma HLS loop_tripcount min=150 max=150
			KernelLOOP:for(int kn=0; kn<kernelN; kn+=numPEs){
				PRAGMA_HLS(HLS loop_tripcount min=1024/numPEs max=1024/numPEs);
				PRAGMA_HLS(HLS pipeline II=IIValue);//
				if(1){
					//Address Generation
					kernelSupplement+= biasJump;
					kernelSupplementFM++;
					if(kn==0){
						kernelSupplement=0;
						kernelSupplementFM=0;
						//printf("New X\n");

						if(x==0){
							//printf("New line\n");
							if(!y)BRAMIndexSupplement+=KernelSize;
							if(BRAMIndexSupplement==KernelSizeSquaredPlus)BRAMIndexSupplement=0;
							for(int i=0;i<4;i++){
								for(int j=0;j<3;j++){
									BRAMIndex[i][j]=BRAMIndexRef[(i-y)&0x03][j];
									BRAMIndex_[i][j]=BRAMIndexRef[(i+y)&0x03][j]-1;
									BRAMAddr[i][j]=0;
									BRAMAddr_[i][j]=0;

								}
							}
						}

						for(int i=0,ii=0;i<4;i++){
							for(int j=0;j<3;j++,ii++){
								FMBRAMIndex[ii][0]=BRAMIndex[i][j]-1; //Bram Index
								FMBRAMIndex[ii][1]=BRAMAddr[i][j];     //Addr
								FMBRAMIndex_[ii][0]=BRAMIndex_[i][j]; //Bram Index
								FMBRAMIndex_[ii][1]=BRAMAddr_[i][j];     //Addr
								//printf("%d-%d  ",FMBRAMIndex[ii][0],FMBRAMIndex[ii][1]);
								//printf("%d-%d  ",FMBRAMIndex_[ii][0],FMBRAMIndex_[ii][1]);
								BRAMIndex[i][j]--;
								if(BRAMIndex[i][j]==BRAMIndexRef[(i-y)&0x03][0]-1){
									BRAMIndex[i][j]=BRAMIndexRef[(i-y)&0x03][2];
									BRAMAddr[i][j]+=commonDiv+1;
								}

								BRAMIndex_[i][j]++;
								if(BRAMIndex_[i][j]==BRAMIndexRef[(i+y)&0x03][2]){
									BRAMIndex_[i][j]=BRAMIndexRef[(i+y)&0x03][0]-1;
									BRAMAddr_[i][j]+=commonDiv+1;
								}

							}
							//printf("\n ");
						}
						//printf("\n\n");

					}

					if(kn==0){
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
						//kn=LOOPKernelMaxN;
					}



					if(x < outMapXSize ){ //Accum Reset with bias
						//printf("New assignment of bias\n");
						for(short pes=0,i=0,j=0; pes<numPEs; pes++,i++){
							#pragma HLS unroll
							if(i==biasPerStream){
								i=0;
								j++;
							}
							accum[pes]=0;
							if(kn+pes<kernelN ){
								//printf("bias addr is %d for filter %d and pes %d\n",kernelSupplement+j,kernelSupplement,pes);
								biasDataType biasVal=bias[kernelSupplement+j].range((((biasPerStream-1)-i)+1)*BWidth-1,(((biasPerStream-1)-i)*BWidth));
								//printf("kernel is %d from %d to %d for pes %d\n",kernelSupplement+j,(((biasPerStream-1)-i)+1)*BWidth-1,(((biasPerStream-1)-i)*BWidth), pes);
								accum[pes]+=biasVal;
								accum[pes]=accum[pes]<<biasScale;
							}


							//if(biasSupplement+pes<kernelN ) printf("resetting accum for pes %d with %d\n",pes,biasSupplement+pes);
							//else printf("pes %d not active for accum reset\n",pes);
						}
					}



					if(x < outMapXSize && kn<kernelN && y<outMapYSize && active){ //Dot producti
						for(int i=0;i<KernelSizeSquaredPlus;i++){
							#pragma HLS unroll
							//featureMapPE[i]=featureMap[FMBRAMIndex_[i][1]][FMBRAMIndex_[i][0]];
							featureMapPE[FMBRAMIndex[i][0]]=featureMap[FMBRAMIndex[i][1]+kernelSupplementFM][i];//+kernelSupplement
							//printf("fm %d is %ld  BRAM %d addr %d\n",i,featureMapPE[FMBRAMIndex_[i][0]].to_long(),FMBRAMIndex_[i][0],FMBRAMIndex_[i][1]);
							//printf("fm %d is %ld  BRAM %d addr %d\n",FMBRAMIndex[i][0],featureMapPE[FMBRAMIndex[i][0]].to_long(),i,FMBRAMIndex[i][1]+kernelSupplementFM);
						}
						//printf("\n\n");

						PEDeploymentLOOP:for(short pes=0;pes<numPEs;pes++){
							#pragma HLS unroll
							if(kn+pes<kernelN ) PE(featureMapPE,filter[kernelSupplementFM], &accum[pes],pes,isMapSigned);
							//printf("kn is %d\n",kernelSupplementFM);
							//if(kn+pes<kernelN ) printf("dot Product with pes %d final result %ld\n",pes,accum[pes].to_long());
						}
					}

					if(y<outMapYSize-1){
						if(x<xPadlowerBound || x >= xPadUpperBound || y < yPadlowerBound || y>=yPadUpperBound) featureMap[ FMBRAMIndex_[KernelSizeSquared][1]+kernelSupplementFM ][FMBRAMIndex_[KernelSizeSquared][0] ]=0;
						else readActs(featureMap[FMBRAMIndex_[KernelSizeSquared][1]+kernelSupplementFM],FMBRAMIndex_[KernelSizeSquared][0] ,strm_in);
						//if(x<xPadlowerBound || x >= xPadUpperBound || y < yPadlowerBound || y>=yPadUpperBound) printf("inside padding BRAM %d with addr %d\n",FMBRAMIndex_[KernelSizeSquared][0],FMBRAMIndex_[KernelSizeSquared][1]+kernelSupplementFM);
						//else printf("inside reading for BRAM %d with addr %d\n",FMBRAMIndex_[9][0],FMBRAMIndex_[9][1]+kernelSupplementFM);

					}
					if(active){ //Read Next Values

						if( x < outMapXSize){
							for(short pes=0;pes<numPEs;pes++){
								#pragma HLS unroll
								if(relu && (accum[pes]< 0)) accum[pes] =0;
								else accum[pes] =accum[pes]>>scale;
								if(relu){
									if(accum[pes]>=(1<<AWidth)-1) accum[pes] = (1<<AWidth)-1;

								}else{
									if(accum[pes]>=(1<<(AWidth-1))-1) accum[pes] = (1<<(AWidth-1))-1;
									else if (accum[pes]<=-(1<<(AWidth-1)) ) accum[pes] = -(1<<(AWidth-1));
								}
								//printf("relu is %d and accum is %d for pe %d\n",relu,accum[pes].to_long(),pes);
							}
							short peIndex=0;
							short limit=0;

							OutStreamLoop:for(short streamIters=0;streamIters<streamItersBound;streamIters++){
								if(!(kn+limit>=kernelN)){
									OutWordLOOP:for(short pes=0;pes<itersPerStream;pes++){
										#pragma HLS unroll
										//printf("\t\tpe %d out is %d from %d\n",peIndex,accum[peIndex].range(AWidth-1+scale,scale).to_int(),accum[peIndex].to_int());
										outValues=(outValues<<AWidth)+accum[peIndex].range(AWidth-1,0);		//mudar range com escala
										peIndex++;
									}
									outValues=outValues<<ARemainder;
									//printf("\t\tsent stream %lu\n\n",outValues);
									tmpo.data = outValues;
									tmpo.keep = 0xFF;
									tmpo.strb = 0xFF;
									limit+=itersPerStream;
									if(lastPixel) tmpo.last =  (kn+limit>=kernelN);
									else tmpo.last = !(y<outMapYSize-1-(stride-1)) && !(x<outMapXSize-1-(stride-1)) && (kn+limit>=kernelN) ;
									//printf("y %d outMapYSize %d x %d outMapXSize %d kn %d limit %d kernelN %d \n",y,outMapYSize,x,outMapXSize,kn,limit,kernelN);
									strm_out.write(tmpo);
									//printf("tlast is %d\n", tmpo.last);
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
