/**
  ******************************************************************************
  * @file    Audio/Audio_playback_and_record/Inc/waveplayer.h
  * @author  MCD Application Team
  * @brief   Header for waveplayer.c module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */  
/* Define to prevent recursive inclusion -------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __WAVEPLAYER__
#define WAVE_EXTRN
#else
#define WAVE_EXTRN extern
#endif


/* Includes ------------------------------------------------------------------*/
#include "wave.h"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

// Size of the big buffer 176400 -> 1 sec 16 bits 2 Channel 44100 -> 44100*2*2 border 4096


#define 	GRAIN_BIGBUFFER_SIZE	245760*2



#ifdef __WAVEPLAYER__
uint8_t		u8_Tab_BigBuffer[GRAIN_BIGBUFFER_SIZE] __attribute__ ((section (".bigdata")));		// WARNING VERY BIG BUFFER
#else
extern uint8_t		u8_Tab_BigBuffer[GRAIN_BIGBUFFER_SIZE] __attribute__ ((section (".bigdata")));		// WARNING VERY BIG BUFFER
#endif


WAVE_EXTRN uint8_t		u8_File8Select;			// For the scrool whell
WAVE_EXTRN uint8_t		u8_File8SelectValid;	// File really select on a push buotton

WAVE_EXTRN AUDIO_OUT_BufferTypeDef  BufferCtl;

WAVE_EXTRN uint32_t startgrain;

WAVE_EXTRN WAVE_FormatTypeDef WaveFormat;
WAVE_EXTRN uint16_t Aff_Grain_divx;

#define LOCK_DMA	1
#define UNLOCK_DMA	0

#define LOCK_TASK	1
#define UNLOCK_TASK	0


WAVE_EXTRN uint8_t  Ask_REDRAWBIGBUFFER;

WAVE_EXTRN uint8_t  Lock_DMA;
WAVE_EXTRN uint8_t  Lock_TASK;

WAVE_EXTRN uint8_t  Lock_IT1ms;

WAVE_EXTRN uint8_t	half_full;

typedef enum {
  AUDIO_STATE_IDLE = 0,
  AUDIO_STATE_WAIT,
  AUDIO_STATE_INIT,
  AUDIO_STATE_PLAY,
  AUDIO_STATE_PRERECORD,
  AUDIO_STATE_RECORD,
  AUDIO_STATE_NEXT,
  AUDIO_STATE_PREVIOUS,
  AUDIO_STATE_FORWARD,
  AUDIO_STATE_BACKWARD,
  AUDIO_STATE_STOP,
  AUDIO_STATE_PAUSE,
  AUDIO_STATE_RESUME,
  AUDIO_STATE_VOLUME_UP,
  AUDIO_STATE_VOLUME_DOWN,
  AUDIO_STATE_ERROR,
}AUDIO_PLAYBACK_StateTypeDef;




#define WAVEPLAY_MODE_FILE					0			// Play all the file and loop
														// Go to WAVEPLAY_MODE_BIGBUFFERINFILEREC with the PB1
#define WAVEPLAY_MODE_BIGBUFFERINFILEREC	1			// Play only the big buffer in the file view and loop in this section Rec the samples in the big buffer
#define WAVEPLAY_MODE_BIGBUFFERINFILEFULL	2			// Play only the big buffer in the file view and loop in this section Rec Full
														// Go back to rec mode if we change the pos of the buffer
														// Go back to WAVEPLAY_MODE_FILE with the PB1
														// Go to WAVEPLAY_MODE_BIGBUFFERVIEW with the PB2
#define WAVEPLAY_MODE_BIGBUFFERVIEW			3			// Play the big buffer in the big buffer view
WAVE_EXTRN uint8_t u8_Waveplayer_Mode;


//#define TRIGBIGBUFFER_STANDBY			0			// FIll big buffer wait for trig
//#define TRIGBIGBUFFER_RECORD			1			// Record the big buffer
//#define TRIGBIGBUFFER_FULL				2			// The big buffer is full
//WAVE_EXTRN uint8_t	u8_Grain_TrigBigBuffer_Full;	// Check if the buffer is full

WAVE_EXTRN AUDIO_ErrorTypeDef GetFileInfo(char* filename, WAVE_FormatTypeDef *info);

WAVE_EXTRN AUDIO_ErrorTypeDef Audio_Play_File(uint8_t idx,uint32_t startpos);

WAVE_EXTRN AUDIO_ErrorTypeDef AUDIO_PLAYER_Init(void);
WAVE_EXTRN AUDIO_ErrorTypeDef AUDIO_PLAYER_Start(uint8_t idx,uint16_t* size);
WAVE_EXTRN AUDIO_ErrorTypeDef AUDIO_PLAYER_Process(void);
WAVE_EXTRN AUDIO_ErrorTypeDef AUDIO_PLAYER_Stop(void);


#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
