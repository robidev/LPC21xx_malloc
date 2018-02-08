/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <robin.dev@gmail.com> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Robin Massink
 * ----------------------------------------------------------------------------
 *
 * processor   : LPC2106
 *
 * File        : malloc.h
 * Version     : 1.0
 * By          : Robin Massink <robin.dev@gmail.com>
 *
 * Toolchain   : GCC
 * Description : malloc function
 *
 * This malloc function tries to cope with memory effinciently, without re-
 * organising any used blocks, and with returning aligned blocks of memory.
 * It also allows for the stack/heap to expand/shrink dynamically.
 * It uses a first-fit strategy, and tries to keep any excess memory available.
 * It has a minimum size for blocks, so any size under this will be resized to
 * this.
 *
 *
 * External dependancy
 *
 * The only external data this code relies on is the 'end' definition in the
 * linkerscript. this definition is normaly located after the .bss section.
 *
 * ( Any parameters that can be tweaked are at the top of malloc.c )
 */


#ifndef   MALLOC_H
#define   MALLOC_H

/*
 * Global defines
 */

//if it does not allready exist, have a NULL pointer...
#ifndef NULL
#define NULL            0
#endif


/*
 * Global functions
 */

/* init_malloc
 *
 * call to initialise heap space, goes from _end to stack, grows upwards.
 *
 * WARNING: makes use of current maximum stack size. If origin of call is
 * from within a separated stack, which is not located after the heap space
 * (_end), this will return with an error code(-2).
 */ 
int      init_malloc(void);


/* update_heap_size
 *
 * call if global stack is changed, to keep heap out of the stack space
 *
 * WARNING: makes use of current maximum stack size. If origin of call is
 * from within a separated stack, which is not located after the heap space
 * (_end), this will return with an error code(-2).
 * 
 * If new maximum stack pointer is allready in the used portion of the heap,
 * this will return with an error code(-1)
 */ 
int      update_heap_size(void);


/* malloc
 *
 * call to allocate a block of memory. Tries to keep the heap small, using first-
 * fit strategy, while not claiming any excess memory. Blocks are a multiple of
 * BLOCK_ALIGN, to avoid data exeptions on the ARM.
 *
 * If an error occured, or there is no memory available anymore;
 * this returns NULL.
 *
 * (this algorithm could be better by checking where the piece fits best, so
 * fragmentation would be kept to a minimum)
 */
void    *malloc(unsigned int size);


/* free
 * 
 * call to free a block of memory, that is allocated by malloc. Tries to merge
 * free blocks and resizes heap if memory is freed at the end of the heap.
 */
void     free(void * mem_chunk);


#endif    /*MALLOC_H*/
