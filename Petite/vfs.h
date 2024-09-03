
#ifndef _FS_VS_H_
#define _FS_VS_H_

#define SEEK_SET 0 //将偏移量设置为距离文件offset个字节处
#define SEEK_CUR 1 //当前偏移量加offset,offset可为正或负
#define SEEK_END 2 //设置为文件长度加offset,offset可为正或负

#define VFS_DIR_LEN  8//目录长度，一般都是mtd0 mtd1
#define VFS_NAME_LEN 32//文件名长度

#define VFS_STR_FATFS 		"fatfs"
#define VFS_STR_LITTLEFS 	"littlefs"
#define VFS_STR_CONSTFS 	"constfs"

typedef enum 
{
	FS_TYPE_FATFS = 1,
	FS_TYPE_SPIFFS,
	FS_TYPE_LITTLEFS,
	FS_TYPE_CONSTFS
}FS_TYPE;

typedef struct _strMtdPra
{
    u8 vfsdir[8];     //盘符名
    FS_TYPE type;       //文件系统类型
    u8 dir[8];
}VFSDIR;

#define O_RDONLY 	1// Open a file as read only
#define O_WRONLY	2// Open a file as write only
#define O_RDWR		3// Open a file as read and write
#define O_CREAT		0x0100// Create a file if it does not exist
#define O_EXTC		0x0200// Fail if a file already exists
#define O_TRUNC		0x0400// Truncate the existing file to zero size
#define O_APPEND	0x0800// Move to end of file on every write

extern void vfs_init(void);
extern int vfs_mount(void *part);
extern int vfs_creat(const char *filename, int mode);
extern int vfs_open(const char *pathname, int oflags);
extern int vfs_read(int fd,  void *buf, size_t length);
extern int vfs_write(int fd, void *buf, size_t length);
extern int vfs_lseek(int fd, int offset, int whence);
extern int vfs_close(int fd);


typedef struct
{
	int na;
}ft_file_struct;



#endif

