//////////////////////////////////////////////////////////////////////////////
// prog: mutex.c
// comm: demonstrates the usage of mutual exclusion semaphores
// auth: MSC
//////////////////////////////////////////////////////////////////////////////

#include <includes.h>
   
#include <lcd.h>
#include <buzzer.h>

#include <main.h>

//////////////////////////////////////////////////////////////////////////////
// func: MutexTask0
// args: void *pdata, needed by os
// comm: to test semaphores; func waits for semaphore, then plays with leds,
//       then releases the sem 
//////////////////////////////////////////////////////////////////////////////
void MutexTask0 (void *pdata)
{
    OS_TCB inf;
    
    OSTaskQuery(OS_PRIO_SELF,&inf);
    
    /* A task is usually an endless loop */
    while(TRUE) 
    {
        UART_put("\n\r task: ");
        UART_putint(inf.OSTCBPrio);
        UART_put(" alive!");
        OSTimeDly(LOOP_DELAY);
    }
}
