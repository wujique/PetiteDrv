/**
 * @file            soundplay.c
 * @brief           声音播放功能
 * @author          wujique
 * @date            2018年1月6日 星期六
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年1月6日 星期六
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
/*

	本函数功能是，提供接口，播放指定路径下的声音文件，通过指定的设备输出

	备注：
		1 暂时只做16BIT WAV文件播放
		2 单声道通过数据填充做成双声道

*/
#include "mcu.h"
#include "petite_config.h"
#include "petite.h"

#if (PANEL_SOUND_MODULE == 1)

#include "log.h"
#include "vfs.h"
#include "audio_pipeline.h"
#include "soundplay.h"

#define FUN_SOUND_DEBUG

#ifdef FUN_SOUND_DEBUG
#define SOUND_DEBUG	wjq_log 
#else
#define SOUND_DEBUG(a, ...)
#endif

/*wav 文件结构*/
typedef struct _TWavHeader 
{          
	/*RIFF块*/
    int rId;    //标志符（RIFF）  0x46464952
    int rLen;   //数据大小,包括数据头的大小和音频文件的大小   (文件总大小-8)      
    int wId;    //格式类型（"WAVE"）   0x45564157
    
    /*Format Chunk*/
    int fId;    //"fmt " 带一个空格 0X20746D66     
    int fLen;   //Sizeof(WAVEFORMATEX)          
    short wFormatTag;       //编码格式，包括 1 WAVE_FORMAT_PCM，WAVEFORMAT_ADPCM等         
    short nChannels;        //声道数，单声道为1，双声道为2         
    int nSamplesPerSec;   //采样频率         
    int nAvgBytesPerSec;  //每秒的数据量         
    short nBlockAlign;      //块对齐          
    short wBitsPerSample;   //WAVE文件的采样大小         
    int dId;              //"data"     有可能是FACT块     
    int wSampleLength;    //音频数据的大小 
    /*紧跟后面可能有一个fact 块，跟压缩有关，如果没有，就是data块*/
}TWavHeader;


TWavHeader *wav_header;	
int SoundFileFd;//声音文件
volatile SOUND_State SoundSta = SOUND_IDLE;
u32 playlen;

/*------------------------------------------*/


AudioPipeNode pipenode;

/**
 *@brief:      fun_sound_play
 *@details:    通过指定设备播放指定声音
 *@param[in]   char *name  
               char *dev   
 *@param[out]  无
 *@retval:     
 */
