/**
 * @file            
 * @brief           数学函数
 * @author          wujique
 * @date            2018年06月26日 星期二
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:      
 *   作    者:         wujique
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
#include "log.h"

/*

非递归判断一个数是2的多少次方

*/
int math_log2(int value) 
{ 
  int x=0; 
  while(value>1) 
  { 
	value>>=1; 
	x++; 
  } 
  return x; 
} 

/*
	对输入的U16 进行希尔排序
*/
void ShellSort(u16 *pSrc, s32 Len)
{
    u32 d;//增量
    s32 j, k;

    u16 temp;

    //uart_printf("shell:%d\r\n", Len);

    for(d = Len/2; d >0; d=d/2)
    {
        for(j = d; j < Len; j++)
        {
            temp = *(pSrc + j);

            for(k = j - d; (k >=0 && temp <(*(pSrc + k))); k -=d)
            {
                *(pSrc + k + d) = *(pSrc + k);
            }

            *(pSrc + k + d) = temp;
        }
    }
}

/*
	函数名称: 
	函数功能: 二分法查找u16
	入口参数: 
	返 回 值: 返回-1没找到，其他为索引值
	待查数据是降序
*/
int BinarySearch(u16 *pArray, u16 data, u16 len)
{
    s16 s, e, mid;
	
    s = 0;
    e = len - 1;
	wjq_log(LOG_INFO,"find: %d---", data);
	
    while(s <= e)
    {

        mid = (s + e) / 2;

        if (*(pArray + mid) > data)
        {
            s = mid + 1;
        }
        else if (*(pArray + mid) < data)
        {
            e = mid - 1;
        }
        else 
        {
        	wjq_log(LOG_INFO,"yes %d %d %d\r\n", s,e,mid);
            return mid;
        }
    }
	
	mid = s;
	s = e;
	e = mid;
	
	wjq_log(LOG_INFO,"no %d %d\r\n", s,e);
    return -1;
}
/*
	待查数据是升序
*/
int BinarySearchD(u16 *pArray, u16 data, u16 len)
{
    s16 s, e, mid;
	
    
	wjq_log(LOG_INFO,"find: %d---", data);

	s = 0;
    e = len - 1;
	
    while(1)
    {

        mid = (s + e) / 2;

        if (*(pArray + mid) < data)
        {
            s = mid;
        }
        else if (*(pArray + mid) > data)
        {
            e = mid;
        }
        else 
        {
        	wjq_log(LOG_INFO,"yes %d %d %d\r\n", s,e,mid);
            return mid;
        }
		
		if(s == e)
			break;
    }
	
	wjq_log(LOG_INFO,"no %d %d\r\n", s,e);
    return -1;
}

#if 0
//const u16 test1[10] = {100, 90, 80, 70, 60, 50, 40, 30, 20, 10};
const u16 test1[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};


const u16 test2[30] = {101, 100,99, 
						91, 90, 89, 
						81, 80, 79,
						71, 70, 69, 
						61, 60, 59,
						51, 50, 49, 
						41, 40, 39, 
						31, 30, 29, 
						21, 20, 19,
						11, 10, 9};

void test_bs(void)
{
	u16 i=0;

	while(i<30)
	{
		BinarySearchD(test1, test2[i], 10);	
		i++;
	}

	while(1);
}

#endif

/*
	函数名称: 
	函数功能: asc转U32
	入口参数: 
	返 回 值: 返回-1没找到，其他为索引值
*/
u32 ascii_2_u32(u8 *pb,u8 asciidatalen)
{
    u32 i;
    u32 d;
    u8 tmpvalue;

    d=0;
    for(i=0; i<asciidatalen&&i<8; i++)
    {
        if((*(pb+i))>='a'&&(*(pb+i))<='f')
        {
            tmpvalue = 0x0a + (*(pb+i)) - 'a';
        }
        else if((*(pb+i))>='A'&&(*(pb+i))<='F')
        {
            tmpvalue = 0x0a + (*(pb+i)) - 'A';
        }
        else if((*(pb+i))>='0'&&(*(pb+i))<='9')
        {
            tmpvalue = (*(pb+i));
        }
        else
        {
            return 0;
        }

        d = d<<4;
        d += (tmpvalue & 0x0f);
    }

    return d;
}

/*
	函数名称: 
	函数功能: 在指针  pData 开始位置查找一个[...]字符串，并把中括号之间
                    的内容拷贝到seg中，len说明seg buf的长度，找到后返回字符串长度
                    同时返回已经查询的长度index
	入口参数: 
	返 回 值: 0 找到，-1 没找到
*/
s32 vfs_find_seg(const u8 *pData, u32 DataLen,u8 *seg, u32 *len ,u32 *ScanLen)
{
    u32 index = 0;
    u32 seg_buf_len = *len;
    u32 str_len = 0;
    s32 res = -1;

    while(index < DataLen)
    {
        if(*(pData+index) == '[')
            break;
        index++;
    }

    index++;

    while((index < DataLen) && (str_len < seg_buf_len))
    {
        if(*(pData+index) == ']')
        {
            res = 0;
            break;
        }

        *(seg+str_len) = *(pData+index);
        index++;
        str_len++;
    }

    if(res == 0)
        *len = str_len;
    else
        *len = 0;

    *ScanLen = index;
    return res;
}


