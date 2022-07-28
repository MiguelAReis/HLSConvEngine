//-// MEMORY DEFINES //-//
#define DMAWidth 64
#define WWidth 16 //Bit width of weights
#define AWidth 16 //Bit width of activations/bias

#define numPEs 4 //Num of PE's (Has to be multiple eg. for AWidth == 16 && DMAWidth==64 numPEs = 4,8,12,16 etc)

#define IIValue (outWidth/DMAWidth)

#define outWidth numPEs*AWidth
#define streamItersBound ((outWidth-1)/DMAWidth+1)
#define WRemainder (DMAWidth%WWidth)
#define ARemainder (DMAWidth%AWidth)

#define weightsPerStream (DMAWidth/WWidth)
#define actsPerStream (DMAWidth/AWidth)

#if weightsPerStream>actsPerStream
#define itersPerStream actsPerStream //SIMD
#else
#define itersPerStream weightsPerStream //SIMD
#endif


#define KernelMaxN 1024//2048 //Highest number of Channels Used
#define MaxBRAMValuesForKernelMaxN ((KernelMaxN-1)/itersPerStream+1) //Highest number of Channels Used
#define KernelSize 3 // Highest Size of Kernel Used X & Y
#define KernelSizeSquared 9
#define KernelSizeSquaredPlus (KernelSizeSquared+KernelSize)

#define MapMaxXSize 32 //Highest number of X stored until Bram increases
#define MapMaxYSize KernelMaxSize //Number of Y Lines Stored == Size of Kernel Y
#define MapMaxN KernelMaxN



//-// LOOP DEFINES //-//


#define LOOPKernelMaxN KernelMaxN
#define LOOPKernelMaxSize KernelMaxSize

#define LOOPMapMaxXSize 1024
#define LOOPMapMaxYSize LOOPMapMaxXSize

//-//

#define PRAGMA_SUB(x) _Pragma (#x)
#define PRAGMA_HLS(x) PRAGMA_SUB(x)
