//-// MEMORY DEFINES //-//
#define DMAWidth 64
#define WWidth 4 //Bit width of weights
#define AWidth 4 //Bit width of activations/bias
#define BWidth 8 //must be bigger than previous bit widths
#define accumBitWidth 23 // log2(NumAdds * (2^(AWidth+WWidth)-1)+1  // Num adds = 9*2048+1

#define numPEs 16 //Num of PE's (Has to be multiple eg. for AWidth == 16 && DMAWidth==64 numPEs = 4,8,12,16 etc)

#define IIValue (outWidth/DMAWidth)

#define outWidth numPEs*AWidth
#define streamItersBound ((outWidth-1)/DMAWidth+1)
#define WRemainder (DMAWidth%WWidth)
#define ARemainder (DMAWidth%AWidth)
#define BRemainder (DMAWidth%BWidth)

#define weightsPerStream (DMAWidth/WWidth)
#define actsPerStream (DMAWidth/AWidth)
#define biasPerStream (DMAWidth/BWidth)

#define biasJump 2

#if weightsPerStream>actsPerStream
#define itersPerStream actsPerStream //SIMD
#else
#define itersPerStream weightsPerStream //SIMD
#endif
#define biasIterFactor (itersPerStream/biasPerStream)

#define FilterMaxN 16 //Highest number of Channels Used
#define FilterMaxNPerPE ((FilterMaxN-1)/numPEs+1) //Highest number of Channels Used
#define BiasMaxN 2048
#define KernelMaxN 2048//2048 //Highest number of Channels Used
#define KernelMaxSize 3 // Highest Size of Kernel Used X & Y

#define MapMaxXSize 150 //Highest number of X stored until Bram increases
#define MapMaxYSize KernelMaxSize //Number of Y Lines Stored == Size of Kernel Y
#define MapMaxN 128


//-// LOOP DEFINES //-//

#define LOOPFilterMaxN 32
#define LOOPKernelMaxN KernelMaxN
#define LOOPKernelMaxSize KernelMaxSize

#define LOOPMapMaxXSize 1024
#define LOOPMapMaxYSize LOOPMapMaxXSize

//-//

#define PRAGMA_SUB(x) _Pragma (#x)
#define PRAGMA_HLS(x) PRAGMA_SUB(x)





