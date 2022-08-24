//-// MEMORY DEFINES //-//
#define DMAWidth 64
#define WWidth 4 //Bit width of weights
#define AWidth 4 //Bit width of activations/bias


#define streamItersBound ((outWidth-1)/DMAWidth+1)
#define WRemainder (DMAWidth%WWidth)
#define ARemainder (DMAWidth%AWidth)
#define BRemainder (DMAWidth%BWidth)

#define weightsPerStream (DMAWidth/WWidth)
#define actsPerStream (DMAWidth/AWidth)
#define biasPerStream (DMAWidth/BWidth)

#if weightsPerStream>actsPerStream
#define itersPerStream actsPerStream //SIMD
#else
#define itersPerStream weightsPerStream //SIMD
#endif


#define accumBitWidth 14 // log2(NumAdds * (2^AWidth)-1)+1   AWidth=8 :18  AWidth=4:14

#define KernelMaxN 2048 //Highest number of Channels Used

#define PRAGMA_SUB(x) _Pragma (#x)
#define PRAGMA_HLS(x) PRAGMA_SUB(x)





