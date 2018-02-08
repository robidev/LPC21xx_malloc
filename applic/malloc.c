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
 * File        : malloc.c
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
 */


/*
 * Tweakable parameters
 */

/* BLOCK_ALIGN
 *
 * This defines the minimum size for a block that can be allocated. If you 
 * choose to edit this value, make sure that the calls to malloc do not throw
 * off the alignment for the folowing call(malloc connects all the blocks from
 * head to tail). At default it is set to 8 bytes, which is the size of the 
 * structure that malloc uses to keep track of a block. Any wrong alignment 
 * usually resuls in data abort exeptions, and corrupt block information for
 * the following calls.
 */
 
#define BLOCK_ALIGN       8

/* STACK_MARGIN
 *
 * this defines the margin between the maximum stack and heap size in bytes,
 * as the stack sometimes uses some space for stack overflow checking.
 */

#define STACK_MARGIN      16

/*
 * No user serviceable parts behind this point.
 */
 
/*
 * Where the global function definitions reside:
 */
#include "malloc.h"


// Structure that describes a block.
typedef struct memory_block_header {
    unsigned int                 size;//means that there is a max of 4 gigs per block
    struct memory_block_header  *next;
    /* Actual memory block starts here */
} memory_block_header;


/*
 * Global variabeles
 *
 * These variabeles are used by the functions to keep track of information as
 * where the stack begins, the heap ends, and where free memory is located. 
 */

memory_block_header *free_memory_blocks;//this is the pointer to the start of the free list

unsigned char * global_stack_ptr;//global pointer to maximum heap size
unsigned char * heap_end;//global pointer to current heap size


/*
 * Local function prototypes
 *
 * These functions may be acessible from the outside, as they can break stuff.
 */

/* _sbrk
 *
 * function that actually claims/releases a piece of memory between _end and stack.
 * partly copied from the libgloss library(arm port)
 */
volatile unsigned char * _sbrk (int incr);



/*
 * Function implementations
 */

/* init_malloc
 *
 * call to initialise heap space, goes from _end to stack, grows upwards.
 *
 * WARNING: makes use of current maximum stack size. If origin of call is
 * from within a separated stack, which is not located after the heap space
 * (_end), this will return with an error code(-2).
 */ 
 
int init_malloc(void)
{
    register unsigned char * stack_ptr asm ("sp");//pointer to maximum stack size
    extern unsigned char   end asm ("end");/* Defined by the linker. heap comes */

    if((stack_ptr - STACK_MARGIN) < (& end))
        return -2;//maximum stack size is under _end, so no heap is available
    
    heap_end = & end;//do it now
        
    global_stack_ptr=(stack_ptr - STACK_MARGIN);
    free_memory_blocks = NULL;
    return 0;
}


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
 
int update_heap_size(void)
{
    register unsigned char * stack_ptr asm ("sp");//pointer to maximum stack size
    extern unsigned char   end asm ("end");/* Defined by the linker. heap comes */
    
    if((stack_ptr - STACK_MARGIN) < (& end))
        return -2;//new maximum stack pointer lies beneath begining of heap(under _end).
    
    if(heap_end>(stack_ptr - STACK_MARGIN))
        return -1;//new maximum stack pointer is allready in used memory space
        
    global_stack_ptr=(stack_ptr - STACK_MARGIN);
    return 0;//all is well, heap maximum is changed.
}


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
 
void * malloc(unsigned int size)
{
    memory_block_header *h;
    memory_block_header *previous;
    
    if(size<1)//no memory asked, so no pointer returned.
        return NULL;
    
    if((size % BLOCK_ALIGN)>0)//check if block is multiple of BLOCK_ALIGN bytes
    {//if not, make it aligned by allocating extra bytes.
	    size -= size % BLOCK_ALIGN;
	    size += BLOCK_ALIGN;
	}
    
    previous = NULL;
    //check for free blocks, minimum is BLOCK_ALIGN bytes + header size
    
    //find fitting piece of mem
    for (h = free_memory_blocks; h != NULL; h = h->next)
    {
        if (h->size >= size)
        {
            //if there is no room for additional free space
            if(h->size < (size + sizeof(memory_block_header) + BLOCK_ALIGN))
            {// unlink allocated block from list
                if (previous == NULL)
                    free_memory_blocks = h->next; // allocated block is first block on list
                else
                    previous->next = h->next;
                
            }    
            else//there is room for a additional free space, so split
            {
                h->size -= (size + sizeof(memory_block_header));
                (char *)h = ((char *)h + h->size + sizeof(memory_block_header));//add used memory at end
                h->size = size;
            }
            return h+1;// Address following h; is the actual block of data   
        }   
        previous = h;
    }
    
    //new piece of mem
    (unsigned char *)h = _sbrk(sizeof (memory_block_header) + size);
    
    if (h == (memory_block_header *)-1) // no memory availible
        return NULL;
    
    h->size = size;
    h->next = NULL;
    
    return h + 1;
}


