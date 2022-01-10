#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <stdint.h>

//global variable for memory size
unsigned int global_mem_size = 0;


//used and free lists are implemented as linked lists
struct node 
{
   int size; //size of block
   void *location; //location of head of block in memory
   struct node *next;
};

struct node *used_head = NULL; //head pointer to used list
struct node *free_head = NULL; //head pointer to free list

void printlist(char list_type)
{
    struct node* cur;
    if(list_type == 'f')
    {
        //start from the first node
        cur = free_head;      
    }
    else if (list_type == 'u')
    {
        //start from the first node
        cur = used_head;
    }
    else
    {
        fprintf(stderr, "error: not valid list type\n");
        return;
    }

    while (cur != NULL)
    {
        printf("%p ", cur->location);
        cur = cur->next;
    }
    printf("\n");
    
}


//insert node in order of location in memory
//if list type = 'f', insert into free, if list type = 'u', insert into used
void insert(void *location, int size, char list_type) 
{
    //create a node
    struct node *block = (struct node*) malloc(sizeof(struct node));
	
    block->location = location;
    block->size = size;
    
    struct node* cur;
    if(list_type == 'f')
    {
        //start from the first node
        cur = free_head;      
    }
    else if (list_type == 'u')
    {
        //start from the first node
        cur = used_head;
    }
    else
    {
        fprintf(stderr, "error: not valid list type\n");
        return;
    }
    
    struct node* prev = NULL;

	//if list is empty, assign block as the head
    if(cur == NULL) 
    {
        //point block->next to NULL
        block->next = NULL;

        //point head to new first node
        if (list_type == 'f')
        {
            free_head = block;
        }
        else
        {
            used_head = block;
        }
        
    }
    //loop through the list
    else
    {
        while(cur != NULL && cur->location < block->location) 
        {
            //set prev to cur
            prev = cur;
            //move to next node
            cur = cur->next;
        }
        //insert block
        //if the list has one node
        if (prev == NULL)
        {
            if (list_type == 'f')
            {
                free_head = block;
                block->next = cur;
            }
            else
            {
                used_head = block;
                block->next = cur;
            }
            
        }
        else
        {
            prev->next = block;
            block->next = cur;
        }
    }
}


//delete a node from the used list with given memory location
void delete(unsigned char *location, char list_type) 
{
    struct node* cur;
    if(list_type == 'f')
    {
        //start from the first node
        cur = free_head;      
    }
    else if (list_type == 'u')
    {
        //start from the first node
        cur = used_head;
    }
    else
    {
        fprintf(stderr, "error: not valid list type\n");
        return;
    }
    

    struct node* prev = NULL;
	
    //if list is empty, return
    if(cur == NULL) 
    {
        fprintf(stderr, "error: list is empty, nothing to delete\n");
        return;
    }

    //loop through the list
    while(cur->location != location) 
    {
 
        //if it is the last node
        if(cur->next == NULL) 
        {
            fprintf(stderr, "error: couldn't find node to delete\n");
            return;
        } 
        else 
        { 
            //store reference to current node
            prev = cur;
            //move to next node
            cur = cur->next;
        }
    }

    //found a match, delete the node
    //if only one node in list
    if(cur == used_head || cur == free_head) 
    {
        if (list_type == 'f')
        {
            //change free head to point to next 
            free_head = free_head->next;
        }
        else
        {
            //change used head to point to next 
            used_head = used_head->next;
        }
    } 
    else 
    {
        //skip over the current node
        prev->next = cur->next;
    }    

}


//merge free blocks that are adjacent in memory
void merge_blocks(void)
{
   //add functionality to merge blocks
   struct node *cur = free_head;
   struct node *prev = NULL;

   //loop through the free list and merge adjacent blocks
   while(cur != NULL)
   {
      if (prev != NULL)
      {
         int size = prev->size;

         //if previous block is next to current block in memory, merge them
         if (prev->location + size == cur->location)
         {
            prev->size = cur->size + size;
            cur = cur->next;
            prev->next = cur;
         }
         
      }

      if (cur == NULL)
      {
          break;
      }
      
      //store reference to current node
      prev = cur;
      //move to next node
      cur = cur->next;
  }

}



void mem_init(unsigned char *my_memory, unsigned int my_mem_size)
{
    global_mem_size = my_mem_size;
    insert((void *) my_memory, my_mem_size, 'f'); //insert memory into free list

}

void *my_malloc(unsigned size)
{
  //if you are trying to allocate too much memory, send an error
  if(size > global_mem_size)
  {
    fprintf(stderr, "error: not enough memory available\n");
    return NULL;
  }
  
  struct node *cur = free_head;
  
  //loop through the free list and look for a block of memory that is large enough to fit size
  while(cur != NULL)

  {
    //if we found a large enough block
    if (cur->size >= size)

    {
      //if we found a block too big, split it and add remainder to the free list
      if (cur->size >= size * 2)

      {
        //insert into used
        insert(cur->location, size, 'u');
        void *temp = cur->location;

        //reduce the amount of memory in the block we found in the free list by size
        cur->location = cur->location + size;
        cur->size = cur->size - size;
        return temp;
      }
      
      //otherwise, just use the whole block
      else 
      {
        //insert into used
        insert(cur->location, cur->size, 'u');

        //delete from free
        delete(cur->location, 'f');

        return cur->location;
      }   
    }
    
    cur = cur->next;
  }
   
}

