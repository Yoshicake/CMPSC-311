#ifndef MDADM_H_
#define MDADM_H_

#include <stdint.h>
#include "jbod.h"
#include "cache.h"

/* Return 1 on success and -1 on failure */
int mdadm_mount(void);

/* Return 1 on success and -1 on failure */
int mdadm_unmount(void);

int mdadm_write_permission(void);

uint32_t encode(uint32_t command, uint32_t diskID, uint32_t blockID);

int mdadm_revoke_write_permission(void);


/* Return the number of bytes read on success, -1 on failure. */
int mdadm_read(uint32_t addr, uint32_t len, uint8_t *buf);

/* Return the number of bytes written on success, -1 on failure. */
int mdadm_write(uint32_t addr, uint32_t len, const uint8_t *buf);

#endif
