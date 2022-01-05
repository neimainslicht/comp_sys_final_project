#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>


//used and free lists are implemented as linked lists
struct node {
   int *size; //pointer to size of block
   int *global_size; //pointer to size of the entire memory
   void *location; //pointer to location
   struct node *next;
};

struct node *used_head = NULL; //head pointer to used list
struct node *used_cur = NULL; //current pointer to used list
struct node *free_head = NULL; //head pointer to free list
struct node *free_cur = NULL; //current pointer to free list

//insert node at the front of used list
void insert_used(void *location, int *size, int *global_size) {
   //create a node
   struct node *block = (struct node*) malloc(sizeof(struct node));
	
   block->location = location;
   block->size = size;
   block->global_size = global_size;
	
   //point it to old first node
   block->next = used_head;
	
   //point first to new first node
   used_head = block;
}
//insert node at the front of free list
void insert_free(void *location, int *size, int *global_size) {
   //create a node
   struct node *block = (struct node*) malloc(sizeof(struct node));
	
   block->location = location;
   block->size = size;
   block->global_size = global_size;
	
   //point it to old first node
   block->next = free_head;
	
   //point first to new first node
   free_head = block;
}

//delete a node from the used list with given memory location
struct node* delete_used(void *location) 
{

   //start from the first link
   struct node* cur = used_head;
   struct node* prev = NULL;
	
   //if list is empty
   if(used_head == NULL) 
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
         //store reference to current link
         prev = cur;
         //move to next link
         cur = cur->next;
      }
   }

   //found a match, update the link
   if(cur == used_head) 
   {
      //change first to point to next link
      free_head = used_head->next;
   } 
   else 
   {
      //bypass the current link
      prev->next = cur->next;
   }    

   return cur;
}

//delete a node from the free list with given memory location
struct node* delete_free(void *location) {

   //start from the first link
   struct node* cur = free_head;
   struct node* prev = NULL;
	
   //if list is empty
   if(free_head == NULL) {
      return NULL;
   }

   //loop through the list
   while(cur->location != location) {

      //if it is last node
      if(cur->next == NULL) {
         return NULL;
      } else {
         //store reference to current link
         prev = cur;
         //move to next link
         cur = cur->next;
      }
   }

   //found a match, update the link
   if(cur == free_head) {
      //change first to point to next link
      free_head = free_head->next;
   } else {
      //bypass the current link
      prev->next = cur->next;
   }    
	
   return cur;
}



typedef struct  {
  int num_blocks_used;
  int num_blocks_free;
  int smallest_block_free;
  int smallest_block_used;
  int largest_block_free;
  int largest_block_used;
} mem_stats_struct, *mem_stats_ptr;

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
    return -1;
  }

  //if there is one block of memory in the used list, reallocate
  else if(used_head->next == NULL)
  {
    
    //reallocate head to the size you need
    used_head = (void *) realloc(used_head, size);

    //set the next block of memory at the front of the free list containing global size - size amount of memory
    insert_free(used_head->location + size, used_head->global_size - size, used_head->global_size); 
  }

  //loop through the free list and look for a block of memory that is large enough to fit size
  while(free_head != NULL)
  {
    //if we found a large enough block, allocate enough memory and save the rest in free list
    if (free_head->size >= size)
    {
      //insert into used
      insert_used(free_head->location, size, free_head->global_size);

      //reduce the amount of memory in the block we found in the free list by size
      free_head->location = free_head->location + size;
      free_head->size = free_head->size - size;
      
      
    }
    
    free_head = free_head->next;
  }
  
}

void my_free(void *mem_pointer)
{

}

void mem_get_stats(mem_stats_ptr mem_stats_ptr)
{

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