/* free
 * 
 * call to free a block of memory, that is allocated by malloc. Tries to merge
 * free blocks and resizes heap if memory is freed at the end of the heap.
 */
 
void free(void * mem_chunk)
{
    extern unsigned char   end asm ("end");// Defined by the linker. heap comes here after
    memory_block_header *h;
    memory_block_header *n;
    memory_block_header *previous=NULL;
    
    if((char *)mem_chunk == NULL)//check if pointer != NULL
        return;

    //check if pointer between bss end and stack top.
    if(((unsigned char *)mem_chunk < (unsigned char *)(& end)) || ((unsigned char *)mem_chunk > heap_end))
        return;
        
    h = (memory_block_header *) mem_chunk;
    h = h - 1;   // Back up to the header itself

    //place block in mem at right place
    //and merge with ajacent free blocks      
    for(n = free_memory_blocks; n != NULL; n = n->next)
    {
        if(n > h)//found next free space
        {
            if(previous == NULL)//first in list
            {
                if((char *)n == (char *)h + h->size + sizeof(memory_block_header))//space can be merged with next free block
                {
                     h->next = n->next;//point to next free block
                     h->size += n->size + sizeof(memory_block_header); //merge size
                }
                else// space can not be merged
                {
                     h->next = n;// just add the item on front of list
                }
                free_memory_blocks = h;//redefine the start of the list
            }
            else//not first in list
            {
                if((char *)previous + previous->size + sizeof(memory_block_header) == (char *)h)//space can be merged
                {//with previous free block
                    previous->size += h->size + sizeof(memory_block_header);//just add it to the previous block
                    h = previous;//merge pointers
                }
                else// space can not be merged with previous block
                {
                    previous->next = h;//redefine 'next' pointer of previous
                }
                if((char *)n == (char *)h + h->size + sizeof(memory_block_header))//space can be merged
                {//with next free block
                     h->next = n->next;//point to next free block
                     h->size += n->size + sizeof(memory_block_header);//add size to current block
                }
                else// space can not be merged with next free block
                {
                    h->next = n;//point to next in list
                }
            }
            return;
        }  
        previous = n;
    }  
    //block is located after last free block
    if(n==NULL)
    {
	    if(previous == NULL)//no list defined yet
	    {   //the last piece, so add to list
            free_memory_blocks = h;
            h->next=NULL;//last in list, so next is NULL
		}
		else//list defined, so append
        {
            if((char *)previous + previous->size + sizeof(memory_block_header) == (char *)h)//space can be merged
            {//with previous free block
                previous->size += h->size + sizeof(memory_block_header);//just add it to the previous block
                h = previous;//merge pointers
            }
            else// space can not be merged with previous block
			{
			    previous->next = h;
			    h->next=NULL;//last in list, so next is NULL
			}
        }
        //if it is last block in heap, give mem back to system.
        if( (unsigned char *)((char *)h + h->size + sizeof(memory_block_header)) >= heap_end)//chunk is last in heap
        {
            _sbrk(0 - (h->size + sizeof(memory_block_header)));//return all mem
            if(previous == NULL)
                free_memory_blocks = NULL;
            else//ugly hack, but better than implementing a doubly linked list
            {
                if(free_memory_blocks == h)
                    free_memory_blocks = NULL;
                    
                for(n = free_memory_blocks; n != NULL; n = n->next)
                {
                    if(n->next == h)
                        n->next = NULL;
				}
			}
        }
    }    
    return;
}


/* _sbrk
 *
 * function that actually claims/releases a piece of memory between _end and stack.
 * partly copied from the libgloss library(arm port)
 */
 
volatile unsigned char * _sbrk (int incr)
{
    extern unsigned char   end asm ("end");// Defined by the linker. heap comes here after
    unsigned char *        prev_heap_end;
  
    prev_heap_end = heap_end;
  
    //check if the block is located in heap.
    if((heap_end + incr > global_stack_ptr) || (heap_end + incr < (&end)))
    {
        return (volatile unsigned char *) -1;
    }
  
    heap_end += incr;

    return (volatile unsigned char *) prev_heap_end;
}

