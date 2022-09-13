#include <stdio.h>
#include <stdlib.h>
#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "depthwiseParameters.h"

#define tbFilterN 64
#define tbFilterSize 3
#define tbInputMapSize 150
#define tbStride 1
#define tbPadding 1
#define tbOutputMapSize ((tbInputMapSize - tbFilterSize+ 2*tbPadding)/tbStride + 1)
#define tbScale 6
#define tbBiasScale 0
#define tbRelu 0
#define tbIsMapSigned 0
#define tbTlast 1



typedef ap_int<WWidth> filterDataType;
typedef ap_int<AWidth> actDataType;
typedef ap_axis<DMAWidth, 0, 0, 0> axisStream;

static long filter[tbFilterN*tbFilterSize*tbFilterSize];
static long inputMap[tbFilterN*tbInputMapSize*tbInputMapSize];
long outputMap[tbOutputMapSize*tbOutputMapSize];

void depthwise(hls::stream<axisStream> &strm_in,
		hls::stream<axisStream> &strm_out,
		int kernelN,
		int mapSizeX,
		int mapSizeY,
		int ctrl);


void print_inputMat(actDataType *x, int rows, int cols, int channels){
	int i;
	for(int c=0;c<channels;c++){
		for (i=0; i<rows; i++) {
			for (int j=0; j<cols; j++) {
				printf("%5d ", (int)x[3*(i*rows+j)+c]);
			}
			printf("\n");
		}
		printf("\n\n\n");
	}
}

void print_Map(long *x, int rows, int cols, int channels,bool relud){
	int i;
	int addrSuplement =0;
	int shifting= itersPerStream;
	int addr=0;
	//printf("starting shifting is %d\n",shifting);
	for(int c=channels; c>0 ;c--){ //(tbKernelN/itersPerStream+1)  ((channels-1)/itersPerStream+1)*itersPerStream;c>((~(channels-1)&(itersPerStream-1)))
		shifting--;
		addr=0;
		int value=0;
		int signedBit=0;
		if(shifting <0) shifting=itersPerStream-1;
		for (i=0; i<rows; i++) {
			for (int j=0; j<cols; j++) {

				//printf("shifitng is %d   addr suplement is %d",shifting  , addrSuplement);


				//printf("%d ",addr+addrSuplement);//(i*rows*((channels/itersPerStream+1)/itersPerStream)+j*((channels/itersPerStream+1)/itersPerStream))+addrSuplement
				value=((x[addr+addrSuplement]>>ARemainder)>>(shifting)*AWidth)&(((long)1<<AWidth)-1);
				signed bit=(value&((1<<AWidth)-1))>>(AWidth-1);
				//printf("bit is %d for value %d\n",bit,value);
				if(bit&&relud){
					value=(int)(((1<<32-AWidth)-1)<<AWidth)+(int)value;
				}
				printf("%hd  ",value);//(x[(i*rows+j)+c/actsPerStream]>>(AWidth*(c&(actsPerStream-1))))&((1<<AWidth)-1)  &(((long)1<<AWidth)-1)
				addr+=((channels-1)/itersPerStream+1);
			}
			printf("\n");
		}
		printf("\n\n\n");
		if(shifting%itersPerStream== 0) addrSuplement++;

	}
}

void print_Filter(long *x, int rows, int cols, int channels){
	int i;
	int addrSuplement =0;
	int shifting= itersPerStream;
	int addr=0;
	//printf("starting shifting is %d\n",shifting);
	for(int c=channels; c>0 ;c--){ //(tbKernelN/itersPerStream+1)  ((channels-1)/itersPerStream+1)*itersPerStream;c>((~(channels-1)&(itersPerStream-1)))
		shifting--;
		addr=0;
		int value=0;
		int signedBit=0;
		if(shifting <0) shifting=itersPerStream-1;
		for (i=0; i<rows; i++) {
			for (int j=0; j<cols; j++) {

				//printf("shifitng is %d   addr suplement is %d\n",shifting  , addrSuplement);


				//printf("%d ",addr+addrSuplement);//(i*rows*((channels/itersPerStream+1)/itersPerStream)+j*((channels/itersPerStream+1)/itersPerStream))+addrSuplement
				value=((x[addr+addrSuplement]>>WRemainder)>>(shifting)*WWidth)&(((long)1<<WWidth)-1);
				signed bit=(value&((1<<WWidth)-1))>>(WWidth-1);
				//printf("bit is %d for value %d\n",bit,value);
				if(bit){
					value=(int)(((1<<32-WWidth)-1)<<WWidth)+(int)value;
				}
				printf("%hd  ",value);//(x[(i*rows+j)+c/actsPerStream]>>(AWidth*(c&(actsPerStream-1))))&((1<<AWidth)-1)  &(((long)1<<AWidth)-1)
				addr+=((channels-1)/itersPerStream+1);
			}
			printf("\n");
		}
		printf("\n\n\n");
		if(shifting%itersPerStream== 0) addrSuplement++;

	}
}

