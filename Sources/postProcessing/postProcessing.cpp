#include <ap_int.h>
#include <stdint.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "postProcessingParameters.h"



typedef ap_int<itersPerStream*WWidth> filterDataType;
typedef ap_int<itersPerStream*AWidth> actDataType;
typedef ap_int<biasPerStream*BWidth> biasStreamDataType;
typedef ap_int<DMAWidth> accDataType;
typedef ap_int<BWidth> biasDataType;
typedef ap_uint<AWidth> PEAct;
typedef ap_int<AWidth+1> PESignedAct;
typedef ap_int<WWidth> PEWeight;
typedef ap_int<WWidth+AWidth> PEWAccum;
typedef ap_uint<AWidth+1> addAct;
typedef ap_axis<64, 0, 0, 0> axisStream;

actDataType BilinearInterpolation(actDataType q11, actDataType q12, actDataType q21, actDataType q22, ap_int<20> x2x1y2y1,  ap_int<20> first, ap_int<10> second, ap_int<20> third, ap_int<20> fourth,ap_int<20> factor,int scale,bool map0Signed)
{
//#pragma HLS INLINE
    actDataType outValues=0,result=0;



    for(unsigned short w=0;w<itersPerStream;w++){
		#pragma HLS UNROLL
    	PESignedAct q11Value, q21Value, q12Value, q22Value;
    	if(map0Signed){
    		q11Value=q11.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth));
    		q12Value=q12.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth));
    		q21Value=q21.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth));
    		q22Value=q22.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth));
    	}
    	else{
    		q11Value=q11.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth))&((1<<AWidth)-1);
    		q12Value=q12.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth))&((1<<AWidth)-1);
    		q21Value=q21.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth))&((1<<AWidth)-1);
    		q22Value=q22.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth))&((1<<AWidth)-1);
    	}
    	//printf("q11 %d q12 %d q21 %d q22 %d\n",q11Value.to_int(),q12Value.to_int(),q21Value.to_int(),q22Value.to_int());

		result=  (((x2x1y2y1+(first*q11Value+second*q21Value+third*q12Value+fourth*q22Value))*factor)>>factorBits)-1;
		//printf("result is %d\n",result.to_int());
		outValues=(outValues<<AWidth)+result.range(AWidth-1+scale,scale);
    }

    return outValues;

}


