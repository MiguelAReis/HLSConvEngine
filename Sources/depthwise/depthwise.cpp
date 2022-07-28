#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "depthwiseParameters.h"

//#define DEBUG

#define DONTCARE 999


typedef ap_int<itersPerStream*WWidth> filterDataType;
typedef ap_int<itersPerStream*AWidth> actDataType;
typedef ap_int<32> accDataType;
typedef ap_uint<AWidth> PEAct;
typedef ap_int<WWidth> PEWeight;
typedef ap_int<WWidth+AWidth> PEWAccum;

typedef ap_axis<DMAWidth, 0, 0, 0> axisStream;



void readBias(accDataType bias[KernelMaxN],unsigned int totalbias, hls::stream<axisStream> &strm_in){

	SaveBiasLOOP:for(int i=0;i<totalbias;i++){
			#pragma HLS loop_tripcount min=2 max=2
			#pragma HLS pipeline II=1
			axisStream tmp = strm_in.read(); //Save Bias
			bias[i]=tmp.data;
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
		#pragma HLS loop_tripcount min=(5*3*3*3) max=(5*3*3*3)
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
		#pragma HLS loop_tripcount min=(3*5*3) max=(3*5*3)
		#pragma HLS PIPELINE II=1
		if(BRAMxIndex<padding || BRAMxIndex >= padXUpperBound || yIndex < padding) featureMap[BRAMkIndex][BRAMyIndex+BRAMxIndex]=0;
		else readActs(featureMap[BRAMkIndex],BRAMyIndex+BRAMxIndex,strm_in);
		//if(BRAMxIndex<padding || BRAMxIndex >= padXUpperBound || yIndex < padding) printf("padding on bram %d BRAMyIndex %d BRAMxIndex %d addr %d\n",BRAMyIndex+BRAMxIndex,BRAMyIndex,BRAMxIndex,BRAMkIndex);
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
			}else{
				BRAMkIndex=kIndexSupplement;
				BRAMxIndex++;

				if(BRAMxIndex==KernelSize){
					BRAMxIndex=0;
					kIndexSupplement+=numZPositions;
					BRAMkIndex=kIndexSupplement;
				}

			}

		}
	}
}

void PE( actDataType featureMapPE[KernelSizeSquared],filterDataType filterValue[KernelSizeSquared], accDataType *accum,unsigned int PE){

	//printf("NEW SIMD pe %d\n",PE);


	for(int i=0;i<KernelSizeSquared;i++){
		#pragma HLS inline
		#pragma HLS unroll
		//printf("fm is %ld\n",featureMapPE[i].to_long());
		PEAct fmValue=featureMapPE[i].range((((itersPerStream-1)-PE)+1)*AWidth-1,(((itersPerStream-1)-PE)*AWidth));
		//printf("fmValue is %ld\n",fmValue.to_long());
		PEWeight fValue=filterValue[i].range((((itersPerStream-1)-PE)+1)*WWidth-1,(((itersPerStream-1)-PE)*WWidth));
		//printf("fValue is %ld\n",fValue.to_long());
		PEWAccum MAC = fmValue * fValue;

		*accum+= MAC;
		accDataType accumValue = *accum;
		//printf("Doing mac map %d  filter %d  result %d accum is %d.\n\n",fmValue.to_int(),fValue.to_int(),MAC.to_int(),accumValue.to_int());
	}

}


