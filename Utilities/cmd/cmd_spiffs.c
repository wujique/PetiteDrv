
#include <command.h>
#include "console.h"

#include <spiffs.h>

int cmd_spiffs_ls( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	cmd_printf("do cmd spiffs ls\r\n");
	return 0;
}


int cmd_spiffs_format( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	cmd_printf("do cmd spiffs format!\r\n");
	return 0;
}



