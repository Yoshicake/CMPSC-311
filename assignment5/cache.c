#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "cache.h"
#include "jbod.h"

// Uncomment the below code before implementing cache functions.
static cache_entry_t *cache = NULL;
static int cache_size = 0;
static int clock = 0;
static int num_queries = 0;
static int num_hits = 0;

// Creates a cache
int cache_create(int num_entries) {
    if (((num_entries >= 2) && (num_entries <= 4096)) && (cache == NULL)) {
        cache_size = num_entries;
        cache = malloc(cache_size * sizeof(cache_entry_t));
        for (int i = 0; i < cache_size; i++){
          cache[i].valid = false;
        }
        return 1;
    }
    return -1;
}


// Destroys a cache
int cache_destroy(void) {
  if (cache != NULL) {
    free(cache);
    cache = NULL;
    cache_size = 0;
    return 1;
  }
  return -1;
}

// Looks for a valid cache entry based on the parameters given. If found, copies into the buffer.
int cache_lookup(int disk_num, int block_num, uint8_t *buf) {
  if ((buf == NULL) || (cache == NULL)){
    return -1;
  }
  num_queries++;

  int i = 0;
  while (i < cache_size) {
    if ((cache[i].valid == true) && (disk_num == cache[i].disk_num) && (block_num == cache[i].block_num)){
        memcpy(buf, cache[i].block, JBOD_BLOCK_SIZE);
        num_hits++;
        clock++;
        cache[i].clock_accesses = clock;
        return 1;
    }
    i++;
  }
  return -1;
}

// Updates a cache entry with a block from buf
void cache_update(int disk_num, int block_num, const uint8_t *buf) {
  
  for (int i = 0; i < cache_size; i++){
    if ((cache[i].valid == true) && (cache[i].disk_num == disk_num) && (cache[i].block_num == block_num)){
      memcpy(cache[i].block, buf, JBOD_BLOCK_SIZE);
      clock++;
      cache[i].clock_accesses = clock;
    }
  }
}

int cache_insert(int disk_num, int block_num, const uint8_t *buf) {
  if ((buf == NULL) || (cache_size == 0) || (cache == NULL) || (disk_num < 0) || (disk_num > (JBOD_NUM_DISKS - 1))
      || (block_num < 0) || (block_num > (JBOD_BLOCK_SIZE - 1))) {
        return -1;
    }

    // Checks if cache block already exists.
    for (int i = 0; i < cache_size; i++){
      if((cache[i].valid == true) && (cache[i].disk_num == disk_num) && (cache[i].block_num == block_num)){
        return -1;
      }
    }
    

    // Checks if cache isn't full and updates the cache entry.
    for (int i = 0; i < cache_size; i++){
      if (cache[i].valid == false) {
        cache[i].valid = true;
        cache[i].disk_num = disk_num;
        cache[i].block_num = block_num;
        memcpy(cache[i].block, buf, JBOD_BLOCK_SIZE);
        clock++;
        cache[i].clock_accesses = clock;
        return 1;
      }
    }

    

    // Uses most recently used policy
    int mru_index = 0;

    for (int i = 0; i < cache_size; i++){
      if (cache[i].clock_accesses > cache[mru_index].clock_accesses){
        mru_index = i;
      }
    }

    cache[mru_index].valid = true;
    cache[mru_index].disk_num = disk_num;
    cache[mru_index].block_num = block_num;
    memcpy(cache[mru_index].block, buf, JBOD_BLOCK_SIZE);
    clock++;
    cache[mru_index].clock_accesses = clock;
    return 1;
}

// Boolean for if cache is enabled or not.
bool cache_enabled(void) {
  if (cache != NULL && cache_size > 0){
    return true;
  }
  return false;
}

void cache_print_hit_rate(void) {
	fprintf(stderr, "num_hits: %d, num_queries: %d\n", num_hits, num_queries);
  fprintf(stderr, "Hit rate: %5.1f%%\n", 100 * (float) num_hits / num_queries);
}

// Resizes cache
int cache_resize(int new_num_entries) {
  if ((new_num_entries < 2) && (new_num_entries > 4096)){
    return -1;
  }

  cache_entry_t *new_cache = malloc(new_num_entries * sizeof(cache_entry_t));

  if (new_cache == NULL){
    return -1;
  }

  int i = 0;

  while (i < new_num_entries){
    new_cache[i].valid = false;
    new_cache[i].clock_accesses = 0;
    i++;
  }

  int copied_size = 0;
  if (cache_size < new_num_entries) {
    copied_size = cache_size;
  } else {
    copied_size = new_num_entries;
  }

  i = 0;

  while (i < copied_size){
    if (cache[i].valid){
      new_cache[i] = cache[i];
    }
    i++;
  }

  free(cache);
  cache = new_cache;
  cache_size = new_num_entries;
  return 1;
}
