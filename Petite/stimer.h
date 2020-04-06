/**
 * @file            
 * @brief           
 * @author          wujique
 * @date            2018年1月7日 星期日
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年1月7日 星期日
 *   作    者:         wujique
 *   修改内容:   创建文件
*/
#ifndef _STIMER_H_
#define _STIMER_H_

/*need fix */
#define SYSTEMTICK_PERIOD_MS  1


extern void Delay(uint32_t nCount);
extern uint32_t Stime_get_localtime(void);

#endif