void my_free(void *mem_pointer)
{
  //merge adjacent free blocks
  merge_blocks();

  struct node *cur = used_head;

  //loop through the used list until you find the mem_pointer you are looking for 
  while(cur != NULL)
  {
    //if you found it, delete it from the used list and add it to the free list
    if(mem_pointer == cur->location)
    {
      insert(cur->location, cur->size, 'f');
      delete(cur->location, 'u');
      return;
    }
    cur = cur->next;
  }

  if (cur == NULL)
  {
    fprintf(stderr, "error: tried to free memory that is not allocated\n");
    return;
  }
  

}

typedef struct  {
    int num_blocks_used;
    int num_blocks_free;
    int smallest_block_free;
    int smallest_block_used;
    int largest_block_free;
    int largest_block_used;
} mem_stats_struct, *mem_stats_ptr;

void mem_get_stats(mem_stats_ptr mem_stats_ptr)
{
    //get stats for free
    struct node* free_cur = free_head;
    int num_blocks_free = 0;
    mem_stats_ptr->smallest_block_free = global_mem_size;
    mem_stats_ptr->largest_block_free = 0;

    if (free_cur == NULL)
    {
        mem_stats_ptr->smallest_block_free = 0;
    }

    //loop through all the nodes in free
    while (free_cur != NULL)
    {
        //find smallest free block
        if (free_cur->size < mem_stats_ptr->smallest_block_free)
        {
            mem_stats_ptr->smallest_block_free = free_cur->size;
        }
      
        //find largest free block
        if (free_cur->size > mem_stats_ptr->largest_block_free)
        {
            mem_stats_ptr->largest_block_free = free_cur->size;
        }
      
        //increment the number of blocks
        num_blocks_free++;
        free_cur = free_cur->next;
    }
   
    mem_stats_ptr->num_blocks_free = num_blocks_free;

    /*-------------------------------------------------*/
    //do the same for used list
    struct node* used_cur = used_head;
    int num_blocks_used = 0;
    mem_stats_ptr->smallest_block_used = global_mem_size;
    mem_stats_ptr->largest_block_used = 0;

    if (used_cur == NULL)
    {
        mem_stats_ptr->smallest_block_used = 0;
    }
    
    //loop through all the nodes in used
    while (used_cur != NULL)
    {
        //find smallest used block
        if (used_cur->size < mem_stats_ptr->smallest_block_used)
        {
            mem_stats_ptr->smallest_block_used = used_cur->size;
        }
      
        //find largest used block
        if (used_cur->size > mem_stats_ptr->largest_block_used)
        {
            mem_stats_ptr->largest_block_used = used_cur->size;
        }
      
        //increment the number of blocks
        num_blocks_used++;
        used_cur = used_cur->next;
    }
   
    mem_stats_ptr->num_blocks_used = num_blocks_used;
}

void print_stats(char *prefix) {
    mem_stats_struct mem_stats;

    mem_get_stats(&mem_stats);
    printf("mem stats: %s: %d free blocks, %d used blocks, free blocks: smallest=%d largest=%d, used blocks: smallest=%d largest=%d\n",
	prefix,
	mem_stats.num_blocks_free,
	mem_stats.num_blocks_used,
	mem_stats.smallest_block_free,
	mem_stats.largest_block_free,
	mem_stats.smallest_block_used,
	mem_stats.largest_block_used);
}

//try to malloc when all the memory is allocated
void test_malloc_nothing()
{
   unsigned int global_mem_size = 50;
   unsigned char *global_memory = malloc(global_mem_size);
   unsigned int size = 50;
   my_malloc(size);
   my_malloc(size);//if malloc returns an error, malloc is working
}

//try to free something that is not malloced
void test_free_nothing()
{
   unsigned int global_mem_size = 50;
   unsigned char *global_memory = malloc(global_mem_size);
   unsigned int size = 50;
   unsigned char *ptr = my_malloc(size);
   my_free(ptr);
   my_free(ptr);//if free returns an error, free is working
}

//try allocating more than the global memory size
void test_malloc_more()
{
   unsigned int global_mem_size = 50;
   unsigned char *global_memory = malloc(global_mem_size);
   unsigned int size = 50;
   unsigned char *ptr = my_malloc(size+50);//if malloc returns an error, malloc is working
}

//test to see if merge works
void test_merge()
{
    //insert one block next to another
    unsigned char *mem = malloc(50);
    insert(mem, 50, 'f');
    insert(mem+50, 50, 'f');

    //print the list before and after merging to see if it works
    printlist('f');
    merge_blocks();
    printlist('f');

    //deallocate the memory allocated
    delete(mem, 'f');
    delete(mem+50, 'f');
    free(mem);

}

int main(int argc, char **argv)
{
    test_merge();
    test_free_nothing();
    test_malloc_more();
    test_malloc_nothing();
    unsigned int global_mem_size = 1024 * 1024;
    unsigned char *global_memory = malloc(global_mem_size);

    mem_init(global_memory, global_mem_size);
    print_stats("init");

    unsigned char *ptr_array[10];
    unsigned int sizes[] = {50, 20, 20, 20, 50, 0};

    for (int i = 0; sizes[i] != 0; i++) {
        char buf[1024];
        ptr_array[i] = my_malloc(sizes[i]);
        
        sprintf(buf, "after iteration %d size %d", i, sizes[i]);
        print_stats(buf);
    }

    my_free(ptr_array[1]);  print_stats("after free #1");

    my_free(ptr_array[3]);  print_stats("after free #3");

    my_free(ptr_array[2]);  print_stats("after free #2");

    my_free(ptr_array[0]);  print_stats("after free #0");

    my_free(ptr_array[4]);  print_stats("after free #4");
}
