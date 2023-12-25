/**
 * @file            
 * @brief           
 * @author          wujique
 * @date            2019年3月2日 星期五
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        
 *   作    者:       屋脊雀工作室
 *   修改内容:   创建文件
 	1 源码归屋脊雀工作室所有。
	2 可以用于的其他商业用途（配套开发板销售除外），不须授权。
	3 屋脊雀工作室不对代码功能做任何保证，请使用者自行测试，后果自负。
	4 可随意修改源码并分发，但不可直接销售本代码获利，并且请保留WUJIQUE版权说明。
	5 如发现BUG或有优化，欢迎发布更新。请联系：code@wujique.com
	6 使用本源码则相当于认同本版权说明。
	7 如侵犯你的权利，请联系：code@wujique.com
	8 一切解释权归屋脊雀工作室所有。
*/
#include "mcu.h"
#include "petite_config.h"
#include "petite.h"
#include "partition.h"
#include "log.h"
#include "board_sysconf.h"


/*
	vfs的目的是对上层屏蔽fs的具体实现。
	例如，读字库，有可能是fatfs管理的SD卡，也有可能是littlefs管理的SPI FLASH，甚至是没有fs的spi FLASH(用constfs接口)。
	伪流程：
	0.通过宏定义，决定支持什么文件系统，并将相关接口注册到vfs
	1.上电时，将一个FS注册到VFS：注册内容包括FS类型，FS盘符，FS硬盘信息， FS硬盘操作接口。
	2.vfs通过FS类型决定调用哪种文件系统的驱动接口，并将硬盘信息传入接口。
	3.文件系统是外部模块，无法保证重入，很可能需要在vfs层就管理重入问题。
*/
#if (SYS_USE_VFS == 1)
#include "vfs.h"

//#define Test_VFS 

#ifdef Test_VFS
#define VfsDebug wjq_log
#else
#define VfsDebug(a,...)
#endif

#if (SYS_FS_FATFS == 1)
#include "ff.h"
#endif

#if (SYS_FS_LITTLEFS == 1)
#include "lfs.h"
/**
 * @brief   littlefs初始化需要的内容
 * 
 */
typedef struct _VfsLittlefs{
	struct lfs_config cfg;
	lfs_t lfs;
}VfsLittlefs;

#endif

/**
 * @brief   不同文件系统操作文件需要的变量
 * 
 * 
 */
union _uFilePra
{
	#if (SYS_FS_FATFS == 1)
	FIL fatfd;//fatfs
	#endif
	#if (SYS_FS_LITTLEFS == 1)
	lfs_file_t lfsfd;
	#endif
	u8 NA;
};
	
struct _strFileNode;
/**
 * @brief   虚拟文件系统节点
 * 
 * 
 */
typedef struct _VfsMountNode{
	/*不同文件系统的mount时需要的配置	
		例如littlefs就是 VfsLittlefs 结构体 */
	void *context;
	
	PartitionNode *part;///指向分区节点
		
	///不同文件系统的操作函数
	int (*open)(struct _strFileNode *file, const char *pathname, int oflags);
	int (*read)(struct _strFileNode *file, void *dst, size_t length);
	int (*write)(struct _strFileNode *file, void *src, size_t length);
	int (*lseek)(struct _strFileNode *file, int offset, int whence);
	int (*close)(struct _strFileNode *file);

	int (*tell)(struct _strFileNode *file);
}VfsMountNode;


/**
 * @brief   vfs文件节点
 * @details 打开一个文件则创建一个节点
 * 
 */
typedef struct _strFileNode
{
	/* 是否考虑加一个TAG，防止胡乱close*/
	//uint32_t tag;//0x7666736C 

	union _uFilePra pra;///不同文件系统的文件句柄	
	
	VfsMountNode *vfsnode;///指向文件所属文件系统
}FileNode;


#if (SYS_FS_FATFS == 1)
/**
 * @brief   
 * 
 * @param   file        VFS文件节点
 * @param   pathname    文件名
 * @param   oflags      打开方式
 * @return  int 
 * 
 * @note    目前fatfs对应的sdmmc和usb msc并没有归partiton管理
 *          因此pathname需要包含盘符，例如0:
 */