void postProcessing(hls::stream<axisStream> &strm_in0,
		hls::stream<axisStream> &strm_in1,
		hls::stream<axisStream> &strm_out,
		int ctrl, //0: addMap 1: Map0Signed? 2: Map1Signed? 3-4: Scale 5:interpolate 6-15:size0 16-25:size1
		int totalValues,
		int valuesZWise){

	//#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS INTERFACE axis port=strm_in0
	#pragma HLS INTERFACE axis port=strm_in1
	#pragma HLS INTERFACE axis port=strm_out
	#pragma HLS INTERFACE s_axilite port=return bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=ctrl bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=totalValues bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=valuesZWise bundle=BUS1


	actDataType input[3][MapMaxXSize*((FilterMaxN-1)/itersPerStream+1)];
	//PRAGMA_HLS(HLS array_partition variable=input cyclic factor=2);


	axisStream tmp0,tmp1,tmpo;

	bool addMap = ctrl&0x01;
	bool map0Signed = (ctrl&0x02)>>1;
	bool map1Signed = (ctrl&0x04)>>2;
	unsigned short scale=(ctrl&0x18)>>3;
	bool interpolate=(ctrl&0x20)>>5;
	unsigned short size0=(ctrl&0xFFC0)>>6;//initial size
	unsigned short size1=(ctrl&0x3FF0000)>>16;//final size
	addAct map0Value=0;
	addAct map1Value=0;
	accDataType outValues=0;
	actDataType map0Stream=0;
	actDataType map1Stream=0;

	ap_uint<10> factor0 = size0-1;
	ap_uint<10> factor1 = size1-1;
	//printf("factor0 is %d\n",factor0);
	//printf("factor1 is %d\n",factor1);
	unsigned int factor=0;
	for(int i=0;i<factor1;i++) factor+=factor1;
	//printf("factor is %d\n",factor);
	factor=(1<<factorBits)/factor;
	//printf("factor is %d\n",factor);
	int x2x1=0, y2y1=0, x2x=0, y2y=0, yy1=0, xx1=0, x2x1y2y1=0;
	int x1=0,x1_=0,x2=factor1,x2_=1,y1=0 ,y1_=0,y2=factor1 ,y2_=1;
	unsigned short bramIndex0=0,bramIndex1=1,bramIndex2=2;
	int xLimit=0,yLimit=0;
	actDataType q11=0,q12=0,q21=0,q22=0;
	bool readValues=1;
	int linesRead=3;
	ap_int<20> first;
	ap_int<20> second;
	ap_int<20> third;
	ap_int<20> fourth;
	unsigned int xx=0,xz=0;





	if(addMap){
		ADDLOOP:for(unsigned int i=0,j=0;i<totalValues;i++){
			#pragma HLS PIPELINE II=1
			tmp0 = strm_in0.read();
			tmp1 = strm_in1.read();
			map0Stream=tmp0.data.range(DMAWidth-1,ARemainder);
			map1Stream=tmp1.data.range(DMAWidth-1,ARemainder);
			for(unsigned short w=0;w<itersPerStream;w++){
				#pragma HLS UNROLL
				if(map0Signed) map0Value=map0Stream.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth));
				else map0Value=map0Stream.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth))&((1<<AWidth)-1);
				if(map1Signed) map1Value=map1Stream.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth));
				else map1Value=map1Stream.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth))&((1<<AWidth)-1);
				//printf("map0Value is %d map1Value is%d\n",map0Value.to_int(),map1Value.to_int());
				map0Value+=map1Value;
				outValues=(outValues<<AWidth)+map0Value.range(AWidth-1+scale,scale);
			}

			outValues=outValues<<ARemainder;
			//printf("\toutValue is %ld\n",outValues.to_long());
			tmpo.data = outValues;
			tmpo.keep = 0xFF;
			tmpo.strb = 0xFF;
			tmpo.last = (j==valuesZWise-1);
			strm_out.write(tmpo);
			if(j==valuesZWise-1) j=0;
			outValues=0;
		}
	}else if(interpolate){

		//printf("size0 is %d\n",size0);
		//printf("size1 is %d\n",size1);
		INTERPOLATEFIRSTLOOP:for(unsigned short y=0;y<2;y++){
			//printf("changed line\n");
			for(unsigned int x=0,xzz=0;x<size0;x++){
				for(unsigned int z=0;z<(ap_uint<10>)valuesZWise;z++,xz++){
					#pragma HLS PIPELINE II=1
					if(!x && !z)xz=0;

					tmp0 = strm_in0.read();
					input[y][xz]=tmp0.data.range(DMAWidth-1,ARemainder);
					//printf("received %lu\n",input[y][xz].to_long());

				}

			}
		}
		//printf("interpolating\n");
		x2x1 = x2 - x1;
		y2y1 = y2 - y1;
		x2x1y2y1 = (x2x1 * y2y1);
		INTERPOLATEYLOOP:for(ap_uint<10> outY=0,yy=0;outY<size1;outY++,yy+=factor0){
			PRAGMA_HLS(HLS loop_tripcount min=9 max=9);
			//printf("changed line\n");
			INTERPOLATEXLOOP:for(ap_uint<10> outX=0;outX<size1;outX++){
				PRAGMA_HLS(HLS loop_tripcount min=9 max=9);
				INTERPOLATEZLOOP:for(ap_uint<10> outZ=0;outZ<valuesZWise;outZ++,xz++){
					PRAGMA_HLS(HLS loop_tripcount min=(256/itersPerStream) max=(256/itersPerStream));
					#pragma HLS PIPELINE II=1

					if(!outZ){	//when x changes
						if(!outX){//when y changes
							xx=0;
							xz=0;
							x1=0;
							x2=factor1;
							x1_=0;
							x2_=valuesZWise;
							xLimit=0;
							if(yLimit>y2){
								y1=y2;
								//y1_=y2_;
								y2+=factor1;
								//y2_++;
								bramIndex0=bramIndex1;
								bramIndex1=bramIndex2;
								bramIndex2++;
								if(bramIndex2==3)bramIndex2=0;
								if(linesRead<size0){
									readValues=1;
									//printf("changed read values at y=%d\n",outY);
									linesRead++;
								}

							}
							yLimit+=factor0;

						}else xx+=factor0;
						if(xLimit>x2){
							x1=x2;
							x1_=x2_;
							x2+=factor1;
							x2_+=valuesZWise;
						}
						xLimit+=factor0;



					    x2x = x2 - xx;
					    y2y = y2 - yy;
					    yy1 = yy - y1;
					    xx1 = xx - x1;
					    first = (ap_int<10>)x2x * (ap_int<10>)y2y;
					    second = (ap_int<10>)xx1 * (ap_int<10>)y2y;
					    third =  (ap_int<10>)x2x * (ap_int<10>)yy1;
					    fourth = (ap_int<10>)xx1 * (ap_int<10>)yy1;

					   //printf("x2x %d y2y %d yy1 %d xx1 %d\n",x2x,y2y,yy1,xx1);
					   //printf("first %d second %d third %d fourth %d\n",first,second,third,fourth);
					}
					q11=input[bramIndex0][x1_+outZ];
					q12=input[bramIndex1][x1_+outZ];
					q21=input[bramIndex0][x2_+outZ];
					q22=input[bramIndex1][x2_+outZ];
					//printf("\n\nNew Interpolation for pixel y %d x %d\n",outY,outX);
					outValues=BilinearInterpolation(q11,q12,q21,q22,(ap_int<20>)x2x1y2y1,first,second,third,fourth,(ap_int<20>)factor,scale,map0Signed);
					outValues=outValues<<ARemainder;
					//printf("\toutValue is %ld\n",outValues.to_long());
					tmpo.data = outValues;
					tmpo.keep = 0xFF;
					tmpo.strb = 0xFF;
					tmpo.last = (outZ==valuesZWise-1);
					strm_out.write(tmpo);
					if(readValues){
						tmp0 = strm_in0.read();
						input[bramIndex2][xz]=tmp0.data.range(DMAWidth-1,ARemainder);
						//printf("received %lu\n",input[bramIndex2][xz].to_long());
						if(outX==size0-1 && outZ==valuesZWise-1)readValues=0;
					}


				}

			}
		}

	}

}
