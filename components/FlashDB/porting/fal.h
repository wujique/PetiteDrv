
#ifndef _FAL_H_
#define _FAL_H_

#if 1
/**
 * FAL (Flash Abstraction Layer) initialization.
 * It will initialize all flash device and all flash partition.
 *
 * @return >= 0: partitions total number
 */
int fal_init(void);

/* =============== flash device operator API =============== */
/**
 * find flash device by name
 *
 * @param name flash device name
 *
 * @return != NULL: flash device
 *            NULL: not found
 */
void *fal_flash_device_find(const char *name);

uint32_t fal_partition_getlen(void *part);
uint32_t fal_partition_getblksize(void *part);

/* =============== partition operator API =============== */
/**
 * find the partition by name
 *
 * @param name partition name
 *
 * @return != NULL: partition
 *            NULL: not found
 */
void *fal_partition_find(const char *name);

/**
 * get the partition table
 *
 * @param len return the partition table length
 *
 * @return partition table
 */
void *fal_get_partition_table(size_t *len);

/**
 * set partition table temporarily
 * This setting will modify the partition table temporarily, the setting will be lost after restart.
 *
 * @param table partition table
 * @param len partition table length
 */
void fal_set_partition_table_temp(void *table, size_t len);

/**
 * read data from partition
 *
 * @param part partition
 * @param addr relative address for partition
 * @param buf read buffer
 * @param size read size
 *
 * @return >= 0: successful read data size
 *           -1: error
 */
int fal_partition_read(void *part, uint32_t addr, uint8_t *buf, size_t size);

/**
 * write data to partition
 *
 * @param part partition
 * @param addr relative address for partition
 * @param buf write buffer
 * @param size write size
 *
 * @return >= 0: successful write data size
 *           -1: error
 */
int fal_partition_write(void *part, uint32_t addr, const uint8_t *buf, size_t size);

/**
 * erase partition data
 *
 * @param part partition
 * @param addr relative address for partition
 * @param size erase size
 *
 * @return >= 0: successful erased data size
 *           -1: error
 */
int fal_partition_erase(void *part, uint32_t addr, size_t size);
#endif


#endif /* _FAL_H_ */