int vfs_fatfs_file_open(FileNode *file, const char *pathname, int oflags)
{
	FRESULT res;
	BYTE mode;
	
	if(oflags == O_RDONLY)
		mode = FA_OPEN_EXISTING | FA_READ;
	else if(oflags == O_CREAT)
		mode = FA_CREATE_ALWAYS | FA_WRITE;
	else {
		mode = FA_OPEN_EXISTING | FA_READ;	
	}

	uint8_t i = 0;
	if(*pathname == '/') i++;//把根目录去掉
		
	res = f_open(&(file->pra.fatfd), pathname + i, mode);
	if(res == FR_OK) {
		wjq_log(LOG_INFO, "fatfs open file :%s ok!\r\n", pathname);	
		return 0;
		
	} else {
		wjq_log(LOG_INFO, "fatfs open file:%s,err\r\n", pathname);
		return -1;
	}

}
/**
 * @brief   挂载fatfs到vfs
 * 
 * @param   vfs_node    参数描述
 * @return  int 
 * 
 * @note    目前仅仅将fatfs接口挂到vfs，初始化放在用户程序
 */
int vfs_fatfs_mount(VfsMountNode *vfs_node)
{

	#if 0
	FRESULT res;
	/* Initialises the File System 
		FATFS这个结构是全局，理应挂到partition上去 */
	FATFS *fs;
	fs = wjq_malloc(sizeof(FATFS));
	if (fs == NULL) {
		return -1;
	}
	
	res = f_mount(fs, "0:", 1);
	if ( res == FR_OK )  {
		wjq_log(LOG_DEBUG, "> vfs mount fatfs initialized.\r\n");
		return 0;
	}
	//vfs_explore_disk("1:/", 1);
	wjq_log(LOG_DEBUG, "> vfs mount fatfs err:%d\r\n", res);
	return -1;
	#endif
	/* fatfs底层操作没有挂入partition，因此
		vfs node 的contex为空，
		只需要把fatfs接口函数填入vfs node，以便vfs调用 */
	vfs_node->open = vfs_fatfs_file_open;
	
	return 0;
}
#endif

#if (SYS_FS_LITTLEFS == 1)

/**
 * @brief   littlefs读底层storage接口
 * 
 * @param   c           lfs配置
 * @param   block       块编号
 * @param   off         块内偏移
 * @param   buffer      数据指针
 * @param   size        数据长度
 * @return  int 
 * 
 */
int vfs_lfs_sto_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
	PartitionNode *part;
	int ret;
	
	part =  c->context;

	ret = petite_partition_read(part, block*c->block_size + off, buffer, size);

	return ret;
}


int vfs_lfs_sto_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
	PartitionNode *part;

	part =  c->context;

	int ret = petite_partition_write(part, block*c->block_size + off, buffer, size);

	return ret;
}

int vfs_lfs_sto_erase(const struct lfs_config *c, lfs_block_t block)
{
	PartitionNode *part;

	part =  c->context;

	int ret = petite_partition_erase(part, block*c->block_size , c->block_size);

	return ret;
}

int vfs_lfs_sto_sync(const struct lfs_config *c)
{
	return 0;
}

/*
	二次封装littlefs接口给vfs使用
	*/
int vfs_lfs_file_open(FileNode *file, const char *path, int oflags)
{
	lfs_t *lfs;
	lfs_file_t *lfsfile;
	VfsLittlefs *vlfs;
	VfsMountNode *vfs_node;
	
	int flags;
	int ret;
	
	if(oflags == O_RDONLY)
		flags = LFS_O_RDONLY;
	else if(oflags == O_CREAT)
		flags = LFS_O_CREAT | LFS_O_RDWR;
	else {
		flags = LFS_O_RDWR;	
	}

	/* vfs filenode->VfsMountNode->VfsLittlefs->lfs_t 
		从 <文件节点> 找到 <文件系统节点>，然后找到<littlefs配置>,
		从 <littlefs配置> 找到要的 <lfs和config>
		*/
	vfs_node = file->vfsnode;
	vlfs = (VfsLittlefs *)vfs_node->context;
	lfs = &vlfs->lfs;

	/* 打开文件得到的句柄，保存到文件节点pra联合体中 */
	lfsfile = &file->pra.lfsfd;
	
	ret = lfs_file_open(lfs, lfsfile, path, flags);
	if (ret != LFS_ERR_OK) {
		wjq_log(LOG_INFO, "lfs open file:%s,err\r\n", path);
		return -1;

	}
	wjq_log(LOG_INFO, "lfs open file:%s,ok\r\n", path);
	return 0;
}

