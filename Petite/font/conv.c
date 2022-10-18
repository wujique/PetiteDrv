
#include "mcu.h"
#include "log.h"

//#include "gb2312.h"
//#include "gb18030.h"

#include "mem/alloc.h"

extern const unsigned short int gb2312_lookup[];
extern const unsigned short int gb18030_twobyte_lookup[];
extern const unsigned short int gb18030_fourbyte_lookup[];
/*
* 将UTF8编码转换成Unicode（UCS-2LE）编码  低地址存低位字节
* 参数：
*    char* pInput     输入字符串
*    char*pOutput   输出字符串
* 返回值：转换后的Unicode字符串的字节数，如果出错则返回-1


警告警告警告警告警告警告警告
	本函数对ASC码不进行补0处理
	比如 a UTF8是0x61, unicode 应该是0x0061，但是转到gbk又回到0x61
	    所以，不补0

*/
#if 0
int Utf8ToUnicode(char* pInput, char* pOutput)
{
	int outputSize = 0; //记录转换后的Unicode字符串的字节数
	char high;
	char middle;
	char low ;
			
	while (*pInput)
	{
		if (*pInput > 0x00 && *pInput <= 0x7F) //处理单字节UTF8字符（英文字母、数字）
		{
			*pOutput = *pInput;
			/*  警告 */
			 //pOutput++;
			//*pOutput = 0; //小端法表示，在高地址填补0
		}
		else if (((*pInput) & 0xE0) == 0xC0) //处理双字节UTF8字符
		{
			high = *pInput;
			pInput++;
			low = *pInput;
			
			if ((low & 0xC0) != 0x80)  //检查是否为合法的UTF8字符表示
			{
				return -1; //如果不是则报错
			}
 
			*pOutput = (high >> 2) & 0x07;
			pOutput++;
			*pOutput = (high << 6) + (low & 0x3F);
			
		} else if (((*pInput) & 0xF0) == 0xE0) {//处理三字节UTF8字符
			high = *pInput;
			pInput++;
			middle = *pInput;
			pInput++;
			low = *pInput;
			
			if (((middle & 0xC0) != 0x80) || ((low & 0xC0) != 0x80)) {
				return -1;
			}
			
			*pOutput = (high << 4) + ((middle >> 2) & 0x0F); //取出high的低四位与middle的中间四位，组合成unicode字符的高8位
			pOutput++;
			*pOutput = (middle << 6) + (low & 0x3F);//取出middle的低两位与low的低6位，组合成unicode字符的低8位
			
		} else //对于其他字节数的UTF8字符不进行处理
		{
			return -1;
		}
		pInput ++;//处理下一个utf8字符
		pOutput ++;
		outputSize += 2;
	}
	//unicode字符串后面，有两个\0
	*pOutput = 0;
	 pOutput++;
	*pOutput = 0;
	return outputSize;
}
#endif

/*
	source utf8数据
	vals  转换后的utf16数据
	length 输入长度

	返回：小于0 失败
		  大于=0，转换后数据长度
*/
int utf8_2_utf16(const unsigned char source[], unsigned char vals[], unsigned int length) 
{
    unsigned int bpos;
    int jpos;
    int next;
	int utf16;
    bpos = 0;
    jpos = 0;
    next = 0;

    do {
        if (source[bpos] <= 0x7f) {
            /* 1 byte mode (7-bit ASCII) */
			vals[jpos] = source[bpos];
            jpos++;
				
			vals[jpos] = 0;
			jpos++;
				
            next = bpos + 1;
            
        } else {
			
            if ((source[bpos] >= 0x80) && (source[bpos] <= 0xbf)) {
                return -1;
            }
			
            if ((source[bpos] >= 0xc0) && (source[bpos] <= 0xc1)) {
                return -1;
            }

            if ((source[bpos] >= 0xc2) && (source[bpos] <= 0xdf)) {
                /* 2 byte mode */
                utf16 = ((source[bpos] & 0x1f) << 6) + (source[bpos + 1] & 0x3f);
				
				vals[jpos] = utf16&0xff;
                jpos++;
				
				vals[jpos] = utf16>>8;
				jpos++;
			
				next = bpos + 2;
            } else {
                if ((source[bpos] >= 0xe0) && (source[bpos] <= 0xef)) {
                /* 3 byte mode */
                utf16 = ((source[bpos] & 0x0f) << 12) + ((source[bpos + 1] & 0x3f) << 6) + (source[bpos + 2] & 0x3f);

				vals[jpos] = utf16&0xff;
                jpos++;

				vals[jpos] = utf16>>8;
				jpos++;
			
				next = bpos + 3;

            	} else {
                	if (source[bpos] >= 0xf0) {
                	return -1;
                	}
            	}
            }
        }

        bpos = next;

    } while (bpos < length);

    return jpos;
}




