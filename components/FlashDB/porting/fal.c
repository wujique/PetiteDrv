
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#include "partition.h"

/**
 * FAL (Flash Abstraction Layer) initialization.
 * It will initialize all flash device and all flash partition.
 *
 * @return >= 0: partitions total number
 */
int fal_init(void)
{
    return 0;
}
/*
	查找分区
	*/
void *fal_partition_find(const char *name)
{
	return petite_partition_get(name);
}

uint32_t fal_partition_getlen(void *part)
{

	return petite_partition_getlen(part);
}
/*
	返回Flash的blk size
	暂时这样，理应根据Flash参数返回。
	*/
uint32_t fal_partition_getblksize(void *part)
{
	return petite_partition_getblksize(part);;
}

int fal_partition_read(void *part, uint32_t addr, uint8_t *buf, size_t size)
{
	return petite_partition_read(part, addr, buf, size);
}

int fal_partition_erase(void *part, uint32_t addr, size_t size)
{
	return petite_partition_erase(part, addr, size);
}

int fal_partition_write(void *part, uint32_t addr, const uint8_t *buf, size_t size)
{
	return petite_partition_write(part, addr, buf,size);
}

