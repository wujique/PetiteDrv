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

#if (SYS_USE_VFS == 1)
#include "log.h"
#include "board_sysconf.h"
#include "vfs.h"

#if (SYS_FS_FATFS == 1)
#include "ff.h"
#endif
/*
	本代码完成度10%
	
	vfs的目的是对上层屏蔽fs的具体实现。
	例如，读字库，有可能是fatfs管理的SD卡，也有可能是littlefs管理的SPI FLASH，甚至是没有fs的spi FLASH(用constfs接口)。
	伪流程：
	0.通过宏定义，决定支持什么文件系统，并将相关接口注册到vfs
	1.上电时，将一个FS注册到VFS：注册内容包括FS类型，FS盘符，FS硬盘信息， FS硬盘操作接口。
	2.vfs通过FS类型决定调用哪种文件系统的驱动接口，并将硬盘信息传入接口。
	3.文件系统是外部模块，无法保证重入，很可能需要在vfs层就管理重入问题。

							{
								spiffs文件系统接口
							}
							{
								littlefs文件系统接口
							}
							{
								fatfs文件系统接口
							}
			   ------驱动	
app-----VFS ----|
			   ------设备
							{
								mtd0 
								fatfs
								XXX,
								XXXX
							}	
							{	
								//u盘
								mtd1
								fatfs
								XXX,
								xxx
							}
							{
								mtd2
								littlefs
								//硬盘信息和硬盘接口，是提供给文件系统使用的，
								不同的文件系统对信息的要求会有些许差异
								spi flash
								spi flash接口
							}
							、、、
*/
//#define Test_VFS 

#ifdef Test_VFS
#define VfsDebug wjq_log
#else
#define VfsDebug(a,...)
#endif

/*
	操作不同文件系统, mount时需要的数据
*/
union _uFileSysPra
{
	#ifdef SYS_FS_FATFS
    FATFS fatfs;	//fatfs
    #endif
	//CONSTFS constfs;	//const fs
	u8 NA;
};
/*
	操作文件时不同的文件系统需要的数据结构有差异，
	用union 定义
*/
union _uFilePra
{
	#ifdef SYS_FS_FATFS
	FIL fatfd;//fatfs
	#endif
	u8 NA;
};
	
/*
	文件系统节点
*/
typedef struct _strVfsNode
{
	const VFSDIR *Mtd;
	union _uFileSysPra pra;
}FsNode;
/*
	列表，
*/
FsNode VfsNodeList[SYS_FS_NUM+1];



typedef struct _strFileNode
{
	union _uFilePra pra;
	FsNode *fsnode;//记录文件属于哪个文件系统
}FileNode;


/*
**Description:      将一个输入的字符串文件路径，切割成1级目录跟文件名，
                    二级目录放在文件名中
**Input parameters:  
**Output parameters: 
**                   
**Returned value:        
**                   
*/
static s32 vfs_get_dir_name(const u8 *lpPath, u8 *lpDir, u8 *lpName)
{
    int index;
    u8 dir[VFS_DIR_LEN];//目录，只判断第一层
    u8 name[VFS_NAME_LEN];//文件名
    u8 name_index;    
    u8 str_len;
    u8 flag = 0;

    index = 0;
    name_index = 0;
    memset(name, 0, sizeof(name));
    memset(dir, 0, sizeof(dir));
    
    while(index < strlen(lpPath))
    {
        
        if(*(lpPath+index) == '/'
            && flag == 0)//分隔符
        {
            str_len = strlen(name);
            if( str_len > VFS_DIR_LEN)
            {
                VfsDebug(LOG_DEBUG, "目录名太长\r\n");
                return -1;
            }
            else if(str_len > 0)
            {
                strcpy(dir, name);//只能保存一级路径
                name_index = 0;
                memset(name, 0, sizeof(name)); 
                flag = 1;
            }
            
        }
        else
        {
            name[name_index++] = *(lpPath+index);
            if(name_index >= VFS_NAME_LEN)
            {
                VfsDebug(LOG_DEBUG, "文件名太长\r\n");
                return -1;
                break;//文件名太长，错误，退出
            }
        }

        index++;
    }

    strcpy(lpDir, dir);
    strcpy(lpName, name);

    if(strlen(dir) == 0)
    {
        strcpy(lpDir,"mtd0");
    }
    
    return 0;
}