int vfs_lfs_file_read(FileNode *file, void *dst, size_t length)
{
	lfs_t *lfs;
	lfs_file_t *lfsfile;
	VfsLittlefs *vlfs;

	vlfs = (VfsLittlefs *)file->vfsnode->context;
	lfs = &vlfs->lfs;
	lfsfile = &file->pra.lfsfd;
	
	return lfs_file_read(lfs, lfsfile, dst, length);
}
int vfs_lfs_file_write(FileNode *file, void *src, size_t length)
{
	lfs_t *lfs;
	lfs_file_t *lfsfile;
	VfsLittlefs *vlfs;

	vlfs = (VfsLittlefs *)file->vfsnode->context;
	lfs = &vlfs->lfs;
	lfsfile = &file->pra.lfsfd;
	
	return lfs_file_write(lfs, lfsfile, src, length);
}

int vfs_lfs_file_lseek(FileNode *file, int offset, int whence)
{
	lfs_t *lfs;
	lfs_file_t *lfsfile;
	VfsLittlefs *vlfs;

	vlfs = (VfsLittlefs *)file->vfsnode->context;
	lfs = &vlfs->lfs;
	lfsfile = &file->pra.lfsfd;
	
	return lfs_file_seek(lfs, lfsfile, offset, whence);
}

int vfs_lfs_file_close(FileNode *file)
{
	lfs_t *lfs;
	lfs_file_t *lfsfile;
	VfsLittlefs *vlfs;

	vlfs = (VfsLittlefs *)file->vfsnode->context;
	lfs = &vlfs->lfs;
	lfsfile = &file->pra.lfsfd;
	
	return lfs_file_close(lfs, lfsfile);
}


int vfs_lfs_file_tell(FileNode *file)
{
	lfs_t *lfs;
	lfs_file_t *lfsfile;
	VfsLittlefs *vlfs;

	vlfs = (VfsLittlefs *)file->vfsnode->context;
	lfs = &vlfs->lfs;
	lfsfile = &file->pra.lfsfd;
	
	return lfs_file_tell(lfs, lfsfile);
}

/**
 * @brief   挂载littlefs分区
 * 
 * @param   vfs_node    VFS节点
 * @return  int 
 * 
 * @note    littlefs的配置固定在此，后续要提取出来以便适应不同平台不同Flash
 * 
 */
int vfs_mount_lfs(VfsMountNode *vfs_node)
{
	VfsLittlefs *vlfs;

	vlfs = wjq_malloc(sizeof(VfsLittlefs));

	/* 把littlefs的数据挂到vfs node */
	vfs_node->context = vlfs;
	/* 把partition 挂到lfs配置中，以便littlefs调 patition 函数时找到分区 */
	vlfs->cfg.context = vfs_node->part;

	vlfs->cfg.read  = vfs_lfs_sto_read;
	vlfs->cfg.prog  = vfs_lfs_sto_prog;
	vlfs->cfg.erase = vfs_lfs_sto_erase;
	vlfs->cfg.sync  = vfs_lfs_sto_sync;

	// block device configuration
	vlfs->cfg.read_size = 16;
	/* flash通常一个page是256 */
	vlfs->cfg.prog_size = 16;
	
	vlfs->cfg.block_size = petite_partition_getblksize(vfs_node->part);
	vlfs->cfg.block_count = petite_partition_getlen(vfs_node->part)/vlfs->cfg.block_size;
	/* */
	vlfs->cfg.cache_size = 128;
	vlfs->cfg.lookahead_size = 16;
	/* 磨损均衡差值 */
	vlfs->cfg.block_cycles = 300;


	VfsDebug(LOG_INFO, "littlefs mount \r\n");
	int err = lfs_mount(&vlfs->lfs, &vlfs->cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
		VfsDebug(LOG_WAR, "littlefs format \r\n");
		
        lfs_format(&vlfs->lfs, &vlfs->cfg);
        lfs_mount(&vlfs->lfs, &vlfs->cfg);
    }

	vfs_node->open = vfs_lfs_file_open;
	vfs_node->read = vfs_lfs_file_read;
	vfs_node->write = vfs_lfs_file_write;
	vfs_node->lseek = vfs_lfs_file_lseek;
	vfs_node->close = vfs_lfs_file_close;
	vfs_node->tell = vfs_lfs_file_tell;
	return 0;

}