int fun_sound_play(char *name, char *dev)
{
	unsigned int len;
	int rlen;
	
	SoundSta = SOUND_BUSY;
	/* 	打开文件是否需要关闭？
		同时打开很多文件事发后会内存泄漏。	*/
	SoundFileFd = vfs_open(name, O_RDONLY);
	if (SoundFileFd == NULL) {
		SOUND_DEBUG(LOG_DEBUG, "sound open file err\r\n");
		SoundSta = SOUND_IDLE;
		return -1;
	}

	SOUND_DEBUG(LOG_DEBUG, "sound open file ok\r\n");

	wav_header = (TWavHeader *)wjq_malloc(sizeof(TWavHeader));
	if (wav_header == 0) {
		SOUND_DEBUG(LOG_DEBUG, "sound malloc err!\r\n");
		SoundSta = SOUND_IDLE;
		return -1;
	}
	SOUND_DEBUG(LOG_DEBUG, "sound malloc ok\r\n");
	len = vfs_read(SoundFileFd, (void *)wav_header, sizeof(TWavHeader));
	if (len != sizeof(TWavHeader)) {
		SOUND_DEBUG(LOG_DEBUG, "sound read err\r\n");
		SoundSta = SOUND_IDLE;
		return -1;
	}

	SOUND_DEBUG(LOG_DEBUG, "sound read ok\r\n");
	if (len != sizeof(TWavHeader)) {
		SOUND_DEBUG(LOG_DEBUG, "read wav header err %d\r\n", len);
		SoundSta = SOUND_IDLE;
		return -1;
	}
	
	SOUND_DEBUG(LOG_DEBUG, "---%x\r\n", wav_header->rId);
	SOUND_DEBUG(LOG_DEBUG, "---%x\r\n", wav_header->rLen);
	SOUND_DEBUG(LOG_DEBUG, "---%x\r\n", wav_header->wId);//等于WAVE(0X45564157)，就说明是wave格式
	SOUND_DEBUG(LOG_DEBUG, "---%x\r\n", wav_header->fId);
	SOUND_DEBUG(LOG_DEBUG, "---%x\r\n", wav_header->fLen);
	SOUND_DEBUG(LOG_DEBUG, "---wave 格式 %x\r\n", wav_header->wFormatTag);
	SOUND_DEBUG(LOG_DEBUG, "---声道      %x\r\n", wav_header->nChannels);
	SOUND_DEBUG(LOG_DEBUG, "---采样频率  %d\r\n", wav_header->nSamplesPerSec);
	SOUND_DEBUG(LOG_DEBUG, "---每秒数据量 %d\r\n", wav_header->nAvgBytesPerSec);
	SOUND_DEBUG(LOG_DEBUG, "---样点字节数 %d\r\n", wav_header->nBlockAlign);
	SOUND_DEBUG(LOG_DEBUG, "---位宽 :    %d bit\r\n", wav_header->wBitsPerSample);
	SOUND_DEBUG(LOG_DEBUG, "---data =    %x\r\n", wav_header->dId);
	SOUND_DEBUG(LOG_DEBUG, "---数据长度: %x\r\n", wav_header->wSampleLength);


	/*根据文件内容设置采样频率跟样点格式*/
	pipenode.BitsPerSample = wav_header->wBitsPerSample;
	pipenode.nChannels = wav_header->nChannels;
	pipenode.nSamplesPerSec = wav_header->nSamplesPerSec;
	strcpy(pipenode.pipname, dev);
	
	int res;
	res = audio_pipe_open(&pipenode);
	if (res == -1) {
		SoundSta = SOUND_IDLE;
		return -1;
	}
	
	playlen = 0;
	rlen = pipenode.UpdateLen*2;
	len = vfs_read(SoundFileFd, (void *)pipenode.UpdatePointer, rlen);
	pipenode.UpdateLen = 0;
	
	playlen += len;

	audio_pipe_run(&pipenode);
	
	SoundSta = SOUND_PLAY;
	
	return 0;
}

/**
 *@brief:      fun_sound_task
 *@details:    声音播放轮询任务，执行间隔不可以太久，-
               否则声音会有杂音，也就是断续
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void fun_sound_task(void *userdata)
{

	int len;
	volatile s32 buf_index = 0;
	int rlen;
	u16 i;

	if(SoundSta == SOUND_BUSY
		|| SoundSta == SOUND_IDLE)
		return;
	
	if (0 != pipenode.UpdateLen) {
		rlen = pipenode.UpdateLen*2;
		
		//uart_printf("%08x ", pipenode.UpdatePointer);
	
		pipenode.UpdateLen = 0;
	
		if (SoundSta == SOUND_PAUSE) {
			for(i=0; i<rlen; i++) {
				*(pipenode.UpdatePointer + i) = 0x0000;
			}	
		} else {
			len = vfs_read(SoundFileFd, (void *)pipenode.UpdatePointer, rlen);
		}
		if (0 != pipenode.UpdateLen) {
			uart_printf(" ao ");
		}
		
		playlen += len;

		/*	u盘有BUG，有时候读到的数据长度不对
			稳健的做法是用已经播放的长度跟音源长度比较。		*/
		if(len < rlen) {
			fun_sound_stop();
			SOUND_DEBUG(LOG_DEBUG, "play finish %d, playlen:%x\r\n", len, playlen);
		}	
		
	}

}
/**
 *@brief:      fun_sound_get_sta
 *@details:    查询音乐播放状态
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
SOUND_State fun_sound_get_sta(void)
{
	return SoundSta;

}
/**
 *@brief:      fun_sound_stop
 *@details:    停止音乐播放
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 fun_sound_stop(void)
{
	u32 res;
	
	if(SoundSta == SOUND_PLAY
		|| SoundSta == SOUND_PAUSE)	
	{
		audio_pipe_close(&pipenode);
		
		vfs_close(SoundFileFd);
		SOUND_DEBUG(LOG_DEBUG, "f_close:%d\r\n", res);
		wjq_free(wav_header);
		SoundSta = SOUND_IDLE;	
	}
	return 0;
}
/**
 *@brief:      fun_sound_pause
 *@details:    暂停播放
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 fun_sound_pause(void)
{
	if (SoundSta == SOUND_PLAY) {
		SoundSta = SOUND_PAUSE;
	}
	return 0;
}
/**
 *@brief:      fun_sound_resume
 *@details:    恢复播放
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 fun_sound_resume(void)
{
	if (SoundSta == SOUND_PAUSE) {
		SoundSta = SOUND_PLAY;
	}
	return 0;
}
/**
 *@brief:      fun_sound_setvol
 *@details:    设置音量
 *@param[in]   u8 vol  
 *@param[out]  无
 *@retval:     
 */