/*
**Description:      初始化虚拟文件系统列表     
**Input parameters:  
**Output parameters: 
**                   
**Returned value:        
**                   
*/
void vfs_init(void)
{
	u8 i = 0;
	while(1)
	{
		if(i>= SYS_FS_NUM)
			break;
		VfsNodeList[i].Mtd = NULL;
		i++;
	}
	
}
/*
	将一个已经初始化好的文件系统
	添加到vfs节点中
*/
int vfs_add_node(const VFSDIR *Mtd)
{
	u8 i = 0;
	
	VfsDebug(LOG_DEBUG, "mount %s 到 / \r\n", Mtd->vfsdir);

	while(1) {
		if (i>= SYS_FS_NUM) {
			VfsDebug(LOG_DEBUG, "mount err\r\n");
			break;
		}
		
		if (VfsNodeList[i].Mtd == NULL) {
			VfsNodeList[i].Mtd = Mtd;
			return 0;
		}
		
		i++;
	}
	return -1;
}

/*
**Description:      挂一个mtd到root链表上     
**Input parameters:  
**Output parameters: 
**                   
**Returned value:        
**                   
*/
int vfs_mount(const VFSDIR *Mtd)
{
	u8 i = 0;
	
    VfsDebug(LOG_DEBUG, "mount %s 到 / \r\n", Mtd->vfsdir);

    while(1)
	{
		if(i>= SYS_FS_NUM)
		{
			VfsDebug(LOG_DEBUG, "mount err\r\n");
			break;
		}
		
		if(VfsNodeList[i].Mtd == NULL)
		{
			VfsNodeList[i].Mtd = Mtd;

			/*根据 文件系统类型调用对应的初始化*/
			switch(VfsNodeList[i].Mtd->type)
			{
				case FS_TYPE_FATFS:
					VfsDebug(LOG_DEBUG, "mount FS_TYPE_FATFS\r\n");
					#ifdef SYS_FS_FATFS
					FRESULT res;

					/* Initialises the File System*/
					res = f_mount(&VfsNodeList[i].pra.fatfs, Mtd->dir, 1);
					if ( res == FR_OK ) 
					{
						wjq_log(LOG_FUN, "> vfs mount fatfs initialized.\r\n");
						return 0;
					}
					//vfs_explore_disk("1:/", 1);
					
					wjq_log(LOG_FUN, "> vfs mount fatfs err:%d\r\n", res);
					#endif
					return -1;
					
				case FS_TYPE_SPIFFS:
					VfsDebug(LOG_DEBUG, "mount FS_TYPE_SPIFFS\r\n");
					break;
				
				case FS_TYPE_LITTLEFS:
					VfsDebug(LOG_DEBUG, "mount FS_TYPE_LITTLEFS\r\n");
					break;
				
				case FS_TYPE_CONSTFS:
					VfsDebug(LOG_DEBUG, "mount FS_TYPE_CONSTFS\r\n");
					break;
				
				default:
					VfsDebug(LOG_DEBUG, "mount err: unkown fs\r\n");
					break;
			}

			break;
		}
		
		i++;
	}
	return -1;
}

/*
**Description:      查询文件系统，返回目录结构指针     
**Input parameters:  
**Output parameters: 
**                   
**Returned value:        
**                   
*/
static FsNode *vfs_find_dir(u8* dirname)
{
	u8 i = 0;
	
    VfsDebug(LOG_DEBUG, "find dir %s\r\n", dirname);

	while(1)
	{
		if(i>= SYS_FS_NUM)
		{
			VfsDebug(LOG_DEBUG, "scan dir err:no dir %s\r\n", dirname);
			return NULL;
		}
		
		if(VfsNodeList[i].Mtd != NULL)
		{
			if(0 == strcmp(VfsNodeList[i].Mtd->vfsdir, dirname))
        	{
            	VfsDebug(LOG_DEBUG, "找到对应目录 %s\r\n", dirname);
        	    return &VfsNodeList[i];
       		}	
		}
		i++;
	}
	
}

int vfs_creat(const char *filename, int mode)
{
	return -1;	
}
    

