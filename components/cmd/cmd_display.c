/* */
#include <command.h>
#include "console.h"



int cmd_display( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	
	
	/* argv 包含 命令
		比如 display clear i2c_oled
			argc = 3，
			argv 有3个字符串指针，
				字符串： display ,clear, i2c_oled
		*/
	#if 0	
	int cnt = 0;
	cmd_printf("argc:%d\r\n", argc);
	for(cnt = 0; cnt < argc; cnt++)
		cmd_printf("argv:%s\r\n", argv[cnt]);
	#endif
	if(strcmp(argv[1], "?") == 0 || strcmp(argv[1], "help") == 0) {
		cmd_printf("display usage:\r\n");	
		cmd_printf("\t display clear [dev]\r\n");
		cmd_printf("\t display showstr [dev] [str] [x] [y]\r\n");
	}
	
	return 0;
}






