#include <stdio.h>
#include <stdlib.h>
#include <ap_int.h>
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
typedef ap_axis<32, 0, 0, 0> axisStream;

static filterDataType filter[tbFilterN*tbKernelN*tbFilterSize*tbFilterSize];
static actDataType inputMap[tbKernelN*tbInputMapSize*tbInputMapSize];
int outputMap[tbFilterN*tbOutputMapSize*tbOutputMapSize];

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

void print_outputMat(int *x, int rows, int cols, int channels){
	int i;
	for(int c=0;c<channels;c++){
		for (i=0; i<rows; i++) {
			for (int j=0; j<cols; j++) {
				printf("%d ", (int)x[channels*(i*rows+j)+c]);
			}
			printf("\n");
		}
		printf("\n\n\n");
	}
}

void initVectors(){


	for (int i=0; i<(tbInputMapSize*tbInputMapSize); i++) {
		for(int y=0;y<tbKernelN;y++){
			inputMap[tbKernelN*i+y] = (tbInputMapSize*tbInputMapSize)*y+i;
			//printf("%d - %d\n",3*i+y, (tbInputMapSize*tbInputMapSize)*y+i);
			//inputMap[i] = i&0x01;
		}
	}
	/*
	for (int i=0; i<(tbFilterN*tbKernelN*tbFilterSize*tbFilterSize); i++) {
		filter[i] = i;
	}*/

	for (int i=0; i<(tbFilterN); i++) {
		for(int y=0;y<tbFilterSize*tbFilterSize;y++){
			for(int j=0;j<tbKernelN;j++){
				filter[i*tbFilterSize*tbFilterSize*tbKernelN+y*tbKernelN+j] = (tbFilterSize*tbFilterSize*tbKernelN)*i+y+j*tbFilterSize*tbFilterSize;
				//printf("%d -> %d\n",i*tbFilterSize*tbFilterSize*tbKernelN+y*tbKernelN+j,(int)filter[i*tbFilterSize*tbFilterSize*tbKernelN+y*tbKernelN+j]);
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

	tmp.data=(ap_int<32>)0;
	str_in.write(tmp);
	printf("Sent Bias\n");

	for (int i=0; i<(tbFilterN*tbKernelN*tbFilterSize*tbFilterSize); i++) {
		tmp.data=(ap_int<32>)filter[i];
		str_in.write(tmp);
		//printf("%d\n",i);
	}

	printf("Sent whole Filter\n");



	for (int y =0 ;y<tbKernelN*tbInputMapSize*tbInputMapSize; y++) {
		tmp.data=(ap_int<32>)inputMap[y];
		if(y == tbFilterN*tbInputMapSize*tbInputMapSize-1) tmp.last=1;
		else tmp.last=0;
		str_in.write(tmp);
		//printf("%d\n",y);
	}

	printf("Sent whole Input Map\n");

	conv(str_in, str_out,tbFilterN,tbKernelN,tbFilterSize,tbInputMapSize,tbInputMapSize,0);


	for (int i=0; i<tbOutputMapSize*tbOutputMapSize*tbFilterN; i++) {
		tmpa = str_out.read();
		outputMap[i] = ((int)tmpa.data);
	}

	printf("Output is: \n");
	print_outputMat(outputMap, tbOutputMapSize, tbOutputMapSize,tbFilterN);

	return 0;
}
