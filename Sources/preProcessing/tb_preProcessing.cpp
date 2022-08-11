#include <stdio.h>
#include <stdlib.h>
#include <ap_int.h>
#include <cmath>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "preProcessingParameters.h"



#define tbKernelN 3
#define tbInputMapSize 7




typedef ap_int<WWidth> filterDataType;
typedef ap_int<AWidth> actDataType;
typedef ap_axis<64, 0, 0, 0> axisStream;

static  long inputMap[tbInputMapSize*tbInputMapSize*(tbKernelN/itersPerStream+1)] = {-6917430071594582016, 2333753012373159936, 8254337254923698176, -1383319768160796672, -6302021318900449280, 5577578396126281728, 7459962094088093696, 6245865961535045632, 7364820253425008640, 7704701287803125760, -3898647230256513024, 3891670828978274304, 7393653846351806464, 828974592738459648, -5513140417668841472, 6005983310679900160, 941726211632005120, 3064960130681405440, 605379007625560064, 2992457234434228224, 2241888815872475136, 8146613702703972352, 8515008372125204480, -3199145629246816256, 6392601285819891712};
long outputMap[tbInputMapSize*tbInputMapSize*(tbKernelN/itersPerStream+1)];

void preProcessing(hls::stream<axisStream> &strm_in,
		hls::stream<axisStream> &strm_out,
		int ctrl,//1:AvgPool 2
		int mapSize,
		int zWiseValues);


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


void initVectors(){


	for (int i=0; i<(tbInputMapSize*tbInputMapSize); i++) {
		for(int k=0; k<((tbKernelN-1)/itersPerStream+1);k++){
			unsigned long value= 0;
			for(int y=0;y<itersPerStream;y++){
				//printf("k*itersPerStream+y %d\n",k*itersPerStream+y);
				if(k*itersPerStream+y<tbKernelN){
					value= (value<<AWidth)+(tbInputMapSize*tbInputMapSize)*(k*itersPerStream+y)+i;
					//printf("put %d into %lu\n",(tbInputMapSize*tbInputMapSize)*(k*itersPerStream+y)+i,value);
				}else{
					value= (value<<AWidth);
					//printf("shifted %d into %lu",AWidth,value);
				}
			} //arranjar shifts quando width n e potencia 2
			inputMap[i*((tbKernelN-1)/itersPerStream+1)+k] = value<<ARemainder;
			//printf("%lu \n",inputMap[i*((tbKernelN-1)/itersPerStream+1)+k]);
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
	print_Map(inputMap, tbInputMapSize, tbInputMapSize,tbKernelN,0);


	for (int y =0 ;y<tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1); y++) {
		tmp.data=(ap_int<64>)inputMap[y];
		if(y == tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1)-1) tmp.last=1;
		else tmp.last=0;
		str_in.write(tmp);
		//printf("%lu\n",inputMap[y]);
	}

	printf("Sent whole Input Map N = %d  \n",tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1));



	preProcessing(str_in, str_out,1,tbInputMapSize*tbInputMapSize,((tbKernelN-1)/itersPerStream+1));

	printf("Receiving out Map N = %d  \n",tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1));

	for (int i=0; i<1*1*((tbKernelN-1)/itersPerStream+1); i++) {
		tmpa = str_out.read();
		outputMap[i] = ((unsigned long)tmpa.data);
		//printf("%lu\n",outputMap[i]);
		//if(tmpa.last) printf("Received LAST\n");
	}

	printf("Output is: \n");
	print_Map(outputMap, 1, 1,tbKernelN,0);

	return 0;
}