void depthwise(hls::stream<axisStream> &strm_in,
		hls::stream<axisStream> &strm_out,
		int kernelN,
		int mapSizeX,
		int mapSizeY,
		int stride,
		int padding,
		int scale,
		int relu){


	#pragma HLS INTERFACE axis port=strm_in
	#pragma HLS INTERFACE axis port=strm_out
	#pragma HLS INTERFACE s_axilite port=return bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=kernelN bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=kernelSize bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=mapSizeX bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=mapSizeY bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=stride bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=padding bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=scale bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=relu bundle=BUS1

	filterDataType filter[MaxBRAMValuesForKernelMaxN][KernelSizeSquared]; //9BRAM
	PRAGMA_HLS(HLS array_partition variable=filter block factor=(KernelSizeSquared) dim=2);

	//#pragma HLS array_partition variable=filter complete
	actDataType featureMap[MaxBRAMValuesForKernelMaxN*MapMaxXSize][KernelSizeSquaredPlus]; //12BRAM
	PRAGMA_HLS(HLS array_partition variable=featureMap block factor=KernelSizeSquaredPlus dim=2);
	actDataType featureMapPE[KernelSizeSquaredPlus];
	PRAGMA_HLS(HLS array_partition variable=featureMapPE complete);
	accDataType bias[KernelMaxN]; //12BRAM
	PRAGMA_HLS(HLS array_partition variable=bias cyclic factor=numPEs dim=1);
	//#pragma HLS array_partition variable=featureMap complete
	accDataType accum[numPEs];
	#pragma HLS array_partition variable=accum complete


	axisStream tmp,tmpo;
	unsigned int commonDiv =((kernelN-1)/itersPerStream);
	unsigned int totalWeights=0;

	readBias(bias,kernelN, strm_in);

	for(int i=0;i<KernelSizeSquared;i++) totalWeights+=commonDiv+1;
	//printf("Reading Filter totalWeights is %d\n",totalWeights);
	readInitialWeights(filter,totalWeights,commonDiv+1,strm_in);
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
	unsigned int FMBRAMIndex[KernelSizeSquaredPlus][2];
	#pragma HLS array_partition variable=FMBRAMIndex complete
	unsigned int FMBRAMIndex_[KernelSizeSquaredPlus][2];
	#pragma HLS array_partition variable=FMBRAMIndex complete
	unsigned int addr =0;
	unsigned int addrValue=0;
	unsigned int iterCounter=0;
	unsigned int kernelSupplement=0;

    int BRAMIndexRef[4][3]={{1,2,3},
							{4,5,6},
							{7,8,9},
							{10,11,12}};
    int BRAMIndex[4][3]={{1,2,3},
						 {4,5,6},
						 {7,8,9},
						 {10,11,12}};
    int BRAMIndex_[4][3]={{1,2,3},
						 {4,5,6},
						 {7,8,9},
						 {10,11,12}};
    int BRAMAddr[4][3]={{0,0,0},
                        {0,0,0},
                        {0,0,0},
                        {0,0,0}};
    int BRAMAddr_[4][3]={{0,0,0},
                        {0,0,0},
                        {0,0,0},
                        {0,0,0}};
#pragma HLS array_partition variable=BRAMIndexRef complete
#pragma HLS array_partition variable=BRAMIndex complete
#pragma HLS array_partition variable=BRAMAddr complete
#pragma HLS array_partition variable=BRAMIndex_ complete
#pragma HLS array_partition variable=BRAMAddr_ complete
    int BRAMIndexSupplement=0;
    int indexLimitBeginning=0;
	int yPadlowerBound=padding-3<0 ? 0 : padding-3;
	int xPadlowerBound=padding;
	int xPadUpperBound=mapSizeX-padding;
	int yPadUpperBound=mapSizeY-padding-3;
	long outValues=0;
	bool active=1,activey=1,activex=1;
	unsigned short xlimit=0,ylimit=0;
	unsigned short itersInactiveX=99;
	unsigned short itersInactiveY=99;


	OutYLOOP:for(int y=0;y<LOOPMapMaxYSize;y++){
		#pragma HLS loop_tripcount min=5 max=5
		OutXLOOP:for(int x=0;x<LOOPMapMaxXSize;x++){
			#pragma HLS loop_tripcount min=5 max=5
			KernelLOOP:for(int kn=0; kn<LOOPKernelMaxN; kn+=numPEs){
				PRAGMA_HLS(HLS loop_tripcount min=512/numPEs max=512/numPEs);
				PRAGMA_HLS(HLS pipeline II=IIValue);//
				if(y>=outMapYSize){  y=LOOPMapMaxYSize;}
				else if(x>=mapSizeX){ x=LOOPMapMaxXSize;}
				else if(kn>=kernelN){kn=LOOPKernelMaxN;}
				else{
					//Address Generation
					kernelSupplement++;
					if(kn==0){
						kernelSupplement=0;

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
						kn=LOOPKernelMaxN;
					}



					if(x < outMapXSize ){ //Accum Reset with bias
						for(short pes=0; pes<numPEs; pes++){
							#pragma HLS unroll
							if(kn+pes<kernelN ) accum[pes]=bias[kn+pes];
							else accum[pes]=0;

							//if(biasSupplement+pes<kernelN ) printf("resetting accum for pes %d with %d\n",pes,biasSupplement+pes);
							//else printf("pes %d not active for accum reset\n",pes);
						}
					}



					if(x < outMapXSize && kn<kernelN && y<outMapYSize && active){ //Dot producti
						for(int i=0;i<KernelSizeSquaredPlus;i++){
							#pragma HLS unroll
							//featureMapPE[i]=featureMap[FMBRAMIndex_[i][1]][FMBRAMIndex_[i][0]];
							featureMapPE[FMBRAMIndex[i][0]]=featureMap[FMBRAMIndex[i][1]+kernelSupplement][i];
							//printf("fm %d is %ld  BRAM %d addr %d\n",i,featureMapPE[FMBRAMIndex_[i][0]].to_long(),FMBRAMIndex_[i][0],FMBRAMIndex_[i][1]);
							//printf("fm %d is %ld  BRAM %d addr %d\n",FMBRAMIndex[i][0],featureMapPE[FMBRAMIndex[i][0]].to_long(),i,FMBRAMIndex[i][1]);
						}
						//printf("\n\n");

						PEDeploymentLOOP:for(short pes=0;pes<numPEs;pes++){
							#pragma HLS unroll
							if(kn+pes<kernelN ) PE(featureMapPE,filter[kn], &accum[pes],pes);
							//if(kn+pes<kernelN ) printf("dot Product with pes %d final result %ld\n",pes,accum[pes].to_long());
						}
					}

					if(y<outMapYSize-1){
						if(x<xPadlowerBound || x >= xPadUpperBound || y < yPadlowerBound || y>=yPadUpperBound) featureMap[ FMBRAMIndex[KernelSizeSquared][1] ][FMBRAMIndex[KernelSizeSquared][0] ]=0;
						else readActs(featureMap[FMBRAMIndex_[KernelSizeSquared][1]],FMBRAMIndex_[KernelSizeSquared][0] ,strm_in);
						//if(x<xPadlowerBound || x >= xPadUpperBound || y < yPadlowerBound || y>=yPadUpperBound) printf("padding BRAM %d with addr %d\n",FMBRAMIndex[KernelSizeSquared][0],FMBRAMIndex[KernelSizeSquared][1]);
						//else printf("reading for BRAM %d with addr %d\n",FMBRAMIndex[9][0],FMBRAMIndex[9][1]);

					}
					if(active){ //Read Next Values

						if(kn+itersPerStream>=kernelN && x < outMapXSize){
							for(short pes=0;pes<numPEs;pes++){
								#pragma HLS unroll
								if(relu && (accum[pes]< 0)) accum[pes] =0;
								//printf("relu is %d and accum is %d for pe %d\n",relu,accum[pes].to_long(),pes);
							}
							short peIndex=0;
							short limit=0;

							OutStreamLoop:for(short streamIters=0;streamIters<streamItersBound;streamIters++){
								if(!(kn+limit>=kernelN)){
									OutWordLOOP:for(short pes=0;pes<itersPerStream;pes++){
										#pragma HLS unroll
										//printf("\t\tpe %d out is %d from %d\n",peIndex,accum[peIndex].range(AWidth-1+scale,scale).to_int(),accum[peIndex].to_int());
										outValues=(outValues<<AWidth)+accum[peIndex].range(AWidth-1+scale,scale);		//mudar range com escala
										peIndex++;
									}
									outValues=outValues<<ARemainder;
									//printf("\t\tsent stream %lu\n\n",outValues);
									tmpo.data = outValues;
									tmpo.keep = 0xFF;
									tmpo.strb = 0xFF;
									tmpo.last = (!(y<outMapYSize-1) && !(x<outMapXSize-1) && !(kn+limit>=kernelN));
									strm_out.write(tmpo);
									if(tmpo.last) return;
									outValues=0;


									limit+=itersPerStream;

								}
							}
						}
					}

				}
			}
		}
	}

}
