#define PRAGMA_SUB(x) _Pragma (#x)
#define PRAGMA_HLS(x) PRAGMA_SUB(x)0


//-// MEMORY DEFINES //-//
#define WWidth 8 //Bit width of weights
#define AWidth 8 //Bit width of activations/bias

#define weightsPerStream (64/WWidth)
#define actsPerStream (64/AWidth)

#if weightsPerStream>actsPerStream
#define itersPerStream actsPerStream
#else
#define itersPerStream weightsPerStream
#endif

#define FilterMaxN 1 //Highest number of Channels Used
#define KernelMaxN 10//2048 //Highest number of Channels Used
#define KernelMaxSize 3 // Highest Size of Kernel Used X & Y

#define MapMaxXSize 32 //Highest number of X stored until Bram increases
#define MapMaxYSize KernelMaxSize //Number of Y Lines Stored == Size of Kernel Y
#define MapMaxN KernelMaxN


//-// LOOP DEFINES //-//

#define LOOPFilterMaxN 32
#define LOOPKernelMaxN KernelMaxN
#define LOOPKernelMaxSize KernelMaxSize

#define LOOPMapMaxXSize 1024
#define LOOPMapMaxYSize LOOPMapMaxXSize