#endif


/**
 * @brief   将文件名切割成盘符和名字
 * 
 * @param   lpPath      参数描述
 * @param   lpDir       参数描述
 * @param   lpName      参数描述
 * @return  s32 
 * 
 */
static s32 vfs_get_dir_name(const u8 *lpPath, u8 *lpDir, u8 *lpName)
{
    int index;
    u8 name_index, dir_index;    
    u8 str_len, dir_len, name_len;
    u8 flag = 0;

    name_index = 0;
	name_len = 0;
	dir_index = 0;
	dir_len = 0;
	
	index = 0;
	while(index < strlen(lpPath)){
		if(*(lpPath+index) != '/') break;
		index++;
	}	
	dir_index = index;
	
	while(index < strlen(lpPath)){
		if(*(lpPath+index) == '/') break;
		index++;
	}	

	dir_len = index - dir_index;

	if(*(lpPath+index) == '/')index++;
	name_index = index;
	name_len = 	strlen(lpPath) - name_index;

	if (name_len == 0){
		name_index = dir_index;
		name_len = dir_len;
		dir_len = 0;
	}

    memcpy(lpDir, lpPath+dir_index, dir_len);
	lpDir[dir_len] = 0;
	
    memcpy(lpName, lpPath+name_index, name_len);
	lpName[name_len] = 0;
    
    return 0;
}

/**
 * @brief   在输入分区上挂载（初始化）VFS
 * 
 * @param   par         分区指针
 * @return  int 
 * 
 */
int vfs_mount(void *par)
{
	PartitionNode *part = par;
	VfsMountNode *vfs_node;
	
	vfs_node = wjq_malloc(sizeof(VfsMountNode));	
	part->context = vfs_node;///把虚拟文件系统节点挂到分区
	vfs_node->part = part;///把分区挂到虚拟文件节点，形成循环链表
	
	if (strcmp(part->def->type, VFS_STR_FATFS)==0){
		#if (SYS_FS_FATFS == 1)
		wjq_log(LOG_INFO, "mount FS_TYPE_FATFS:%s\r\n", part->def->name);
		return vfs_fatfs_mount(vfs_node);
		#else
		wjq_log(LOG_ERR, "unsuport fatfs\r\n");
		#endif
	} else if (strcmp(part->def->type, VFS_STR_LITTLEFS)==0){
		#if (SYS_FS_FATFS == 1)
		wjq_log(LOG_INFO, "mount VFS_STR_LITTLEFS:%s\r\n", part->def->name);
		return vfs_mount_lfs(vfs_node);
		#else
		wjq_log(LOG_ERR, "unsuport littlefs\r\n");
		#endif
	} else {
		wjq_log(LOG_INFO, "mount unknow:%s\r\n", part->def->name);
	}

	return -1;
}

int vfs_creat(const char *filename, int mode)
{
	return -1;	
}
    
/**
 * @brief   打开文件
 * 
 * @param   pathname    参数描述
 * @param   oflags      参数描述
 * @return  int 
 * 
 */
int vfs_open(const char *pathname, int oflags)
{
	u8 dir[VFS_DIR_LEN];
    u8 name[VFS_NAME_LEN];
	FileNode *filenode;

	PartitionNode *part;
	VfsMountNode *vfs_node;
	int ret;
		
	/* 从pathname中切割盘符（分区名）*/
	vfs_get_dir_name(pathname, &dir[0], &name[0]);
	uart_printf("dir:%s, name:%s\r\n", dir, name);
	/* 查找分区 ，并从分区获取文件系统操作接口 */

	part = petite_partition_get(dir);
	if (part == NULL ) {
		wjq_log(LOG_WAR, "file open err, no partition:%s\r\n", dir);
		return -1;
	}

	vfs_node = part->context;
	
	/* 申请一个节点，保存打开文件的信息 */
	filenode = (FileNode *)wjq_malloc(sizeof(FileNode));
	if(filenode == NULL) return NULL;
	filenode->vfsnode = vfs_node;//记录文件所属文件系统节点
	if (strcmp(part->def->type, VFS_STR_FATFS)==0){
		ret = vfs_node->open(filenode, pathname, oflags);
	} else {
		ret = vfs_node->open(filenode, name, oflags);
	}
	
	if(ret == 0) return (int)filenode;

	wjq_free(filenode);
	return NULL;
}
/**
 * @brief   读文件
 * 
 * @param   fd          参数描述
 * @param   buf         参数描述
 * @param   length      参数描述
 * @return  int 
 * 
 */
