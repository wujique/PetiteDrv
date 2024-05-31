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


#endif


