#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "jbod.h"
#include "mdadm.h"

int mounted = 0;
int write_perm = 0;

int mdadm_mount(void) {
  // Checks if the disk is already mounted.
  if (mounted == 1) {
    return -1;
  } 
  jbod_operation(encode(JBOD_MOUNT,0,0), NULL);
  mounted = 1;
  return 1;
}

int mdadm_unmount(void) {
  // Checks if the disk is already unmounted.
  if (mounted == 0) {
    return -1;
  } 
  jbod_operation(encode(JBOD_UNMOUNT,0,0), NULL);
  mounted = 0;
  return 1;
}

uint32_t encode(uint32_t command, uint32_t diskID, uint32_t blockID){
  // Initialize an op for the 32 bit disk operation.
  uint32_t op = 0;

  // Shift the blockID's bits to the left by 4
  blockID = blockID << 4;

  // Shift the command's bits by 12.
  command = command << 12;

  // Use "or" operation to create the full operation to return.
  op = op | diskID | blockID | command;
  return op;
}

int mdadm_write_permission(void){
 
	// Checks if write perm is already enabled.
  if (write_perm == 1) {
    return -1;
  } 

  jbod_operation(encode(JBOD_WRITE_PERMISSION,0,0), NULL);
  write_perm = 1;
  return 1;
}


int mdadm_revoke_write_permission(void){

	// Checks if the write perm is already disabled.
  if (write_perm == 0) {
    return -1;
  } 
  jbod_operation(encode(JBOD_REVOKE_WRITE_PERMISSION,0,0), NULL);
  write_perm = 0;
  return 1;
}


int mdadm_read(uint32_t start_addr, uint32_t read_len, uint8_t *read_buf)  {
  int megabyte = 1048576;

  // Error cases
  if ((start_addr + read_len) > megabyte){
    return -1;
  }
  if (read_len > 1024){
    return -2;
  }
  if (mounted == 0){
    return -3;
  }
  if (start_addr < 0 || read_len < 0 || ((read_buf == NULL) && (read_len != 0))) {
    return -4;
  }

  uint32_t diskID = start_addr / JBOD_DISK_SIZE;

  uint32_t blockID = (start_addr % JBOD_DISK_SIZE) / JBOD_BLOCK_SIZE;

  // Starting position of a block
  uint32_t offset =  start_addr % JBOD_BLOCK_SIZE;

  // Temporary storage to put in the read_buf
  uint8_t buffer[JBOD_BLOCK_SIZE];

  int retval = read_len;
  int bufferPtr = 0;

   // Seek to Disk
  uint32_t seekToDisk = encode(JBOD_SEEK_TO_DISK, diskID, blockID);
  jbod_operation(seekToDisk, NULL);

  // Seek to Block
  uint32_t seekToBlock = encode(JBOD_SEEK_TO_BLOCK, diskID, blockID);
  jbod_operation(seekToBlock, NULL);

  // Reads 
  while (read_len > 0){

    if (cache_lookup(diskID, blockID, buffer) == 1) {
    // Cache hit: copy data directly to read_buf
      uint32_t read_amt = JBOD_BLOCK_SIZE - offset;
      if (read_amt > read_len) {
        read_amt = read_len;
      }

      memcpy(read_buf + bufferPtr, buffer + offset, read_amt);
      read_len -= read_amt;
      bufferPtr += read_amt;

      
      blockID++;
      if (blockID == JBOD_NUM_BLOCKS_PER_DISK) {
        blockID = 0;
        diskID++;
        if (read_len > 0) {
          jbod_operation(encode(JBOD_SEEK_TO_DISK, diskID, 0), NULL);
        }
      }

      offset = 0; 
      continue; 
    }

    // Cache miss: reads block from diskand puts it in the temporary buffer
    uint32_t readOp = encode(JBOD_READ_BLOCK, 0, 0);
    jbod_operation(readOp, buffer);

    // Adjusts read_amt as necessary with the offset.
    uint32_t read_amt = JBOD_BLOCK_SIZE - offset;
    if (read_amt > read_len){
      read_amt = read_len;
      
    }
    
    // Copies temp buffer data to read_buf
    memcpy(read_buf + bufferPtr, buffer + offset, read_amt);
    read_len = read_len - read_amt;
    bufferPtr += read_amt;

    // Advance to next block, updating diskID if necessary
    blockID++;

    if (blockID == JBOD_NUM_BLOCKS_PER_DISK) {
      blockID = 0;
      diskID++;
      if (read_len > 0) {
        jbod_operation(encode(JBOD_SEEK_TO_DISK, diskID, 0), NULL);

      }

    }

    offset = 0;
    
  }

  return retval;
}



int mdadm_write(uint32_t start_addr, uint32_t write_len, const uint8_t *write_buf) {
	int megabyte = 1048576;
	
	if (mounted == 0) {
		return -3;
	}
	
	if (write_perm == 0) {
		return -5;
	}

	if ((start_addr + write_len) > megabyte){
   	return -1;
  }

  if (write_len > 1024){
   	return -2;
  }

  if (start_addr < 0 || write_len < 0 || ((write_buf == NULL) && (write_len != 0))) {
   	return -4;
  }
	
	uint32_t diskID = start_addr / JBOD_DISK_SIZE;

  uint32_t blockID = (start_addr % JBOD_DISK_SIZE) / JBOD_BLOCK_SIZE;

  // Starting position of a block
  uint32_t offset = (start_addr % JBOD_DISK_SIZE) % JBOD_BLOCK_SIZE;

  // Temporary storage to put in the read_buf
  uint8_t buffer[JBOD_BLOCK_SIZE];
 	int retval = write_len;
 	int bufferPtr = 0;

 	// Writes 
 	while (write_len > 0){

   	// Seek to Disk
   	uint32_t seekToDisk = encode(JBOD_SEEK_TO_DISK, diskID, blockID);
   	jbod_operation(seekToDisk, NULL);
   	// Seek to Block
   	uint32_t seekToBlock = encode(JBOD_SEEK_TO_BLOCK, diskID, blockID);
   	jbod_operation(seekToBlock, NULL);

   	// Reads block and puts it in the temporary buffer
   	uint32_t readOp = encode(JBOD_READ_BLOCK, diskID, blockID);
   	jbod_operation(readOp, buffer);

   	// Adjusts write_amt as necessary with the offset.
   	uint32_t write_amt = JBOD_BLOCK_SIZE - offset;
   	if (write_amt > write_len){
     		write_amt = write_len;
      
   	}
      
    // Seek to Disk
    	
   	jbod_operation(seekToDisk, NULL);

   	// Seek to Block
    	
   	jbod_operation(seekToBlock, NULL);

   	// Copies data needed to be copied to buffer
      
   	memcpy(buffer + offset, write_buf + bufferPtr, write_amt);

    if (cache_insert(diskID, blockID, buffer) == -1){
        cache_update(diskID, blockID, buffer);
    }

   	write_len = write_len - write_amt;
   	bufferPtr += write_amt;

	  uint32_t writeOp = encode(JBOD_WRITE_BLOCK, 0, 0);
   	jbod_operation(writeOp, buffer);

   	// Checks if the next block disk will need to be written.
   	if (blockID == 255){
     		diskID++;
     		blockID = 0;
   	} else {
     		blockID = blockID + 1;
   	}
   	offset = 0;
    
 }

 return retval;


}