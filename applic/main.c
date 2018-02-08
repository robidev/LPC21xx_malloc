//////////////////////////////////////////////////////////////////////////////
// prog: main.c
// comm: demonstrates the creating tasks
// auth: MSC
//////////////////////////////////////////////////////////////////////////////

#include <includes.h>

#include <lcd.h>
#include <main.h>
#include "malloc.h"

// Create different stacks for the tasks
// give each thread/task/process its own stack with size
// stacks are allocated here statically, because malloc() is not supported
OS_STK InitTaskStk     [STACK_SIZE];

// put some debug-output to uart
void DisplayOSData(void)
{
    UART_put("\n\rmain.c: system reset\n\r");
    UART_put("uC/OS-II versie ");
    UART_putint(OSVersion());
    UART_put(" voor de LPC2106\n\r");
    UART_put("processor op ");
    UART_putint(CCLK/1000000);
    UART_put(" MHz, met perhiperals op ");
    UART_putint(CCLK/PBSD/1000000);
    UART_put(" MHz\n\r");
}

//////////////////////////////////////////////////////////////////////////////
// func: InitTask
// args: void *pdata, needed by os
// comm: This is the initialisation-task. It's the first task to run, and 
//       it will perform some tasks that need to be done after uC/OS-II 
//       has been started. Most important task here is starting the timer.
// note: this task deletes itself when finished     
//////////////////////////////////////////////////////////////////////////////

//unsigned char zin[11]="1234567890";
void InitTask(void *pdata)
{
    int i;
    OS_STK * bla;

    /* Start timer; after this, uC/OS-II is fully running */
    timer_init();

    /* Initialise statistics task. 
	This executes a task every second which calculates
    CPU-usage. Result can be found in OSCPUUsage, which is usage in percents */
    OSStatInit();
    
    DisplayOSData(); // output to uart of some data
    
    for(i=9;i</*29*/49;i++)
    {
        bla = (OS_STK *)malloc(STACK_SIZE * sizeof(OS_STK));
        if(bla == NULL)
        {
            break;
        }  
        OSTaskCreateExt(MutexTask0, NULL, &bla[STACK_SIZE-1],i,
                    i, bla, STACK_SIZE, NULL, OS_TASK_OPT_STK_CHK);
    }              


    UART_put("\n\r heap fully filled with dynamic allocated tasks \n\r");

    UART_put("\n\r bytes allocated: ");
    UART_putint((i-9)*STACK_SIZE*sizeof(OS_STK));

    UART_put("\n\r for ");
    UART_putint((i-9));
    UART_put(" tasks");
    UART_put("\n\r with a stack size of ");
    UART_putint(STACK_SIZE*sizeof(OS_STK));
    UART_put(" bytes");
    UART_put("\n\r maximum number of tasks: ");
    UART_putint(OS_MAX_TASKS);
    UART_put("\n\r");


    
    LCD_init();   	
    LCD_clear(); 
	LCD_cursor_home();

    /* This task is no longer needed; delete it */
   	OSTaskDel(OS_PRIO_SELF);
}

int main (void)
/*
  This is the main-function from the C-code. It's called from crt0.s after the needed low-level stuff is done. It's supposed to be an endless loop.
*/
{
    init_malloc();
    /* The first thing to do when starting uC/OS-II; initialise it */
    OSInit();
    /* Initialize hardware */
    BSP_init();
    /* Create the initialisation-task */
    OSTaskCreate(InitTask, NULL, &InitTaskStk[STACK_SIZE-1], INITTASK_PRIORITY);
    /* Now start multitasking. This function should not exit */
    OSStart();

    /* If it did exit, return whit an errorcode. Will be handled in crt0.s */
    
    return TERM;
}

