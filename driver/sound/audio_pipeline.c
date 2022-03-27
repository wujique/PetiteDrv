
#include "mcu.h"
#include "petite_config.h"
#include "petite.h"

#include "audio_pipeline.h"

#include "wm8978.h"
#include "dacsound.h"
//#include "soundplay.h"

/*
	将codec和pip连接到一起
	比如wm8978和I2S
	*/

/*
播放SD卡的音乐，只要2*4K缓冲
播放U盘中的音乐，却要2*8K
*/
#define I2S_DMA_BUFF_SIZE1   (1024*4*2)
#define DAC_SOUND_BUFF_SIZE2 (1024*2*2)

/*
	单声道数据使用WM8978播放要经过处理，
	造成双声道 */
#if 0
static s32 fun_sound_deal_1ch_data(u8 *p)
{
	u8 ch1,ch2;
	u16 shift;
	u16 i;
	
	for (i=SoundBufSize;i>0;) {
		i--;
		//uart_printf("%d-",i);
		ch1 = *(p+i);
		i--;
		ch2 = *(p+i);
		
		shift = i*2;
		
		*(p+shift) = ch2;	
		*(p+shift+1) = ch1;
		*(p+shift+2) = ch2;	
		*(p+shift+3) = ch1;
	}
	
	return 0;
}
#endif

/*
	打开并配置一个音频通道          	*/
s32 audio_pipe_open(AudioPipeNode *anode)
{

	/* 根据配置，申请一个 节点，节点指针返回给上层 */	
	/*  小于16K 使用小一点的缓冲 */
	if (anode->BitsPerSample <= 16000) {
		anode->buflen = DAC_SOUND_BUFF_SIZE2;
	} else {
		anode->buflen = I2S_DMA_BUFF_SIZE1;
	}

	anode->buf = (u16 *)wjq_malloc(anode->buflen*sizeof(u16)); 

	wjq_log(LOG_DEBUG, "%08x\r\n", anode->buf);
	if (anode->buf == NULL) {

		wjq_log(LOG_DEBUG, "sound malloc err\r\n");

		return -1;
	}
	/* 预先 填充 前半部分buf  need fix*/
	anode->UpdatePointer = anode->buf;
	anode->UpdateLen = anode->buflen;
	
	wjq_log(LOG_DEBUG, "buf len: %d\r\n", anode->buflen);
	
	return 0;
}


/*
	获取pipe状态？
	*/
s32 audio_pipe_run(AudioPipeNode *anode)
{
	
	if(0 == strcmp(anode->pipname, "wm8978")) {
		wjq_log(LOG_DEBUG, "wm8978 run\r\n");
		
		dev_wm8978_open();
		dev_wm8978_inout(WM8978_INPUT_DAC|WM8978_INPUT_AUX|WM8978_INPUT_ADC,
					WM8978_OUTPUT_PHONE|WM8978_OUTPUT_SPK);

		dev_wm8978_set_dataformat(AUDIO_I2S_Phillips, anode->BitsPerSample);

		mcu_i2s_dataformat(anode->nSamplesPerSec, AUDIO_I2S_Phillips, anode->BitsPerSample);
		mcu_i2s_dma_init(anode->buf, anode->buflen);
		mcu_i2s_dma_start();//启动I2S传输	
	
	} else {
	#if 0
		dev_dacsound_open();
		dev_dacsound_dataformat(wav_header->nSamplesPerSec, WM8978_I2S_Phillips, format);
		dev_dacsound_setbuf(SoundBufP[0], SoundBufP[1], SoundBufSize);
		SoundDevType = SOUND_DEV_1CH;
		dev_dacsound_transfer(1);
	#endif
	}

	
}
	

s32 audio_pipe_close(AudioPipeNode *anode)
{
	if(0 == strcmp(anode->pipname, "wm8978")) {
		mcu_i2s_dma_stop(); 
	} else {
		//dev_dacsound_transfer(0);
		//dev_dacsound_close();
	}

}

/**
 *@brief:      fun_sound_set_free_buf
 *@details:    设置空闲缓冲索引
 			   这个函数提供给I2S或者DAC SOUND模块调用
 *@param[in]   u8 *index  0 前半buf可填充， 1 后半buf可填充
 *@param[out]  无
 *@retval:     
 */

extern AudioPipeNode pipenode;
AudioPipeNode *pnode = &pipenode;

s32 audio_pipe_callback(u8 index)
{
	//uart_printf("%d ", index);
	if (index == 0) {
		pnode->UpdatePointer = pnode->buf;
		pnode->UpdateLen = pnode->buflen/2;
	} else if (index == 1) {
		pnode->UpdatePointer = pnode->buf+ pnode->buflen/2;
		pnode->UpdateLen = pnode->buflen/2;
	}
	
	return 0;
}