int vfs_read(int fd, void *buf, size_t length)
{
	unsigned int len;
	
	if(fd == NULL) return -1;

	FileNode *filenode;
	PartitionNode *part;
	VfsMountNode *vfs_node;

	/* file -> vfs -> partition */
	filenode = (FileNode *)fd;
	vfs_node = filenode->vfsnode;
	part = vfs_node->part;
	

	if (strcmp(part->def->type, VFS_STR_FATFS)==0){
		VfsDebug(LOG_DEBUG, "vfs_read FS_TYPE_FATFS\r\n");
#if (SYS_FS_FATFS == 1)		
		FRESULT res;
	
		res = f_read(&filenode->pra.fatfd, (void *)buf, length, &len);
		if (res != FR_OK) {
			VfsDebug(LOG_DEBUG, "read err\r\n");
			return -1;
		}

		if (len != length) {
			VfsDebug(LOG_DEBUG, "fs end\r\n");
		}

		return (int)len;
#endif
	}else {
		return vfs_node->read(filenode, buf, length);
	}

	return -2;
}
/**
 * @brief   描述
 * 
 * @param   fd          参数描述
 * @param   buf         参数描述
 * @param   length      参数描述
 * @return  int 
 * 
 */
int vfs_write(int fd, void *buf, size_t length)
{
	unsigned int len;
		
	if(fd == NULL) return -1;
	
	FileNode *filenode;
	PartitionNode *part;
	VfsMountNode *vfs_node;

	/* file -> vfs -> partition */
	filenode = (FileNode *)fd;
	vfs_node = filenode->vfsnode;
	part = vfs_node->part;
	

	if (strcmp(part->def->type, VFS_STR_FATFS)==0){

		VfsDebug(LOG_DEBUG, "vfs_read FS_TYPE_FATFS\r\n");
#if (SYS_FS_FATFS == 1)

		FRESULT res;
	
		res = f_write(&filenode->pra.fatfd, (void *)buf, length, &len);
		if((res != FR_OK) || (len != length)) {
			VfsDebug(LOG_DEBUG, "write err\r\n");
			return -1;
		}

		return (int)len;
#endif
	}else
		return vfs_node->write(filenode, buf, length);

	return -2;
}

/**
 * @brief   描述
 * 
 * @param   fd          参数描述
 * @param   offset      参数描述
 * @param   whence      参数描述
 * @ref		SEEK_SET 0 //将偏移量设置为距离文件offset个字节处
		    SEEK_CUR 1 //当前偏移量加offset,offset可为正或负
	        SEEK_END 2 //设置为文件长度加offset,offset可为正或负	
 * @return  * int 
 * 
 */
int vfs_lseek(int fd, int offset, int whence)
{
	unsigned int len;
	int file_len;
	
	if(fd == NULL) {
		VfsDebug(LOG_INFO, "vfs_lseek fd=NULL\r\n");
		return -1;
	}
	
	FileNode *filenode;
	PartitionNode *part;
	VfsMountNode *vfs_node;

	/* file -> vfs -> partition */
	filenode = (FileNode *)fd;
	vfs_node = filenode->vfsnode;
	part = vfs_node->part;
	

	if (strcmp(part->def->type, VFS_STR_FATFS)==0){

		VfsDebug(LOG_INFO, "vfs_lseek FS_TYPE_FATFS\r\n");
#if (SYS_FS_FATFS == 1)

		FRESULT res;
		if(whence == SEEK_SET){
			res = f_lseek(&filenode->pra.fatfd, offset);
		} else if(whence == SEEK_CUR) {
			file_len = f_tell(&filenode->pra.fatfd);
			res = f_lseek(&filenode->pra.fatfd, offset+file_len);
		} else if(whence == SEEK_END) {
			file_len = f_size(&filenode->pra.fatfd);
			res = f_lseek(&filenode->pra.fatfd, offset+file_len);
		}

		if(res != FR_OK) {
			return -1;
		} else{
			return res;
		}
#endif
	}else{		

		return vfs_node->lseek(filenode, offset, whence);
	}

	return -2;
}
/**
 * @brief   描述
 * 
 * @param   fd          参数描述
 * @return  int 
 * 
 */
