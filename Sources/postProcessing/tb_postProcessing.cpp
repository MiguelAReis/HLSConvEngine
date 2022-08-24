#include <stdio.h>
#include <stdlib.h>
#include <ap_int.h>
#include <cmath>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "postProcessingParameters.h"

#define tbKernelN 1
#define tbInputMapSize 1




typedef ap_int<WWidth> filterDataType;
typedef ap_int<AWidth> actDataType;
typedef ap_axis<64, 0, 0, 0> axisStream;

static  long inputMap0[tbInputMapSize*tbInputMapSize*(tbKernelN/itersPerStream+1)]={9223372036854775807};
static  long inputMap1[tbInputMapSize*tbInputMapSize*(tbKernelN/itersPerStream+1)];
long outputMap[19*19*(tbKernelN/itersPerStream+1)];

void postProcessing(hls::stream<axisStream> &strm_in0,
		hls::stream<axisStream> &strm_in1,
		hls::stream<axisStream> &strm_out,
		int ctrl, //0: addMap 1: Map0Signed? 2: Map1Signed? 3-4: addMapShift 5:interpolate 6-15:size0 16-25:size1
		int totalValues,
		int valuesZWise);




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
		for(int k=0; k<((tbKernelN-1)/itersPerStream+1);k++){
			unsigned long value= 0;
			for(int y=0;y<itersPerStream;y++){
				//printf("k*itersPerStream+y %d\n",k*itersPerStream+y);
				if(k*itersPerStream+y<tbKernelN){
					value= (value<<AWidth)+(((tbInputMapSize*tbInputMapSize)*(k*itersPerStream+y)+i)&0xF);
					//printf("put %d into %lu\n",(tbInputMapSize*tbInputMapSize)*(k*itersPerStream+y)+i,value);
				}else{
					value= (value<<AWidth);
					//printf("shifted %d into %lu",AWidth,value);
				}
			} //arranjar shifts quando width n e potencia 2
			inputMap0[i*((tbKernelN-1)/itersPerStream+1)+k] = value<<ARemainder;
			inputMap1[i*((tbKernelN-1)/itersPerStream+1)+k] = value<<ARemainder;
			//printf("%lu \n",inputMap[i*((tbKernelN-1)/itersPerStream+1)+k]);
		}

	}




}

int main()
{

	hls::stream<axisStream> str_in0("inputStream"); //("sinp");
	hls::stream<axisStream> str_in1("inputStream"); //("sinp");
	hls::stream<axisStream> str_out("outputStream"); //("sout");
	axisStream tmp0,tmp1, tmpa;

	printf("Beginning testbench\n");

	//initVectors();
	printf("Input Map0\n");
	print_Map(inputMap0, tbInputMapSize, tbInputMapSize,tbKernelN,0);
	//printf("Input Map1\n");
	//print_Map(inputMap1, tbInputMapSize, tbInputMapSize,tbKernelN,0);



	for (int y =0 ,j=0;y<tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1); y++) {
		tmp0.data=(ap_int<64>)inputMap0[y];
		tmp1.data=(ap_int<64>)inputMap1[y];
		//if(y == tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1)-1) tmp.last=1;
		tmp0.last=(j==1);
		j++;
		if(j==2) j=0;
		//str_in1.write(tmp1);
		str_in0.write(tmp0);

		printf("sent %lu\n",inputMap0[y]);
		//if(tmp1.last) printf("sent last\n");
	}

	printf("Sent whole Input Maps N = %d  \n",tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1));
	int ctrl=327784;
	printf("ctrl is %d\n",ctrl);
	postProcessing(str_in0,str_in1,str_out,ctrl,tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1),((tbKernelN-1)/itersPerStream+1));

	printf("Receiving out Map N = %d  \n",9*9*((tbKernelN-1)/itersPerStream+1),((tbKernelN-1)/itersPerStream+1));

	/*for (int i=0; i<5*5*((tbKernelN-1)/itersPerStream+1); i++) {
		tmpa = str_out.read();
		outputMap[i] = ((unsigned long)tmpa.data);
		printf("%lu\n",outputMap[i]);
		if(tmpa.last && i<(5*5*((tbKernelN-1)/itersPerStream+1))-1){
			printf("Received LAST\n");
			postProcessing(str_in0,str_in1,str_out,ctrl,tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1),((tbKernelN-1)/itersPerStream+1));

		}
	}*/

	for (int i=0; i<5*5*((tbKernelN-1)/itersPerStream+1); i++) {
		tmpa = str_out.read();
		outputMap[i] = ((unsigned long)tmpa.data);
		printf("%lu \n",outputMap[i]);
		if(tmpa.last)printf("LAST \n",outputMap[i]);
	}

	printf("Output is: \n");
	print_Map(outputMap, 5, 5,tbKernelN,0);

	return 0;
}
