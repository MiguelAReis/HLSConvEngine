#include <stdio.h>
#include <stdlib.h>
#include <ap_int.h>
#include <cmath>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "convParameters.h"

#define tbFilterN 32
#define tbKernelN 1
#define tbFilterSize 3
#define tbInputMapSize 5
#define tbStride 1
#define tbPadding 0
#define tbOutputMapSize ((tbInputMapSize - tbFilterSize+ 2*tbPadding)/tbStride + 1)
#define tbScale 0
#define tbBiasScale 0
#define tbRelu 1
#define tbIsMapSigned 0
#define tbTlast 1


typedef ap_int<WWidth> filterDataType;
typedef ap_int<AWidth> actDataType;
typedef ap_axis<64, 0, 0, 0> axisStream;

static  long filter[tbFilterN*tbFilterSize*tbFilterSize*(tbKernelN/itersPerStream+1)];/*={6669831048135704576,
		-5359283556570890240,
		8944148859957805056,
		-2490490593935884288,
		2814749767106560000,
		-810647932926689280,
		-1603281467343896576,
		9146810843189477376,
		-3873095679538626560,
		-7642608567647731712,
		-7358881791123390464,
		2296835809958952960,
		-6381600671983992832,
		-7363385390750760960,
		-6210463886143913984,
		144115188075855872,
		1504202275541745664,
		6935543426150563840,
		4805340802404319232,
		4998995586381250560,
		1134907106097364992,
		7475975381435023360,
		-4278419646001971200,
		-6390607871238733824,
		8601875288277647360,
		8583860889768165376,
		1832965048339791872,
		8174033323677450240,
		-7525514977336098816,
		-7899313746407849984,
		-4652218415073722368,
		-7561543774355062784};*/
static  long inputMap[tbInputMapSize*tbInputMapSize*(tbKernelN/itersPerStream+1)];/*={2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		-153122387330596864,
		1071856711314178048,
		3301138526862573568,
		3301138526862573568,
		2152720621883097088,
		4382002437431492608,
		3305642126489944064,
		2152720621883097088,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		1139410705724735488,
		3368692521273131008,
		2206763817411543040,
		4589168020290535424,
		4377498837804122112,
		2287828610704211968,
		999799117276250112,
		2224778215921025024,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		-1242993497154256896,
		2274317811822100480,
		1193453901253181440,
		977281119139397632,
		-1031324314667843584,
		1130403506469994496,
		2359886204742139904,
		2373397003624251392,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		-90071992547409920,
		2283325011076841472,
		1125899906842624000,
		2422936599525326848,
		2278821411449470976,
		40532396646334464,
		2440950998034808832,
		1143914305352105984,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		-1098878309078401024,
		2355382605114769408,
		193654783976931328,
		-1112389107960512512,
		45035996273704960,
		45035996273704960,
		4589168020290535424,
		2161727821137838080,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		-13510798882111488,
		54043195528445952,
		193654783976931328,
		184647584722190336,
		4647714815446351872,
		2350879005487398912,
		2296835809958952960,
		2224778215921025024,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2292332210331582464,
		1211468299762663424,
		54043195528445952,
		1342072688956407808,
		-882705526964617216,
		-954763121002545152,
		1292533093055332352,
		0,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		1071856711314178048,
		2296835809958952960,
		-1089871109823660032,
		58546795155816448,
		1143914305352105984,
		0,
		1288029493427961856,
		1220475499017404416,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520,
		2229281815548395520};*/

long outputMap[((2*tbFilterN)/itersPerStream+1)*tbOutputMapSize*tbOutputMapSize];

void conv(hls::stream<axisStream> &strm_in,
		hls::stream<axisStream> &strm_out,
		int filterN,
		int kernelN,
		int mapSizeX,
		int mapSizeY,
		int ctrl);


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
					value= (value<<AWidth)+(((tbInputMapSize*tbInputMapSize)*(k*itersPerStream+y)+i)&0xF);
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
						value= (value<<WWidth)+(((f*tbFilterSize*tbFilterSize*tbKernelN)+(tbFilterSize*tbFilterSize)*(k*itersPerStream+y)+i)&0xF);
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

	initVectors();
	printf("Input Map\n");
	print_Map(inputMap, tbInputMapSize, tbInputMapSize,tbKernelN,tbIsMapSigned);
	printf("Filter 0\n");
	print_Filter(filter, tbFilterSize, tbFilterSize,tbKernelN);
	printf("Filter 1\n");
	int sizeOfFilter= tbFilterSize*tbFilterSize*((tbKernelN-1)/itersPerStream+1);
	//print_Filter(filter+sizeOfFilter, tbFilterSize, tbFilterSize,tbKernelN);
	printf("Filter 3\n");
	//print_Filter(filter+sizeOfFilter*2, tbFilterSize, tbFilterSize,tbKernelN);
	long bias =0;//((long)127<<(64-8))+((long)127<<(64-16))+((long)-128<<(64-24));
	tmp.data=bias;
	str_in.write(tmp);
	str_in.write(tmp);
	str_in.write(tmp);
	str_in.write(tmp);
	//str_in.write(tmp);
	/*tmp.data=-1432125677477567730;
	str_in.write(tmp);
	tmp.data=-4052052124341379355;
	str_in.write(tmp);
	tmp.data=-4917047876375809747;
	str_in.write(tmp);*/

	printf("Sent Bias N = %d \n",((tbFilterN-1)/biasPerStream+1));

	for (int i=0; i<(tbFilterN*tbFilterSize*tbFilterSize*((tbKernelN-1)/itersPerStream+1)); i++) {
		tmp.data=(ap_int<64>)filter[i];
		str_in.write(tmp);
		printf("%d %lu\n",i,filter[i]);
	}

	printf("Sent whole Filter N = %d \n",(tbFilterN*tbFilterSize*tbFilterSize*((tbKernelN-1)/itersPerStream+1)));



	for (int y =0 ;y<tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1); y++) {
		tmp.data=(ap_int<64>)inputMap[y];
		//if(y == tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1)-1) tmp.last=1;
		//else tmp.last=0;
		str_in.write(tmp);
		printf("%lu\n",inputMap[y]);
	}

	printf("Sent whole Input Map N = %d  \n",tbInputMapSize*tbInputMapSize*((tbKernelN-1)/itersPerStream+1));

	// 0-1: kernelSize //2-3: stride  4-6: padding 7:isMapSigned 8-11: biasScale 12-14: scale 15: relu 16:tlast
	int ctrl=(tbFilterSize)+(tbStride<<2)+(tbPadding<<4)+(tbIsMapSigned<<7)+(tbBiasScale<<8)+(tbScale<<12)+(tbRelu<<15)+(tbTlast<<16);
	conv(str_in, str_out,tbFilterN,tbKernelN,tbInputMapSize,tbInputMapSize,ctrl);

	printf("Receiving out Map N = %d  \n",tbOutputMapSize*tbOutputMapSize*((tbFilterN-1)/itersPerStream+1));

	for (int i=0; i<tbOutputMapSize*tbOutputMapSize*((tbFilterN-1)/itersPerStream+1); i++) {
		tmpa = str_out.read();
		outputMap[i] = ((unsigned long)tmpa.data);
		printf("%lu\n",outputMap[i]);
		if(tmpa.last) printf("Received LAST\n");
	}

	printf("Output is: \n");
	print_Map(outputMap, tbOutputMapSize, tbOutputMapSize,tbFilterN,0);

	return 0;
}
