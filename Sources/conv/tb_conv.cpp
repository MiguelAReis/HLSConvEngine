#include <stdio.h>
#include <stdlib.h>
#include <ap_int.h>
#include <cmath>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "convParameters.h"

#define tbFilterN 2
#define tbKernelN 3
#define tbFilterSize 3
#define tbInputMapSize 5
#define tbOutputMapSize (tbInputMapSize - tbFilterSize + 1)


typedef ap_int<WWidth> filterDataType;
typedef ap_int<AWidth> actDataType;
typedef ap_axis<64, 0, 0, 0> axisStream;

static unsigned long filter[tbFilterN*tbFilterSize*tbFilterSize*(tbKernelN/weightsPerStream+1)];
static unsigned long inputMap[tbInputMapSize*tbInputMapSize*(tbKernelN/actsPerStream+1)];
unsigned long outputMap[(tbFilterN/weightsPerStream+1)*tbOutputMapSize*tbOutputMapSize];

void conv(hls::stream<axisStream> &strm_in,
		hls::stream<axisStream> &strm_out,
		int filterN,
		int kernelN,
		int kernelSize,
		int mapSizeX,
		int mapSizeY,
		bool relu);


void print_mat(int *x, int rows, int cols)
{
	int i;
	for (i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			printf("%5d ", x[i*cols+j]);
		}
		printf("\n");
	}
	printf("\n");
}

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

void print_outputMat(unsigned long *x, int rows, int cols, int channels){
	int i;
	int addrSuplement =0;
	int shifting= itersPerStream;
	int addr=0;
	//printf("starting shifting is %d\n",shifting);
	for(int c=channels; c>0 ;c--){ //(tbKernelN/itersPerStream+1)  ((channels-1)/itersPerStream+1)*itersPerStream;c>((~(channels-1)&(itersPerStream-1)))
		shifting--;
		addr=0;
		if(shifting <0) shifting=itersPerStream-1;
		for (i=0; i<rows; i++) {
			for (int j=0; j<cols; j++) {

				//printf("shifitng is %d   addr suplement is %d",shifting  , addrSuplement);


				//printf("%d ",addr+addrSuplement);//(i*rows*((channels/itersPerStream+1)/itersPerStream)+j*((channels/itersPerStream+1)/itersPerStream))+addrSuplement
				printf("%lu  ",(x[addr+addrSuplement]>>(shifting)*AWidth)&(((long)1<<AWidth)-1));//(x[(i*rows+j)+c/actsPerStream]>>(AWidth*(c&(actsPerStream-1))))&((1<<AWidth)-1)  &(((long)1<<AWidth)-1)
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
		for(int k=0; k<((tbKernelN-1)/itersPerStream+1);k++){
			unsigned long value= 0;
			for(int y=0;y<itersPerStream;y++){
				if(k*itersPerStream+y<tbKernelN){
					value= (value<<AWidth)+(tbInputMapSize*tbInputMapSize)*(k*itersPerStream+y)+i;
				}else{
					value= (value<<AWidth);
				}
			}
			inputMap[i*((tbKernelN-1)/itersPerStream+1)+k] = value;
		}

	}
	for (int f=0; f<(tbFilterN); f++) {
		for (int i=0; i<(tbFilterSize*tbFilterSize); i++) {
			for(int k=0; k<((tbKernelN-1)/itersPerStream+1);k++){
				unsigned long value= 0;
				for(int y=0;y<itersPerStream;y++){
					if(k*itersPerStream+y<tbKernelN){
						value= (value<<WWidth)+(f*tbFilterSize*tbFilterSize*tbKernelN)+(tbFilterSize*tbFilterSize)*(k*itersPerStream+y)+i;
						//printf("%d\n",(f*tbFilterSize*tbFilterSize*tbKernelN)+(tbFilterSize*tbFilterSize)*(k*itersPerStream+y)+i);
					}else{
						value= (value<<WWidth);
					}
				}
				filter[(f*tbFilterSize*tbFilterSize*(tbKernelN/itersPerStream+1))+i*(tbKernelN/itersPerStream+1)+k] = value;
				//printf("\t%d  %lu\n",(f*tbFilterSize*tbFilterSize*(tbKernelN/itersPerStream+1))+i*(tbKernelN/itersPerStream+1)+k,value);
			}

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
	print_outputMat(inputMap, tbInputMapSize, tbInputMapSize,tbKernelN);
	printf("Filter 0\n");
	print_outputMat(filter, tbFilterSize, tbFilterSize,tbKernelN);
	printf("Filter 1\n");
	print_outputMat(filter+2*3*3, tbFilterSize, tbFilterSize,tbKernelN);

	tmp.data=(ap_int<64>)0;
	str_in.write(tmp);
	printf("Sent Bias\n");

	for (int i=0; i<(tbFilterN*tbFilterSize*tbFilterSize*((tbKernelN-1)/itersPerStream+1)); i++) {
		tmp.data=(ap_int<64>)filter[i];
		str_in.write(tmp);
		//printf("%d %lu\n",i,filter[i]);
	}

	printf("Sent whole Filter\n");



	for (int y =0 ;y<tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1); y++) {
		tmp.data=(ap_int<64>)inputMap[y];
		if(y == tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1)-1) tmp.last=1;
		else tmp.last=0;
		str_in.write(tmp);
		//printf("%lu\n",inputMap[y]);
	}

	printf("Sent whole Input Map\n");

	conv(str_in, str_out,tbFilterN,tbKernelN,tbFilterSize,tbInputMapSize,tbInputMapSize,0);

	printf("getting %d values\n",tbOutputMapSize*tbOutputMapSize*((tbFilterN-1)/itersPerStream+1));
	for (int i=0; i<tbOutputMapSize*tbOutputMapSize*((tbFilterN-1)/itersPerStream+1); i++) {
		tmpa = str_out.read();
		outputMap[i] = ((unsigned long)tmpa.data);
	}

	printf("Output is: \n");
	print_outputMat(outputMap, tbOutputMapSize, tbOutputMapSize,tbFilterN);

	return 0;
}
