#ifdef __cplusplus
    extern "C" {
    #endif

#define AUDIO_OUT_BUFFER_SIZE                      4096*6		// Min Buffer 4096*2 for fluid GUI 9ms for 4096 samples
#define TOUCHGUI_POT_MAX	4095

#include "main.h"

#ifndef __GILLES__
#define __GILLES__
typedef struct {
  uint32_t ChunkID;       /* 0 */
  uint32_t FileSize;      /* 4 */
  uint32_t FileFormat;    /* 8 */
  uint32_t SubChunk1ID;   /* 12 */
  uint32_t SubChunk1Size; /* 16*/
  uint16_t AudioFormat;   /* 20 */
  uint16_t NbrChannels;   /* 22 */
  uint32_t SampleRate;    /* 24 */

  uint32_t ByteRate;      /* 28 */
  uint16_t BlockAlign;    /* 32 */
  uint16_t BitPerSample;  /* 34 */
  uint32_t SubChunk2ID;   /* 36 */
  uint32_t SubChunk2Size; /* 40 */
}WAVE_FormatTypeDef;

typedef enum {
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF,
  BUFFER_OFFSET_FULL,
}BUFFER_StateTypeDef;

typedef enum {
  AUDIO_ERROR_NONE = 0,
  AUDIO_ERROR_IO,
  AUDIO_ERROR_EOF,
  AUDIO_ERROR_INVALID_VALUE,
}AUDIO_ErrorTypeDef;

/* Audio buffer control struct */
typedef struct {
  uint8_t buff[AUDIO_OUT_BUFFER_SIZE];
  BUFFER_StateTypeDef state;
  uint32_t fptr;
  uint32_t fptrstart;
  uint32_t fptrend;
}AUDIO_OUT_BufferTypeDef;
#endif

#ifdef	__WAVE__
#define WAV_EXTRN
#else
#define WAV_EXTRN	extern
#endif

WAV_EXTRN WAVE_FormatTypeDef WaveFormat;


#ifdef __cplusplus
}
#endif
