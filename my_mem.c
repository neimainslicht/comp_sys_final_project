#include <stdio.h>

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

}

void *my_malloc(unsigned size)
{

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

