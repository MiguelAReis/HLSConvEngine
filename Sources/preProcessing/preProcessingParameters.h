//-// MEMORY DEFINES //-//
#define DMAWidth 64
#define WWidth 4 //Bit width of weights
#define AWidth 4 //Bit width of activations/bias
#define BWidth 8 //must be bigger than previous bit widths

#define numPEs 8 //Num of PE's (Has to be multiple eg. for AWidth == 16 && DMAWidth==64 numPEs = 4,8,12,16 etc)

#define IIValue (outWidth/DMAWidth)

#define outWidth numPEs*AWidth
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
#define biasIterFactor (itersPerStream/biasPerStream)


#define accumBitWidth 18 // log2(NumAdds * (2^AWidth)-1)+1   AWidth=8 :18  AWidth=4:14

#define KernelMaxN 32 //Highest number of Channels Used

#define MapMaxXSize 32 //Highest number of X stored until Bram increases
#define MapMaxYSize KernelMaxSize //Number of Y Lines Stored == Size of Kernel Y
#define MapMaxN KernelMaxN


//-// LOOP DEFINES //-//

#define LOOPFilterMaxN 32
#define LOOPKernelMaxN KernelMaxN
#define LOOPKernelMaxSize KernelMaxSize

#define LOOPMapMaxXSize 1024
#define LOOPMapMaxYSize LOOPMapMaxXSize

//-//

#define PRAGMA_SUB(x) _Pragma (#x)
#define PRAGMA_HLS(x) PRAGMA_SUB(x)





