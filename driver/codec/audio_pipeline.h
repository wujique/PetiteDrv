#ifndef __AUDIO_PIPELINE_H__
#define __AUDIO_PIPELINE_H__

typedef struct {
	char pipname[16];

	/* config */
	short BitsPerSample;
	int nSamplesPerSec;
	short nChannels;
	
	/* */
	__IO uint16_t *UpdatePointer;/* 更新偏移 -1 不需要更新 */
	__IO int16_t UpdateLen;//u16长度
	uint16_t *buf;
	uint32_t buflen;

}AudioPipeNode;


#define 	AUDIO_I2S_LSB         0x00	
#define 	AUDIO_I2S_MSB         0x01
#define 	AUDIO_I2S_Phillips    0x02
#define 	AUDIO_I2S_PCM         0x03


#if 0	
#define 	AUDIO_I2S_Data_16b    0x00
#define 	AUDIO_I2S_Data_20b    0x01
#define 	AUDIO_I2S_Data_24b    0x02
#define 	AUDIO_I2S_Data_32b    0x03
#endif

#endif


