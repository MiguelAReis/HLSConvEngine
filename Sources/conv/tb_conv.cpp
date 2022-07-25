#include <stdio.h>
#include <stdlib.h>
#include <ap_int.h>
#include <cmath>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "convParameters.h"

#define tbFilterN 3
#define tbKernelN 3
#define tbFilterSize 3
#define tbInputMapSize 5
#define tbStride 2
#define tbPadding 2
#define tbOutputMapSize ((tbInputMapSize - tbFilterSize+ 2*tbPadding)/tbStride + 1)



typedef ap_int<WWidth> filterDataType;
typedef ap_int<AWidth> actDataType;
typedef ap_axis<64, 0, 0, 0> axisStream;

static  long filter[tbFilterN*tbFilterSize*tbFilterSize*(tbKernelN/itersPerStream+1)] = {1305675555542138880, -76357784014159872, 2044706798593638400, 4755801206503243776, -7017155776233865216, 5248406605002702848, 4917035790623571968, 5183362795638358016, -5392423936543686656, 2448170391382786048, -1082095363592028160, -5386323846032785408, 1732816031723814912, 344831035726364672, 6315116502387261440, -1578279672439898112, 3187672225410973696, 2231010247827259392, -3949982328645746688, 5747901543361806336, 4375052424432320512, 4025636425218129920, -3299907073839464448, 2172769116903964672, 7006088092188672000, -3186893771178508288, -3325892931650322432} ;
static  long inputMap[tbInputMapSize*tbInputMapSize*(tbKernelN/itersPerStream+1)] = {-6917430071594582016, 2333753012373159936, 8254337254923698176, -1383319768160796672, -6302021318900449280, 5577578396126281728, 7459962094088093696, 6245865961535045632, 7364820253425008640, 7704701287803125760, -3898647230256513024, 3891670828978274304, 7393653846351806464, 828974592738459648, -5513140417668841472, 6005983310679900160, 941726211632005120, 3064960130681405440, 605379007625560064, 2992457234434228224, 2241888815872475136, 8146613702703972352, 8515008372125204480, -3199145629246816256, 6392601285819891712};
long outputMap[((2*tbFilterN)/itersPerStream+1)*tbOutputMapSize*tbOutputMapSize];

void conv(hls::stream<axisStream> &strm_in,
		hls::stream<axisStream> &strm_out,
		int filterN,
		int kernelN,
		int kernelSize,
		int mapSizeX,
		int mapSizeY,
		int stride,
		int padding,
		int scale,
		int relu);


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
	for (int f=0; f<(tbFilterN); f++) {
		for (int i=0; i<(tbFilterSize*tbFilterSize); i++) {
			for(int k=0; k<((tbKernelN-1)/itersPerStream+1);k++){
				unsigned long value= 0;
				for(int y=0;y<itersPerStream;y++){
					//printf("k*itersPerStream+y %d\n",k*itersPerStream+y);
					if(k*itersPerStream+y<tbKernelN){
						value= (value<<WWidth)+(f*tbFilterSize*tbFilterSize*tbKernelN)+(tbFilterSize*tbFilterSize)*(k*itersPerStream+y)+i;
						//printf("put %d into %lu\n",(f*tbFilterSize*tbFilterSize*tbKernelN)+(tbFilterSize*tbFilterSize)*(k*itersPerStream+y)+i,value);
					}else{
						value= (value<<WWidth);
						//printf("shifted %d into %lu",WWidth,value);
					}
				}
				filter[(f*tbFilterSize*tbFilterSize*((tbKernelN-1)/itersPerStream+1))+i*((tbKernelN-1)/itersPerStream+1)+k] = value<<WRemainder;
				//printf("\t%d  %lu\n",(f*tbFilterSize*tbFilterSize*((tbKernelN-1)/itersPerStream+1))+i*((tbKernelN-1)/itersPerStream+1)+k,filter[(f*tbFilterSize*tbFilterSize*((tbKernelN-1)/itersPerStream+1))+i*((tbKernelN-1)/itersPerStream+1)+k]);
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

	//initVectors();
	printf("Input Map\n");
	print_Map(inputMap, tbInputMapSize, tbInputMapSize,tbKernelN,0);
	printf("Filter 0\n");
	print_Filter(filter, tbFilterSize, tbFilterSize,tbKernelN);
	printf("Filter 1\n");
	int sizeOfFilter= tbFilterSize*tbFilterSize*((tbKernelN-1)/itersPerStream+1);
	print_Filter(filter+sizeOfFilter, tbFilterSize, tbFilterSize,tbKernelN);
	printf("Filter 3\n");
	print_Filter(filter+sizeOfFilter*2, tbFilterSize, tbFilterSize,tbKernelN);
	tmp.data=(ap_int<64>)127;
	str_in.write(tmp);
	tmp.data=(ap_int<64>)127;
	str_in.write(tmp);
	tmp.data=(ap_int<64>)-128;
	str_in.write(tmp);
	printf("Sent Bias N = %d \n",3);

	for (int i=0; i<(tbFilterN*tbFilterSize*tbFilterSize*((tbKernelN-1)/itersPerStream+1)); i++) {
		tmp.data=(ap_int<64>)filter[i];
		str_in.write(tmp);
		//printf("%d %lu\n",i,filter[i]);
	}

	printf("Sent whole Filter N = %d \n",(tbFilterN*tbFilterSize*tbFilterSize*((tbKernelN-1)/itersPerStream+1)));



	for (int y =0 ;y<tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1); y++) {
		tmp.data=(ap_int<64>)inputMap[y];
		//if(y == tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1)-1) tmp.last=1;
		//else tmp.last=0;
		str_in.write(tmp);
		//printf("%lu\n",inputMap[y]);
	}

	printf("Sent whole Input Map N = %d  \n",tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1));

	conv(str_in, str_out,tbFilterN,tbKernelN,tbFilterSize,tbInputMapSize,tbInputMapSize,tbStride,tbPadding,9,0);

	printf("Receiving out Map N = %d  \n",tbOutputMapSize*tbOutputMapSize*((tbFilterN-1)/itersPerStream+1));

	for (int i=0; i<tbOutputMapSize*tbOutputMapSize*((tbFilterN-1)/itersPerStream+1); i++) {
		tmpa = str_out.read();
		outputMap[i] = ((unsigned long)tmpa.data);
		//printf("%lu\n",outputMap[i]);
		if(tmpa.last) printf("Received LAST\n");
	}

	printf("Output is: \n");
	print_Map(outputMap, tbOutputMapSize, tbOutputMapSize,tbFilterN,1);

	return 0;
}
