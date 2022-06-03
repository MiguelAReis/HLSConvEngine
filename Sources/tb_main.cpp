#include <stdio.h>
#include <stdlib.h>
#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "engineParameters.h"

#define tbFilterN 3
#define tbFilterSize 3
#define tbInputMapSize 5
#define tbOutputMapSize (tbInputMapSize - tbFilterSize + 1)

typedef ap_int<WWidth> filterDataType;
typedef ap_int<AWidth> actDataType;
typedef ap_axis<32, 0, 0, 0> axisStream;

static filterDataType filter[tbFilterN*tbFilterSize*tbFilterSize];
static actDataType inputMap[tbFilterN*tbInputMapSize*tbInputMapSize];
int outputMap[tbOutputMapSize*tbOutputMapSize];

void conv(hls::stream<axisStream> &strm_in,
		hls::stream<axisStream> &strm_out,
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

void initVectors(){

	for (int i=0; i<(tbFilterN*tbInputMapSize*tbInputMapSize); i++) {
		inputMap[i] = i;
		//inputMap[i] = i&0x01;
	}
	for (int i=0; i<(tbFilterN*tbFilterSize*tbFilterSize); i++) {
		filter[i] = i;

	}
}

int main()
{

	hls::stream<axisStream> str_in; //("sinp");
	hls::stream<axisStream> str_out; //("sout");
	axisStream tmp, tmpa;

	printf("Beginning testbench\n");

	initVectors();
	tmp.data=(ap_int<32>)32;
	str_in.write(tmp);
	printf("Sent Bias\n");

	for (int i=0; i<(tbFilterN*tbFilterSize*tbFilterSize); i++) {
		tmp.data=(ap_int<32>)filter[i];
		str_in.write(tmp);
	}
	printf("Sent whole Filter\n");



	for (int i=0; i<(tbFilterN*tbInputMapSize*tbInputMapSize); i++) {
		tmp.data=(ap_int<32>)inputMap[i];
		str_in.write(tmp);
	}
	printf("Sent whole Input Map\n");

	conv(str_in, str_out,tbFilterN,tbFilterSize,tbInputMapSize,tbInputMapSize,0);


	for (int i=0; i<tbOutputMapSize*tbOutputMapSize; i++) {
		tmpa = str_out.read();
		outputMap[i] = ((int)tmpa.data);
	}

	printf("Output is: \n");
	print_mat(outputMap, tbOutputMapSize, tbOutputMapSize);

	return 0;
}