int vfs_close(int fd)
{
	unsigned int len;
		
	if(fd == NULL) return -1;
	
	FileNode *filenode;
	PartitionNode *part;
	VfsMountNode *vfs_node;

	/* file -> vfs -> partition */
	filenode = (FileNode *)fd;
	vfs_node = filenode->vfsnode;
	part = vfs_node->part;

	if (strcmp(part->def->type, VFS_STR_FATFS)==0){
		VfsDebug(LOG_DEBUG, "vfs_close FS_TYPE_FATFS\r\n");
#if (SYS_FS_FATFS == 1)	

		FRESULT res;
		res = f_close(&filenode->pra.fatfd);

#endif

	}else {
		vfs_node->close(filenode);
	}

	wjq_free(filenode);

	return NULL;
}
/**
 * @brief   描述
 * 
 * @param   fd          参数描述
 * @return  int 
 * 
 */
int vfs_tell(int fd)
{

	unsigned int len;
		
	if(fd == NULL) 	return -1;

	FileNode *filenode;
	PartitionNode *part;
	VfsMountNode *vfs_node;

	/* file -> vfs -> partition */
	filenode = (FileNode *)fd;
	vfs_node = filenode->vfsnode;
	part = vfs_node->part;
	
	if (strcmp(part->def->type, VFS_STR_FATFS)==0){
		VfsDebug(LOG_INFO, "vfs_tell FS_TYPE_FATFS\r\n");
#if (SYS_FS_FATFS == 1)	

		int res;
		res = f_tell(&filenode->pra.fatfd);
		return res;
#endif 

	}else {
		return vfs_node->tell(filenode);
	}

	return NULL;
}


int vfs_file_copy(char *srcname, char *dstname)
{
	int srcfilefd, dstfilefd;
	int res;
	
	srcfilefd = vfs_open(srcname, O_RDONLY);
	if(srcfilefd <= 0)  {
		return -1;
	}

	dstfilefd = vfs_open(dstname, O_CREAT);
	if(dstfilefd <= 0)  {
		vfs_close(srcfilefd);
		return -1;
	}

	uint8_t *tmpbuf;

	tmpbuf = wjq_malloc(4096);
	
	while(1){
		res = vfs_read(srcfilefd, tmpbuf, 4096);
		if (res <= 0) break;
		
		vfs_write(dstfilefd, tmpbuf, 4096);
		wjq_log(LOG_INFO, "*");
	}
	wjq_log(LOG_INFO, "\r\ncppy finish!\r\n");

	wjq_free(tmpbuf);
	
	vfs_close(srcfilefd);
	vfs_close(dstfilefd);
	
}


#if 1
ft_file_struct *vfs_node_open(const char *pathname, char* oflags)
{
	//Uprintf("vfs_node_open:%s\r\n", pathname);
	int tmp;
	tmp = vfs_open(pathname, O_RDONLY);
	return (ft_file_struct *)tmp;
}

int vfs_node_read(void *buf, unsigned int size,unsigned int nmemb, ft_file_struct * fd)
{
	
	return vfs_read((int)fd, buf, size*nmemb);	
}

int vfs_node_write(ft_file_struct * fd, const void *buf, size_t length)
{
}

int vfs_node_lseek(ft_file_struct * fd, int offset, int whence)
{
	return vfs_lseek((int) fd, offset, whence);
}

int vfs_node_close(ft_file_struct * fd)
{
	return vfs_close((int) fd);
}

int vfs_node_tell(ft_file_struct * fd)
{
	return vfs_tell((int) fd);
}
#endif

#endif