s32 fun_sound_setvol(u8 vol)
{
	return 0;
}

/**
 *@brief:      fun_sound_test
 *@details:    测试播放
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void fun_sound_test(void)
{
	SOUND_DEBUG(LOG_DEBUG, "play sound\r\n");
	fun_sound_play("1:/mono_16bit_8k.wav", "dacsound");		

}
/*-------------------------------------------------------------------------------------*/
/*

	通过I2S利用WM8978录音
	跟播放一样，通过双缓冲DMA录音。
	需要注意的一点是，录音属于I2S_EXIT，不产生时钟，需要I2S才能产生通信时钟。
	也就是说要播音才能录音。
	所以录音时，也要配置I2S播放，我们只配置一个字节的DMA缓冲，以便I2S产生通信时钟，
	
*/
#include "wm8978.h"
#include "dacsound.h"

#if 0
/*---录音跟播音缓冲要差不多，否则会互相卡顿----*/
u32 SoundRecBufSize;
u16 *SoundRecBufP[2];
static u8 SoundRecBufIndex=0xff;//双缓冲索引，取值0和1，都填充后赋值0XFF
TWavHeader *recwav_header;	
int SoundRecFileFd;//声音文件
u32 RecWavSize = 0;
u16 RecPlayTmp[8];
s32 RecSta = 0;

/*
	录音频率，如果考虑播音跟录音一起工作，
	需要综合考虑如何配置录音频率
*/
#define SOUND_REC_FRE 32000
/**
 *@brief:      fun_rec_set_free_buf
 *@details:    设置录音缓冲索引，在I2S exit中断中使用
 *@param[in]   u8 index  
 *@param[out]  无
 *@retval:     
 */
s32 fun_rec_set_free_buf(u8 index)
{
	SoundRecBufIndex = index;
	return 0;
}
/**
 *@brief:      fun_rec_get_buff_index
 *@details:    查询录音满的缓冲，满就要读走
 *@param[in]   void  
 *@param[out]  无
 *@retval:     static
 */
static s32 fun_rec_get_buff_index(void)
{
	s32 res;

	res = SoundRecBufIndex;
	SoundRecBufIndex = 0xff;
	return res;
}
/**
 *@brief:      fun_sound_rec
 *@details:    启动录音
 *@param[in]   char *name  
 *@param[out]  无
 *@retval:     
 */