int vfs_open(const char *pathname, int oflags)
{
	u8 dir[VFS_DIR_LEN];
    u8 name[VFS_NAME_LEN];
	FsNode *fsnode;
	FileNode *filenode;
	
	vfs_get_dir_name(pathname, &dir[0], &name[0]);

	fsnode = vfs_find_dir(&dir[0]);
	if(fsnode == NULL) 	return NULL;

	filenode = (FileNode *)wjq_malloc(sizeof(FileNode));
	if(filenode == NULL) return NULL;
	filenode->fsnode = fsnode;
	
	switch(filenode->fsnode->Mtd->type)
	{
		case FS_TYPE_FATFS:
			VfsDebug(LOG_DEBUG, "open FS_TYPE_FATFS\r\n");
			#ifdef SYS_FS_FATFS
			FRESULT res;
			BYTE mode;
			/*need fix*/
			if(oflags == O_RDONLY)
				mode = FA_READ;
			else if(oflags == O_CREAT)
				mode = FA_CREATE_ALWAYS | FA_WRITE;
			
			res = f_open(&filenode->pra.fatfd, name, mode);
			if(res == FR_OK) {
				wjq_log(LOG_INFO, "open file :%s ok!\r\n", name);	
				wjq_log(LOG_INFO, "vfs_open fd=%08x\r\n", (int)filenode);
				return (int)filenode;
				
			}else
			#endif
			{
				wjq_log(LOG_INFO, "open file:%s, err\r\n", name);	
			}
			return NULL;

			
		case FS_TYPE_SPIFFS:
			VfsDebug(LOG_DEBUG, "open FS_TYPE_SPIFFS\r\n");
			break;
		
		case FS_TYPE_LITTLEFS:
			VfsDebug(LOG_DEBUG, "open FS_TYPE_LITTLEFS\r\n");
			break;
		
		case FS_TYPE_CONSTFS:
			VfsDebug(LOG_DEBUG, "open FS_TYPE_CONSTFS\r\n");
			break;
		
		default:
			VfsDebug(LOG_DEBUG, "open err: unkown fs\r\n");
			break;
	}
	return NULL;
}

int vfs_read(int fd, const void *buf, size_t length)
{
	FileNode *filenode;
	unsigned int len;
		
	if(fd == NULL) return -1;
	
	filenode = (FileNode *)fd;
	switch(filenode->fsnode->Mtd->type)
	{
		case FS_TYPE_FATFS:
			VfsDebug(LOG_DEBUG, "vfs_read FS_TYPE_FATFS\r\n");
			
			#ifdef SYS_FS_FATFS
			FRESULT res;
		
			res = f_read(&filenode->pra.fatfd, (void *)buf, length, &len);
			if((res != FR_OK) || (len != length))
			{
				VfsDebug(LOG_DEBUG, "read err\r\n");
				return -1;
			}

			return (int)len;
			#else
			return -1;
			#endif 

			
		case FS_TYPE_SPIFFS:
			VfsDebug(LOG_DEBUG, "vfs_read FS_TYPE_SPIFFS\r\n");
			break;
		
		case FS_TYPE_LITTLEFS:
			VfsDebug(LOG_DEBUG, "vfs_read FS_TYPE_LITTLEFS\r\n");
			break;
		
		case FS_TYPE_CONSTFS:
			VfsDebug(LOG_DEBUG, "vfs_read FS_TYPE_CONSTFS\r\n");
			break;
		
		default:
			VfsDebug(LOG_DEBUG, "vfs_read err: unkown fs\r\n");
			break;
	}
	return NULL;
}

int vfs_write(int fd, const void *buf, size_t length)
{
	FileNode *filenode;
	unsigned int len;
		
	if(fd == NULL) return -1;
	
	filenode = (FileNode *)fd;
	switch(filenode->fsnode->Mtd->type)
	{
		case FS_TYPE_FATFS:
			VfsDebug(LOG_DEBUG, "vfs_read FS_TYPE_FATFS\r\n");
			
		#ifdef SYS_FS_FATFS
			FRESULT res;
		
			res = f_write(&filenode->pra.fatfd, (void *)buf, length, &len);
			if((res != FR_OK) || (len != length))
			{
				VfsDebug(LOG_DEBUG, "write err\r\n");
				return -1;
			}

			return (int)len;
		#else
			return -1;
		#endif 

			
		case FS_TYPE_SPIFFS:
			VfsDebug(LOG_DEBUG, "vfs_read FS_TYPE_SPIFFS\r\n");
			break;
		
		case FS_TYPE_LITTLEFS:
			VfsDebug(LOG_DEBUG, "vfs_read FS_TYPE_LITTLEFS\r\n");
			break;
		
		case FS_TYPE_CONSTFS:
			VfsDebug(LOG_DEBUG, "vfs_read FS_TYPE_CONSTFS\r\n");
			break;
		
		default:
			VfsDebug(LOG_DEBUG, "vfs_read err: unkown fs\r\n");
			break;
	}
	return NULL;
}
/*
	whence: SEEK_SET 0 //将偏移量设置为距离文件offset个字节处
		   SEEK_CUR 1 //当前偏移量加offset,offset可为正或负
	       SEEK_END 2 //设置为文件长度加offset,offset可为正或负
	*/