void initVectors(){


	for (int i=0; i<(tbInputMapSize*tbInputMapSize); i++) {
		for(int k=0; k<((tbFilterN-1)/itersPerStream+1);k++){
			unsigned long value= 0;
			for(int y=0;y<itersPerStream;y++){
				//printf("k*itersPerStream+y %d\n",k*itersPerStream+y);
				if(k*itersPerStream+y<tbFilterN){
					value= (value<<AWidth)+(tbInputMapSize*tbInputMapSize)*(k*itersPerStream+y)+i;
					//printf("put %d into %lu\n",(tbInputMapSize*tbInputMapSize)*(k*itersPerStream+y)+i,value);
				}else{
					value= (value<<AWidth);
					//printf("shifted %d into %lu",AWidth,value);
				}
			} //arranjar shifts quando width n e potencia 2
			inputMap[i*((tbFilterN-1)/itersPerStream+1)+k] = value<<ARemainder;
			//printf("%lu \n",inputMap[i*((tbKernelN-1)/itersPerStream+1)+k]);
		}

	}

	for (int i=0; i<(tbFilterSize*tbFilterSize); i++) {
		for(int k=0; k<((tbFilterN-1)/itersPerStream+1);k++){
			unsigned long value= 0;
			for(int y=0;y<itersPerStream;y++){
				//printf("k*itersPerStream+y %d\n",k*itersPerStream+y);
				if(k*itersPerStream+y<tbFilterN){
					value= (value<<WWidth)+(tbFilterSize*tbFilterSize)*(k*itersPerStream+y)+i;
					//printf("put %d into %lu\n",(f*tbFilterSize*tbFilterSize*tbKernelN)+(tbFilterSize*tbFilterSize)*(k*itersPerStream+y)+i,value);
				}else{
					value= (value<<WWidth);
					//printf("shifted %d into %lu",WWidth,value);
				}
			}
			filter[i*((tbFilterN-1)/itersPerStream+1)+k] = value<<WRemainder;
			//printf("\t%d  %lu\n",i*((tbFilterN-1)/itersPerStream+1)+k,filter[i*((tbFilterN-1)/itersPerStream+1)+k]);
		}

	}




}
int main()
{

	hls::stream<axisStream> str_in("inputStream"); //("sinp");
	hls::stream<axisStream> str_out("outputStream"); //("sout");
	axisStream tmp, tmpa;

	printf("Beginning testbench\n");

	initVectors();

	printf("Input Map\n");
	//print_Map(inputMap, tbInputMapSize, tbInputMapSize,tbFilterN,0);
	printf("Filters \n");
	//print_Filter(filter, tbFilterSize, tbFilterSize,tbFilterN);

	for(int i=0;i<((tbFilterN-1)/biasPerStream+1);i++){
		tmp.data=(ap_int<64>)0;
		str_in.write(tmp);
	}
	printf("Sent whole Bias N =%d\n",tbFilterN);

	for (int i=0; i<(tbFilterSize*tbFilterSize*((tbFilterN-1)/itersPerStream+1)); i++) {
		//tmp.data=(ap_int<64>)filter[i];
		tmp.data=(ap_int<64>)4;
		str_in.write(tmp);
		//printf("%d %lu\n",i,filter[i]);
	}
	printf("Sent whole Filter N =%d\n",(tbFilterSize*tbFilterSize*((tbFilterN-1)/itersPerStream+1)));
	for (int y =0 ;y<tbInputMapSize*tbInputMapSize*((tbFilterN-1)/itersPerStream+1); y++) {
		//tmp.data=(ap_int<64>)inputMap[y];
		tmp.data=(ap_int<64>)5;
		//if(y == tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1)-1) tmp.last=1;
		//else tmp.last=0;
		str_in.write(tmp);
		//printf("%lu\n",inputMap[y]);
	}


	printf("Sent whole Input Map N =%d\n",tbInputMapSize*tbInputMapSize*((tbFilterN-1)/itersPerStream+1));
	//0-1: stride  2-4: padding  5: isMapSigned 6-9:biasScale 10-12: scale 13:relu 14:tlast
	int ctrl=(tbStride)+(tbPadding<<2)+(tbIsMapSigned<<5)+(tbBiasScale<<6)+(tbScale<<10)+(tbRelu<<13)+(tbTlast<<14)+(1<<15);
	//ctrl=6162;
	printf("ctr is %d\n",ctrl);
	depthwise(str_in, str_out,tbFilterN,tbInputMapSize,tbInputMapSize,ctrl);


	for (int i=0; i<tbOutputMapSize*tbOutputMapSize*((tbFilterN-1)/itersPerStream+1); i++) {
		tmpa = str_out.read();
		outputMap[i] = tmpa.data;
		printf("%d %lu\n",i,outputMap[i]);
		if(tmpa.last)printf("LAST\n");
	}

	printf("Read whole Output Map N =%d\n",tbOutputMapSize*tbOutputMapSize*((tbFilterN-1)/itersPerStream+1));
	print_Map(outputMap,tbOutputMapSize,tbOutputMapSize,tbFilterN,0);

	return 0;
}
