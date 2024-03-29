#include <ap_int.h>
#include <stdint.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "preProcessingParameters.h"




typedef ap_int<itersPerStream*(accumBitWidth)> actDataType0;

typedef ap_uint<AWidth> act;
typedef ap_uint<accumBitWidth> act_;

typedef ap_axis<64, 0, 0, 0> axisStream;


void preProcessing(hls::stream<axisStream> &strm_in,
		hls::stream<axisStream> &strm_out,
		int ctrl,//0:AvgPool 1: LoadWeights 2-4: scale
		int mapSize,
		int zWiseValues){

	//#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS INTERFACE axis port=strm_in
	#pragma HLS INTERFACE axis port=strm_out
	#pragma HLS INTERFACE s_axilite port=return bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=ctrl bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=mapSize bundle=BUS1
	#pragma HLS INTERFACE s_axilite port=zWiseValues bundle=BUS1


	//actDataType0 featureMap[((KernelMaxN-1)/itersPerStream+1)];
	actDataType0 featureMap[128];

	axisStream tmp,tmpo;

	bool avg=(ctrl&0x01);
	bool loadWeights= (ctrl&0x02);
	ap_int<3> scale = (ctrl&0x1C)>>2;

	//printf("avg is %d\n",avg);
	//printf("loadWeights is %d\n",loadWeights);
	//printf("scale is %d\n",scale.to_int());
	act actValue=0;
	act_ actValue_=0;
	act_ actValueDiv=0;
	actDataType0 values=0,values_=0,values__=0,values___=0;
	unsigned int j=0,jj=0;
	ap_uint<25> factor=0;
	ap_uint<32> result=0;


	act finalValue=0;
	bool last=0;


	if(avg){
		factor = ((1<<25)/mapSize)+1;
		if(loadWeights){
			for(int i=0;i<2;i++){
				while(!last){ //bias
					tmp = strm_in.read();
					//printf("Received value %lu sending it back\n",tmp.data.to_long());
					tmpo.data=tmp.data;
					strm_out.write(tmpo);
					last=tmp.last;
				}
				last=0;
			}
		}


		OutYLOOP:for(ap_uint<11> xy=0;xy<(ap_uint<11>)mapSize;xy++){
			#pragma HLS loop_tripcount min=5 max=5
			KernelLOOP:for(ap_uint<12> kn=0; kn<(ap_uint<12>)zWiseValues; kn++){
			#pragma HLS PIPELINE  II=1
				tmp = strm_in.read();
				//printf("xy %d kn %d j %d jj %d factor %d zWiseValues %d\n",xy,kn,j,jj,factor,zWiseValues);
				if(!xy){
					values=0;
					for(int w=0;w<itersPerStream;w++){
						actValue=tmp.data.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth));
						values=(values<<(accumBitWidth))+actValue;
						//printf("Got value %d putting it into featureMap Array kn %d\n",actValue.to_int(),kn);
					}
					featureMap[kn]=values;


				}else{
					values_=0;
					values=featureMap[kn];
					for(int w=0;w<itersPerStream;w++){
						actValue_=values.range((((itersPerStream-1)-w)+1)*(accumBitWidth)-1,(((itersPerStream-1)-w)*(accumBitWidth)));
						actValue=tmp.data.range((((itersPerStream-1)-w)+1)*AWidth-1,(((itersPerStream-1)-w)*AWidth));
						//printf("Got value %d adding int to featuremap array value %d kn %d %d\n",actValue.to_int(),actValue_.to_int(),kn);
						actValue_+=actValue;
						//printf("result array value %d\n",actValue_.to_int());
						values_=(values_<<(accumBitWidth))+actValue_;
					}
					featureMap[kn]=values_;
				}


			}


		}
		//printf("sending out values\n");
		for(int kn=0; kn<zWiseValues; kn++){
		#pragma HLS PIPELINE  II=1
			values_=0;
			values=featureMap[kn];

			for(int w=0;w<itersPerStream;w++){
			#pragma HLS Unroll
				actValue_=values.range((((itersPerStream-1)-w)+1)*(accumBitWidth)-1,(((itersPerStream-1)-w)*(accumBitWidth)));
				//printf("Got Value %d to send out\n",actValue_.to_int());
				result=(actValue_*factor);
				result=(result>>(25+scale));
				if(result>=(1<<AWidth)-1) actValue = (1<<AWidth)-1;
				else actValue=result;


				//if(result.range(24,24))actValue++;
				//printf("Result %d to send out\n",actValue.to_int());
				values_=(values_<<(AWidth))+actValue;
			}

			tmpo.data=values_;

			strm_out.write(tmpo);

		}


	}else{

		if(loadWeights){
			for(int i=0;i<2;i++){
				while(!last){ //bias && filters
					tmp = strm_in.read();
					//printf("Received value %lu sending it back\n",tmp.data.to_long());
					tmpo.data=tmp.data;
					strm_out.write(tmpo);
					last=tmp.last;
				}
				last=0;
			}
		}


		for(int xy=0;xy<mapSize;xy++){
			for(int z=0;z<zWiseValues;z++){
				tmp = strm_in.read();
				//printf("Received value %lu sending it back\n",tmp.data.to_long());
				tmpo.data=tmp.data;
				strm_out.write(tmpo);
				last=tmp.last;
			}
		}



	}
}
