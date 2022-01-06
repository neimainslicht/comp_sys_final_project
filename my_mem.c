#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>


//used and free lists are implemented as linked lists
struct node 
{
   int *size; //pointer to size of block
   int *global_size; //pointer to size of the entire memory
   void *location; //pointer to location
   struct node *next;
};

struct node *used_head = NULL; //head pointer to used list
struct node *free_head = NULL; //head pointer to free list

//change this to insert by location
//insert node in order of location in memory
void insert_used(void *location, int *size, int *global_size) 
{
   //create a node
   struct node *block = (struct node*) malloc(sizeof(struct node));
	
   block->location = location;
   block->size = size;
   block->global_size = global_size;

   //start from the first node
   struct node* cur = used_head;
   struct node* prev = NULL;
	
   //if list is empty, assign block as the head
   if(free_head == NULL) 
   {
      //point block->next to NULL
      block->next = NULL;
	
      //point head to new first node
      free_head = block;
   }

   //loop through the list
   while(cur->location < location) 
   {
      //set prev to cur
      prev = cur;
      //move to next node
      cur = cur->next;
   }

   //insert block
   prev->next = block;
   block->next = cur;    

}

//insert node in order of location in memory
void insert_free(void *location, int *size, int *global_size) 
{
   //create a node
   struct node *block = (struct node*) malloc(sizeof(struct node));
	
   block->location = location;
   block->size = size;
   block->global_size = global_size;

   //start from the first node
   struct node* cur = free_head;
   struct node* prev = NULL;
	
   //if list is empty, assign block as the head
   if(free_head == NULL) 
   {
      //point block->next to NULL
      block->next = NULL;
	
      //point head to new first node
      free_head = block;
   }

   //loop through the list
   while(cur->location < location) 
   {
      //set prev to cur
      prev = cur;
      //move to next node
      cur = cur->next;
   }

   //insert block
   prev->next = block;
   block->next = cur;    

}

//delete a node from the used list with given memory location
struct node* delete_used(void *location) 
{
   //start from the first node
   struct node* cur = used_head;
   struct node* prev = NULL;
	
   //if list is empty, return NULL
   if(used_head == NULL) 
   {
      return NULL;
   }

   //loop through the list
   while(cur->location != location) 
   {

      //if it is the last node
      if(cur->next == NULL) 
      {
         return NULL;
      } 
      else 
      {
         //store reference to current node
         prev = cur;
         //move to next node
         cur = cur->next;
      }
   }

   //found a match, update the node
   if(cur == used_head) 
   {
      //change head to point to next 
      used_head = used_head->next;
   } 
   else 
   {
      //skip over the current node
      prev->next = cur->next;
   }    

   return cur;
}

//delete a node from the free list with given memory location
struct node* delete_free(void *location) 
{

   //start from the first node
   struct node* cur = free_head;
   struct node* prev = NULL;
	
   //if list is empty
   if(free_head == NULL) 
   {
      return NULL;
   }

   //loop through the list
   while(cur->location != location) 
   {

      //if it is last node
      if(cur->next == NULL) 
      {
         return NULL;
      } 
      else 
      {
         //store reference to current node
         prev = cur;
         //move to next node
         cur = cur->next;
      }
   }

   //found a match, update the node
   if(cur == free_head) 
   {
      //change head to point to next 
      free_head = free_head->next;
   } 
   else 
   {
      //skip over the current node
      prev->next = cur->next;
   }    
	
   return cur;
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
      
      //store reference to current node
      prev = cur;
      //move to next node
      cur = cur->next;
  }

}



void mem_init(unsigned char *my_memory, unsigned int my_mem_size)
{
  insert_used(my_memory, my_mem_size, my_mem_size); //initialize pointer to memory pool
  insert_free(my_memory, my_mem_size, my_mem_size); //initialize pointer to free list

}

void *my_malloc(unsigned size)
{
  //if you are trying to allocate too much memory, send an error
  if(size > used_head->global_size)
  {
    fprintf(stderr, "error: not enough memory available");
    return NULL;
  }
  
  struct node *cur = free_head;
  
  //loop through the free list and look for a block of memory that is large enough to fit size
  while(cur != NULL)

  {
    //if we found a large enough block, allocate enough memory and save the rest in free list
    if (cur->size >= size)

    {
      //if we found a block too big, split it and add remainder to the free list
      if (cur->size >= size * 2)

      {
        //insert into used
        insert_used(cur->location, size, cur->global_size);

        //reduce the amount of memory in the block we found in the free list by size
        cur->location = cur->location + size;
        cur->size = cur->size - size;
      }
      
      //otherwise, just use the whole block
      else 
      {
        //insert into used
        insert_used(cur->location, cur->size, cur->global_size);

        //delete from free
        delete_free(cur->location);
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
    if(cur->location == mem_pointer)
    {
      insert_free(cur->location, cur->size, cur->global_size);
      delete_used(cur->location);
    }
    cur = cur->next;
  }

  if (cur == NULL)
  {
    fprintf(stderr, "error: tried to free memory that is not allocated");
    return NULL;
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
   int smallest_size_free = free_head->global_size;
   int largest_size_free = 0;
   mem_stats_ptr->smallest_block_free = 0;
   mem_stats_ptr->largest_block_free = 0;

   //loop through all the nodes in free
   while (free_cur != NULL)
   {
      //find smallest free block
      if (free_cur->size < smallest_size_free)
      {
         mem_stats_ptr->smallest_block_free = (int) free_cur->location;
      }
      
      //find largest free block
      else if (free_cur->size > largest_size_free)
      {
         mem_stats_ptr->largest_block_free = (int) free_cur->location;
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
   int smallest_size_used = used_head->global_size;
   int largest_size_used = 0;
   mem_stats_ptr->smallest_block_used = 0;
   mem_stats_ptr->largest_block_used = 0;

   //loop through all the nodes in free
   while (used_cur != NULL)
   {
      //find smallest free block
      if (used_cur->size < smallest_size_used)
      {
         mem_stats_ptr->smallest_block_used = (int) used_cur->location;
      }
      
      //find largest free block
      else if (used_cur->size > largest_size_used)
      {
         mem_stats_ptr->largest_block_used = (int) used_cur->location;
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

int main(int argc, char **argv)
{
  unsigned int global_mem_size = 1024 * 1024;
  unsigned char *global_memory = malloc(global_mem_size);

  mem_init(global_memory, global_mem_size);
  print_stats("init");

  unsigned char *ptr_array[10];
  unsigned int sizes[] = {50, 20, 20, 20, 50, 0};

  for (int i = 0; sizes[i] != 0; i++) {
    char buf[1024];
    //my_malloc returns an unspecified pointer
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

