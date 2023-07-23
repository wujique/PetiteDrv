


#include "mcu.h"

/*
	移植某些组件时，缺失某些函数会编译失败。
	在此定义一些空函数。
	*/

void exit(int status)
{

}


int system(const char *command)
{

	return 0;
}



