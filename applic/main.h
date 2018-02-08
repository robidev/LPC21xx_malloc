//////////////////////////////////////////////////////////////////////////////
// prog: main.h
// comm: external definitions and prototypes for main
// auth: MSC
//////////////////////////////////////////////////////////////////////////////

//#define OUTPUT_DATA
#define OUTPUT_DEBUG

 
#define INITTASK_PRIORITY        5

#define WAIT_FOREVER             0
#define STACK_SIZE               500
#define LOOP_DELAY               20   


// function prototypes of threads/tasks/processes to prevent compiler warnings
extern void DisplayOSData (void);
extern void InitTask      (void *pdata);
extern void MutexTask0    (void *pdata);