int vfs_lseek(int fd, int offset, int whence)
{
	FileNode *filenode;
	unsigned int len;
	int file_len;
	
	if(fd == NULL) {
		VfsDebug(LOG_INFO, "vfs_lseek fd=NULL\r\n");
		return -1;
	}
	
	filenode = (FileNode *)fd;
	switch(filenode->fsnode->Mtd->type)
	{
		case FS_TYPE_FATFS:
			VfsDebug(LOG_INFO, "vfs_lseek FS_TYPE_FATFS\r\n");
			#ifdef SYS_FS_FATFS
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
			}
			#else 	
			return -1;
			#endif 
			break;
			
		case FS_TYPE_SPIFFS:
			VfsDebug(LOG_DEBUG, "vfs_lseek FS_TYPE_SPIFFS\r\n");
			break;
		
		case FS_TYPE_LITTLEFS:
			VfsDebug(LOG_DEBUG, "vfs_lseek FS_TYPE_LITTLEFS\r\n");
			break;
		
		case FS_TYPE_CONSTFS:
			VfsDebug(LOG_DEBUG, "vfs_lseek FS_TYPE_CONSTFS\r\n");
			break;
		
		default:
			wjq_log(LOG_INFO, "vfs_lseek err: unkown fs\r\n");
			break;
	}
	return NULL;
}

int vfs_close(int fd)
{
	FileNode *filenode;
	unsigned int len;
		
	if(fd == NULL)
		return -1;
	
	filenode = (FileNode *)fd;
	switch(filenode->fsnode->Mtd->type)
	{
		case FS_TYPE_FATFS:
			VfsDebug(LOG_DEBUG, "vfs_close FS_TYPE_FATFS\r\n");
		#ifdef SYS_FS_FATFS
			FRESULT res;
			res = f_close(&filenode->pra.fatfd);
			if(res != FR_OK)
			{
				return 0;
			}
		#else 	
			return -1;
		#endif 
			break;
			
		case FS_TYPE_SPIFFS:
			VfsDebug(LOG_DEBUG, "vfs_close FS_TYPE_SPIFFS\r\n");
			break;
		
		case FS_TYPE_LITTLEFS:
			VfsDebug(LOG_DEBUG, "vfs_close FS_TYPE_LITTLEFS\r\n");
			break;
		
		case FS_TYPE_CONSTFS:
			VfsDebug(LOG_DEBUG, "vfs_close FS_TYPE_CONSTFS\r\n");
			break;
		
		default:
			VfsDebug(LOG_DEBUG, "vfs_close err: unkown fs\r\n");
			break;
	}
	return NULL;
}

int vfs_tell(int fd)
{
	FileNode *filenode;
	unsigned int len;
		
	if(fd == NULL)
		return -1;

	filenode = (FileNode *)fd;
	switch(filenode->fsnode->Mtd->type)
	{
		case FS_TYPE_FATFS:
			VfsDebug(LOG_INFO, "vfs_tell FS_TYPE_FATFS\r\n");
		#ifdef SYS_FS_FATFS
			int res;
			res = f_tell(&filenode->pra.fatfd);

			return res;
		#else 	
			return 0;
		#endif 
			break;
			
		case FS_TYPE_SPIFFS:
			VfsDebug(LOG_DEBUG, "vfs_tell FS_TYPE_SPIFFS\r\n");
			break;
		
		case FS_TYPE_LITTLEFS:
			VfsDebug(LOG_DEBUG, "vfs_tell FS_TYPE_LITTLEFS\r\n");
			break;
		
		case FS_TYPE_CONSTFS:
			VfsDebug(LOG_DEBUG, "vfs_lseek FS_TYPE_CONSTFS\r\n");
			break;
		
		default:
			VfsDebug(LOG_DEBUG, "vfs_lseek err: unkown fs\r\n");
			break;
	}
	return NULL;
}


#endif