s32 fun_sound_rec(char *name)
{
	u32 len;

	SOUND_DEBUG(LOG_DEBUG, "sound rec\r\n");
	RecWavSize = 0;
	SoundRecBufSize = SoundBufSize;

	/*  创建WAV文件 */
	SoundRecFileFd = vfs_open(name, O_CREAT);
	if(SoundRecFileFd == NULL)			//文件创建失败
	{
		SOUND_DEBUG(LOG_DEBUG, "create rec file err!\r\n");
		return -1;
	}

	recwav_header = (TWavHeader *)wjq_malloc(sizeof(TWavHeader));
	if(recwav_header == NULL)
	{
		SOUND_DEBUG(LOG_DEBUG, "rec malloc err!\r\n");
		return -1;	
	}
	
	recwav_header->rId=0X46464952;
	recwav_header->rLen = 0;//录音结束后填
	recwav_header->wId = 0X45564157;//wave
	recwav_header->fId=0X20746D66;
	recwav_header->fLen = 16;
	recwav_header->wFormatTag = 0X01;
	recwav_header->nChannels = 2;
	recwav_header->nSamplesPerSec = SOUND_REC_FRE;//这个采样频率需要特殊处理，暂时不做。
	recwav_header->nAvgBytesPerSec = (recwav_header->nSamplesPerSec)*(recwav_header->nChannels)*(16/8);
	recwav_header->nBlockAlign = recwav_header->nChannels*(16/8);
	recwav_header->wBitsPerSample = 16;
	recwav_header->dId = 0X61746164;
	recwav_header->wSampleLength = 0;
	len = vfs_write(SoundRecFileFd, (const void*)recwav_header, sizeof(TWavHeader));
	if(len != sizeof(TWavHeader))
	{
		SOUND_DEBUG(LOG_DEBUG, "rec write err!\r\n");
		wjq_free(recwav_header);
		return -1;		
	}
	else
	{
		SOUND_DEBUG(LOG_DEBUG, "create rec wav ok!\r\n");
	}

	/*  测试录音     */
	SoundRecBufP[0] = (u16 *)wjq_malloc(SoundRecBufSize*2); 
	SoundRecBufP[1] = (u16 *)wjq_malloc(SoundRecBufSize*2); 
	
	SOUND_DEBUG(LOG_DEBUG, "%08x, %08x\r\n", SoundRecBufP[0], SoundRecBufP[1]);
	if(SoundRecBufP[0] == NULL)
	{

		SOUND_DEBUG(LOG_DEBUG, "sound malloc err\r\n");
		return -1;
	}

	if(SoundRecBufP[1] == NULL )
	{
		wjq_free(SoundRecBufP[0]);
		return -1;
	}
	
	dev_wm8978_open();	
	dev_wm8978_inout(WM8978_INPUT_DAC|WM8978_INPUT_AUX|WM8978_INPUT_ADC|WM8978_INPUT_LMIC|WM8978_INPUT_RMIC,
					WM8978_OUTPUT_PHONE);
	dev_wm8978_dataformat(SOUND_REC_FRE, WM8978_I2S_Phillips, WM8978_I2S_Data_16b);
	mcu_i2s_dma_init(RecPlayTmp, RecPlayTmp, 1);

	mcu_i2sext_dma_init(SoundRecBufP[0], SoundRecBufP[1], SoundRecBufSize);
	mcu_i2sext_dma_start();
	
	RecSta = 1;
	
	dev_wm8978_transfer(1);//启动I2S传输
	

	SOUND_DEBUG(LOG_DEBUG, "rec--------------------\r\n");
	
	return 0;
}
/**
 *@brief:      fun_rec_stop
 *@details:    停止录音
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 fun_rec_stop(void)
{
	u32 len;

	RecSta = 0;
	
	dev_wm8978_transfer(0);
	mcu_i2sext_dma_stop();
	
	recwav_header->rLen = RecWavSize+36;
	recwav_header->wSampleLength = RecWavSize;
	
	vfs_lseek(SoundRecFileFd, 0, SEEK_SET);
	len = vfs_write(SoundRecFileFd, (const void*)recwav_header, sizeof(TWavHeader));
	vfs_close(SoundRecFileFd);

	wjq_free(SoundRecBufP[0]);
	wjq_free(SoundRecBufP[1]);
	wjq_free(recwav_header);
	return 0;
}
/**
 *@brief:      fun_rec_task
 *@details:    录音线程
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void fun_rec_task(void)
{
	int buf_index = 0;
	u32 len;

	if(RecSta == 0)
		return;
	
	buf_index = fun_rec_get_buff_index();
	if(0xff != buf_index)
	{
		//uart_printf("rec buf full:%d!\r\n", buf_index);
		RecWavSize += SoundRecBufSize*2;
		len = vfs_write(SoundRecFileFd, (const void*)SoundRecBufP[buf_index], 2*SoundRecBufSize);
		if(len != 2*SoundRecBufSize)
		{
			SOUND_DEBUG(LOG_DEBUG, "write err\r\n");
		}
		
		if(len!= 2*SoundRecBufSize)
		{
			SOUND_DEBUG(LOG_DEBUG, "len err\r\n");
		}
		
	}
}

/**
 *@brief:      fun_rec_test
 *@details:    开始录音
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void fun_rec_test(void)
{
	fun_sound_rec("1:/rec9.wav");
}

void fun_play_rec_test(void)
{
	fun_sound_play("1:/rec9.wav", "wm8978");	
}

#endif

#endif