/* Convert Unicode input to GB-18030 

	1 批量装换，需要申请内存，因为ASC码从UTF8转到UTF16是1个字节变两个字节。
	2 考虑提供1次转换1个？
*/
int utf8_2_gbk(char *udata, char *gdata, unsigned int length)
{
    int i, j;
    int posn;
    int error_number;
	int done;
	int len;
	unsigned short int *punicode;
	unsigned short int gbkcode;

	char *Utf16Buf;

	Utf16Buf = wjq_malloc(length*2);
	
	/* 转为UTF16 */
	len = utf8_2_utf16(udata, Utf16Buf, length);
    if (len < 0) {
		wjq_free(Utf16Buf);
        return -1;
    }
	
    posn = 0;
    for (i = 0; i < len; ) {
		
        done = 0;
        /* Single byte characters in range U+0000 -> U+007F */
		/* 取1个UTF数据，返回1个GBK数据*/
		punicode = (unsigned short int *)&Utf16Buf[i];
		i += 2;
		//uart_printf("<%04x> ", *punicode);
	
        if (*punicode <= 0x7f) {
			//Uprintf("asc ");
            gdata[posn] = *punicode&0xff;
            posn++;
            done = 1;
        }

        /* Two bytes characters in GB-2312 */
        if (done == 0) {
            j = 0;
            do {
                if (gb2312_lookup[j * 2] == *punicode) {
					gbkcode = gb2312_lookup[(j * 2) + 1];
                    gdata[posn] = gbkcode>>8;
                    posn++;
					
					gdata[posn] = gbkcode&0xff;
                    posn++;
				
                    done = 1;
					//Uprintf("2312 ");
                }
                j++;
            } while ((j < 7445) && (done == 0));
        }

        /* Two byte characters in GB-18030 */
        if (done == 0) {
            j = 0;
            do {
                if (gb18030_twobyte_lookup[j * 2] == *punicode) {
                    gbkcode =  gb18030_twobyte_lookup[(j * 2) + 1];
					
                    gdata[posn] = gbkcode>>8;
                    posn++;
					
					gdata[posn] = gbkcode&0xff;
                    posn++;
				
                    done = 1;
					//Uprintf("18030 ");
                }
                j++;
            } while ((j < 16495) && (done == 0));
        }

        /* Four byte characters in range U+0080 -> U+FFFF */
        if (done == 0) {
            j = 0;
			
            do {
				
                if (gb18030_fourbyte_lookup[j * 3] == *punicode) {
                    gbkcode = gb18030_fourbyte_lookup[(j * 3) + 1];
					gdata[posn] = gbkcode>>8;
                    posn++;
					
					gdata[posn] = gbkcode&0xff;
                    posn++;
					
                    gbkcode = gb18030_fourbyte_lookup[(j * 3) + 2];
                    gdata[posn] = gbkcode>>8;
                    posn++;
					
					gdata[posn] = gbkcode&0xff;
                    posn++;
					
                    done = 1;

					//Uprintf("Four byte ");
                }
                j++;
            } while ((j < 6793) && (done == 0));
        }

        /* Supplementary planes U+10000 -> U+1FFFF 
			未测试 need fix
		*/
        if (done == 0) {
            if (*punicode >= 0x10000 && *punicode < 0x110000) {
                /* algorithm from libiconv-1.15\lib\gb18030.h */
                int j, r3, r2, r1, r0;

                j = *punicode - 0x10000;
                r3 = (j % 10) + 0x30; j = j / 10;
                r2 = (j % 126) + 0x81; j = j / 126;
                r1 = (j % 10) + 0x30; j = j / 10;
                r0 = j + 0x90;
                gdata[posn] = (r0 << 8) + r1;
                gdata[posn + 1] = (r2 << 8) + r3;
                posn += 2;
                done = 1;
            }
        }

        /* Character not found */
        if (done == 0) {
			wjq_free(Utf16Buf);
			Uprintf("conv err!\r\n");
            return -1;
        }
    }
	
	wjq_free(Utf16Buf);

	return posn;
}




