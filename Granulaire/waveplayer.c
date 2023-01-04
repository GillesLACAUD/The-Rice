/**
  ******************************************************************************
  * @file    Audio/Audio_playback_and_record/Src/waveplayer.c 
  * @author  MCD Application Team
  * @brief   This file provides the Audio Out (playback) interface API
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
/* Includes ------------------------------------------------------------------*/
#define __WAVEPLAYER__
#include "waveplayer.h"
#include "affwave.h"
#include "Grain.h"
#include "ff.h"
#include "UsbStorage.h"
#include "MessageEvent.h"

/* TODO find a solution without all the path */
#include "C:/TouchGFXProjects/MyApplication/Drivers/BSP/Components/cs43l22/cs43l22.h"
#include "C:/TouchGFXProjects/MyApplication/Drivers/BSP/STM32469I-Discovery/stm32469i_discovery_audio.h"
#include "Matrix.h"
#include "Midi.h"
#include "Encoder.h"


extern RNG_HandleTypeDef hrng;
extern uint32_t gui_Random_Number;
extern xSemaphoreHandle 	xSemDmaSai;

extern FIL WavFile;
extern FILELIST_FileTypeDef FileList;

AUDIO_PLAYBACK_StateTypeDef AudioState;

static int16_t FilePos = 0;
static __IO uint32_t uwVolume = 70;	// Volume for the CODEC

FIL WavFile;
FIL SndFile;

/* Private function prototypes -----------------------------------------------*/
static uint8_t PlayerInit(uint32_t AudioFreq);
static void AUDIO_PlaybackDisplayButtons(void);
static void AUDIO_AcquireTouchButtons(void);

/* Private functions ---------------------------------------------------------*/

uint32_t posfile;

uint8_t bufftmp[AUDIO_OUT_BUFFER_SIZE];			// Temp buffer to appli the ENV AR
uint8_t buffcumul[AUDIO_OUT_BUFFER_SIZE*2];		//

/**************************************************/
/* Audio Play File                                */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
AUDIO_ErrorTypeDef Audio_Play_File(uint8_t idx,uint32_t startpos)
{
}


/**************************************************/
/* AR ENV                                         */
/* 0      No sound                                */
/* 100000 Max output                              */
/*                                                */
/**************************************************/
uint32_t Audio_Env_Grain(uint8_t v,uint8_t grain)
{
uint32_t t=100000;

	switch(str_Sound_Work.str_tabgrain[v][grain].u8_stateAR)
	{
		case GRAIN_AR_STATE_ATTACK:
		if(str_Sound_Work.str_tabgrain[v][grain].u32_volA<100000)
			str_Sound_Work.str_tabgrain[v][grain].u32_volA +=str_Sound_Work.u32_deltaA;
		else
		{
			str_Sound_Work.str_tabgrain[v][grain].u32_volA = 90000;
			if(str_Sound_Work.u8_sustain == AG_GRAIN_SUSTAIN_ON)
			{
				str_Sound_Work.str_tabgrain[v][grain].u8_stateAR=GRAIN_AR_STATE_SUSTAIN;
			}
			else
			{
				str_Sound_Work.str_tabgrain[v][grain].u8_stateAR = GRAIN_AR_STATE_RELEASE;
				str_Sound_Work.str_tabgrain[v][grain].s32_volR = 100000;
				t=100000;
			}
		}
		t = str_Sound_Work.str_tabgrain[v][grain].u32_volA;
		break;

		case GRAIN_AR_STATE_SUSTAIN:
		str_Sound_Work.str_tabgrain[v][grain].s32_volR = 100000;
		// Trig the Release
		if(str_Sound_Work.str_tabgrain[v][grain].u16_cptAR > str_Sound_Work.u16_TrigR)
		{
			str_Sound_Work.str_tabgrain[v][grain].u8_stateAR = GRAIN_AR_STATE_RELEASE;
		}
		t=100000;
		break;

		case GRAIN_AR_STATE_RELEASE:
		if(str_Sound_Work.str_tabgrain[v][grain].s32_volR>str_Sound_Work.u32_deltaR*2)
		{
			str_Sound_Work.str_tabgrain[v][grain].s32_volR -=str_Sound_Work.u32_deltaR;
			t = str_Sound_Work.str_tabgrain[v][grain].s32_volR;
		}
		else
		{
			str_Sound_Work.str_tabgrain[v][grain].u8_stateAR=GRAIN_AR_STATE_STANDBY;
			//str_Sound_Work.str_tabgrain[v][grain].u8_stateAR=GRAIN_AR_STATE_ATTACK;
			//str_Sound_Work.str_tabgrain[v][grain].u16_cptAR=0;
			str_Sound_Work.str_tabgrain[v][grain].u32_volA=0;
			str_Sound_Work.str_tabgrain[v][grain].s32_volR=0;
			t=0;
		}
		break;

		case GRAIN_AR_STATE_STANDBY:
		t=0;
		break;
	}
	return(t);
}

//***********************************************************
// LIN
//***********************************************************
double speed=0.5;
double cumulspeed=0;
uint16_t cptdest=0;

/**************************************************/
/* Transpose Pitch shifting                       */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
int MyTransposeStereo(uint8_t voice,int16_t *dest,int16_t *src,int16_t sizedest,int16_t* ptsrcSamples)
{
uint16_t srcCount=0;
uint16_t srcSampleEnd = *ptsrcSamples-0;
double whole=0;
double rest=0;

uint32_t u32_whole=0;
uint32_t u32_rest=0;

    while(srcCount < srcSampleEnd)
    {
        dest[0] += src[0];
        dest[1] += src[1];
        dest+=2;
        cptdest++;
        // En entier 1000 = 1.0 -> on pert des decimales
        str_Sound_Work.str_VOICE_Tab[voice].u32_cumulspeed +=str_Sound_Work.str_VOICE_Tab[voice].u32_speed;
        u32_whole = str_Sound_Work.str_VOICE_Tab[voice].u32_cumulspeed/1000;
        src += 2*u32_whole;
        u32_rest = str_Sound_Work.str_VOICE_Tab[voice].u32_cumulspeed - u32_whole*1000;
        str_Sound_Work.str_VOICE_Tab[voice].u32_cumulspeed = u32_rest;
        srcCount += u32_whole;

        //--------------------------------
		// The OUTPUT buffer is full
		//--------------------------------
		if(cptdest>=sizedest)
		{
			cptdest=0;break;
		}
    }
    //-----------------------------------------------
	// ptsrcSamples is the rest of the INPUT buffer
	//-----------------------------------------------
	if(srcCount==*ptsrcSamples)
		*ptsrcSamples = 0;
	else
		*ptsrcSamples -= srcCount;
	if(*ptsrcSamples<0) *ptsrcSamples=0;
	return cptdest;
}


uint32_t posstart;
uint16_t incoff;
uint16_t off;
uint16_t cptit=0;

uint8_t pantoogle=0;
uint16_t panleft;
uint16_t panright;

uint32_t begin;
int32_t max=0;
int32_t min=0;
float dv;

uint16_t u16_Cpt_BufferSrc=0;
uint16_t u16_Cpt_BufferDst=0;

int16_t addtri;
int16_t signe=1;


uint8_t g;
uint8_t g_actif=0;


/**************************************************/
/* Generate Grain                                 */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
void Generate_Grain(uint8_t v)
{
	uint16_t i;
	uint32_t t;
	uint16_t divl;
	uint16_t divr;
	int volL;
	int volR;
	int div;

	//-----------------------------
	// OK Valide le 05.05.2020
	//-----------------------------
	if(0)
	{
		// Generate a TRI in the bigbuffer
		int16_t inc = 16000/(AUDIO_OUT_BUFFER_SIZE/2);
		inc *=32;

		for(i=0;i<AUDIO_OUT_BUFFER_SIZE/2;i+=4)
		{
			addtri +=inc*signe;
			*(int16_t*)&str_Sound_Work.str_VOICE_Tab[v].u8_buffcumul[0+i]=addtri;
			*(int16_t*)&str_Sound_Work.str_VOICE_Tab[v].u8_buffcumul[2+i]=addtri;
			if(addtri > 16000)
			{
				signe = -1;
			}
			if(addtri <= -16000)
			{
				signe = +1;
			}
		}
		//return;
	}

	//-----------------------------
	// OK Valide le 05.05.2020
	//-----------------------------
	static uint16_t cpt=0;
	if(0)
	{

		memset(&str_Sound_Work.str_VOICE_Tab[v].u8_buffcumul[0],0,AUDIO_OUT_BUFFER_SIZE/2);

		posstart=(cpt*AUDIO_OUT_BUFFER_SIZE)/4;
		cpt++;
		posstart >>=u8_Type_BigBuffer;
		posstart <<=u8_Type_BigBuffer;
		memcpy(&bufftmp[0],&u8_Tab_BigBuffer[posstart],AUDIO_OUT_BUFFER_SIZE/4/*+16*/);		//

		for(i=0;i<AUDIO_OUT_BUFFER_SIZE/2/*+8*/;i+=4)
		{
			*(int16_t*)&str_Sound_Work.str_VOICE_Tab[v].u8_buffcumul[0+i] = (*(int16_t*)&bufftmp[0+i/2]);
			*(int16_t*)&str_Sound_Work.str_VOICE_Tab[v].u8_buffcumul[2+i] = (*(int16_t*)&bufftmp[0+i/2]);
		}
		if(posstart>GRAIN_BIGBUFFER_SIZE-1*AUDIO_OUT_BUFFER_SIZE)
			cpt=0;
		return;
	}

	memset(&str_Sound_Work.str_VOICE_Tab[v].u8_buffcumul[0],0,AUDIO_OUT_BUFFER_SIZE/2);
	off=str_Sound_Work.u32_offsetnextgrain;

	for(g=0;g<str_Sound_Work.u8_grainnb;g++)
	{
		str_Sound_Work.str_tabgrain[v][g].u16_cptAR++;	// A grain can trig a grain after standby
		if(str_Sound_Work.str_tabgrain[v][g].u8_stateAR!=GRAIN_AR_STATE_STANDBY)
		{
			//--------------------------------------------------------------------------------------------
			// Read a part of the big buffer
			//--------------------------------------------------------------------------------------------
			posstart=str_Sound_Work.str_tabgrain[v][g].u32_beginpos+(str_Sound_Work.str_tabgrain[v][g].u16_cptAR)*AUDIO_OUT_BUFFER_SIZE/4;
			posstart >>=u8_Type_BigBuffer;
			posstart <<=u8_Type_BigBuffer;
			// If we are to close to the end of the big buffer
			if(posstart>GRAIN_BIGBUFFER_SIZE-str_Sound_Work.str_tabgrain[v][g].u32_size)
				posstart=GRAIN_BIGBUFFER_SIZE-str_Sound_Work.str_tabgrain[v][g].u32_size;

			//--------------------------------------------------------------------------------------------
			// Apply the AR Env to the buffer cumul
			//--------------------------------------------------------------------------------------------

			// volume 		float 	from 0 		to 	1.0
			// panx 		uint6_t	from 400 	to 	1600
			// div 			int		from 100	to  5000 (for 16 grains)

			t=Audio_Env_Grain(v,g);		// 0 to 10000
			t /=4;						// 0 to 25000
			volL = str_Sound_Work.str_tabgrain[v][g].u16_panleft*t/100000;
			volR = str_Sound_Work.str_tabgrain[v][g].u16_panright*t/100000;
			div = 1000 + (700*str_Sound_Work.u8_grainnb)/3;
			int16_t sample;

			volL *= str_Sound_Work.u32_Grain_GeneralVolume;
			volL /=125;
			volR *= str_Sound_Work.u32_Grain_GeneralVolume;
			volR /=125;

			// Apply the ADSR for the voice
			float dv;
			if(!str_Sound_Work.u8_adsr1bypass)
				dv= (float)str_Sound_Work.str_ADSR_AMP_onevoice[v].i16_vol/1000;
			else
				dv=1;

			if(str_Sound_Work.str_ADSR_AMP_onevoice[v].u8_stateADSR !=GRAIN_ADSR_STATE_STANDBY)
			{
				volL *= dv;
				volR *= dv;
			}
			else
			{
				volL =0;
				volR =0;
			}
			for(i=0;i<AUDIO_OUT_BUFFER_SIZE/2;i+=4)
			{
				sample = *(int16_t*)&u8_Tab_BigBuffer[posstart+i/2];
				*(int16_t*)&str_Sound_Work.str_VOICE_Tab[v].u8_buffcumul[0+i] += sample*volL/div;
				*(int16_t*)&str_Sound_Work.str_VOICE_Tab[v].u8_buffcumul[2+i] += sample*volR/div;
			}
		}
		//else
		//{
			//--------------------------------------------------------------------------------------------
			// Trig the Next Grain ?
			//--------------------------------------------------------------------------------------------
			g_actif=0;
			if(str_Sound_Work.str_tabgrain[v][g].u16_cptAR > str_Sound_Work.u16_Grain_NextTrig)
			{
				//str_Sound_Work.str_tabgrain[v][g].u16_cptAR=0;
				// Not the last Grain
				if(g<str_Sound_Work.u8_grainnb-1)
				{
					g_actif = g+1;
					if(str_Sound_Work.str_tabgrain[v][g_actif].u8_stateAR==GRAIN_AR_STATE_STANDBY)
					{
						uint32_t rand;
						uint32_t temp;
						if(!str_Sound_Work.u8_spaceranfixe)
						{
							HAL_RNG_GenerateRandomNumber(&hrng,&rand);
							temp = rand;
							//temp &= 0x1FFF;
							temp &= 0xFFF;
							temp *=str_Sound_Work.u8_rangeshift;
							temp +=1000;
							str_Sound_Work.u32_offsetnextgrain=temp;
//							str_Sound_Work.u32_offsetnextgrain=rand;
//							str_Sound_Work.u32_offsetnextgrain &= 0x1FFF*str_Sound_Work.u8_rangeshift;	// Set the range of the random
//							str_Sound_Work.u32_offsetnextgrain +=4000;
							//str_Sound_Work.u32_offsetnextgrain >>=4;
							//str_Sound_Work.u32_offsetnextgrain <<=4;
						}
						else
						{
							str_Sound_Work.u32_offsetnextgrain = 1000+g_actif*str_Sound_Work.u8_rangeshift*1000; // max 4000+20000
						}
						u8_Ask_DrawGrain=0;
						str_Sound_Work.str_tabgrain[v][g_actif].u32_beginpos = u32_Mod_Grain_Begin+str_Sound_Work.u32_offsetnextgrain;


						if(str_Sound_Work.u8_AR_change== TRUE)
						{
							Grain_Set_AR();
							str_Sound_Work.u8_AR_change=FALSE;
						}
						pantoogle = !pantoogle;
						if(pantoogle)
						{
							panleft 	= 1000 +  str_Sound_Work.u8_pan*1000/120;		// MAX 1600
							panright	= 1000 - str_Sound_Work.u8_pan*1000/120;		// MIN 0400
						}
						else
						{
							panright 	= 1000 +  str_Sound_Work.u8_pan*1000/120;
							panleft		= 1000 - str_Sound_Work.u8_pan*1000/120;
						}
						str_Sound_Work.str_tabgrain[v][g_actif].u16_panleft 	= panleft;
						str_Sound_Work.str_tabgrain[v][g_actif].u16_panright 	= panright;
						str_Sound_Work.str_tabgrain[v][g_actif].u8_stateAR=GRAIN_AR_STATE_ATTACK;
						str_Sound_Work.str_tabgrain[v][g_actif].u16_cptAR=0;
						u8_Ask_DrawGrain=1;
					}
				}
				// Last Grain
				else
				{
					if(str_Sound_Work.str_tabgrain[v][0].u8_stateAR==GRAIN_AR_STATE_STANDBY)
					{
						g_actif =0;
						str_Sound_Work.str_tabgrain[v][0].u8_stateAR=GRAIN_AR_STATE_ATTACK;
						str_Sound_Work.str_tabgrain[v][0].u16_cptAR=0;

						u8_Ask_DrawGrain=0;
						//str_Sound_Work.str_tabgrain[v][0].u32_beginpos = u32_Mod_Grain_Begin;
						str_Sound_Work.str_tabgrain[v][0].u32_beginpos = str_Sound_Work.u32_Grain_Begin;
						if(str_Sound_Work.u8_AR_change== TRUE)
						{
							Grain_Set_AR();
							str_Sound_Work.u8_AR_change=FALSE;
						}
						pantoogle = !pantoogle;
						if(pantoogle)
						{
							panleft 	= 1000 +  str_Sound_Work.u8_pan*1000/120;
							panright	= 1000 - str_Sound_Work.u8_pan*1000/120;
						}
						else
						{
							panright 	= 1000 +  str_Sound_Work.u8_pan*1000/120;
							panleft		= 1000 - str_Sound_Work.u8_pan*1000/120;
						}
						str_Sound_Work.str_tabgrain[v][g_actif].u16_panleft 	= panleft;
						str_Sound_Work.str_tabgrain[v][g_actif].u16_panright 	= panright;
						u8_Ask_DrawGrain=1;
					}
				}
			}
			if(str_Sound_Work.str_tabgrain[v][g_actif].u32_beginpos >(GRAIN_BIGBUFFER_SIZE-u32_Mod_Grain_Size-2*AUDIO_OUT_BUFFER_SIZE))
			{
				str_Sound_Work.str_tabgrain[v][g_actif].u32_beginpos=GRAIN_BIGBUFFER_SIZE-u32_Mod_Grain_Size-2*AUDIO_OUT_BUFFER_SIZE;
			}
		//}
		if(str_Sound_Work.str_tabgrain[0][g].u32_beginpos <str_Sound_Work.u32_Grain_Begin)
		{
			str_Sound_Work.str_tabgrain[0][g].u32_beginpos =str_Sound_Work.u32_Grain_Begin;
		}



	}// end for grain
}


int16_t srcsample = AUDIO_OUT_BUFFER_SIZE/8;			// /4 For mono ok /8 Ok for stereo

/**************************************************/
/* Add one voice                                  */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Audio_Add_Voice(uint8_t v,uint8_t pos)
{
	u16_Cpt_BufferDst = str_Sound_Work.str_VOICE_Tab[v].u16_cpt_bufferdst;
	u16_Cpt_BufferSrc = str_Sound_Work.str_VOICE_Tab[v].u16_cpt_buffersrc;
	srcsample=str_Sound_Work.str_VOICE_Tab[v].i16_srcsample;

	u16_Cpt_BufferDst=MyTransposeStereo(v,(int16_t*)&BufferCtl.buff[pos*AUDIO_OUT_BUFFER_SIZE/2+u16_Cpt_BufferDst*4],(int16_t*)&str_Sound_Work.str_VOICE_Tab[v].u8_buffcumul[u16_Cpt_BufferSrc*4],AUDIO_OUT_BUFFER_SIZE/8,&srcsample);
	u16_Cpt_BufferSrc =(AUDIO_OUT_BUFFER_SIZE/8)-srcsample;

	if(str_Sound_Work.str_VOICE_Tab[v].u32_speed <= 1000)
	{
		//--------------------------------
		// INPUT buffer is empty
		//--------------------------------
		if(srcsample==0)
		{
			u16_Cpt_BufferSrc=0;
			srcsample = AUDIO_OUT_BUFFER_SIZE/8;
			Generate_Grain(v);				// Rempli 2048 bytes donc AUDIO_OUT_BUFFER_SIZE/2
			//--------------------------------
			// If the OUTPUT buffer is not full
			// complete the buffer
			// u16_Cpt_BufferDst = 512 -> Output buffer full
			// u16_Cpt_BufferDst !=512 -> rest 512-u16_Cpt_BufferDst
			//--------------------------------
			if(u16_Cpt_BufferDst != 0)
			{
				u16_Cpt_BufferDst=MyTransposeStereo(v,(int16_t*)&BufferCtl.buff[pos*AUDIO_OUT_BUFFER_SIZE/2+u16_Cpt_BufferDst*4],(int16_t*)&str_Sound_Work.str_VOICE_Tab[v].u8_buffcumul[u16_Cpt_BufferSrc*4],AUDIO_OUT_BUFFER_SIZE/8,&srcsample);
				u16_Cpt_BufferSrc = (AUDIO_OUT_BUFFER_SIZE/8)-srcsample;
			}
		}
	}
	else
	{
		//--------------------------------
		// OUTPUT buffer full wait
		//--------------------------------
		while(u16_Cpt_BufferDst!=0)
		{
			u16_Cpt_BufferSrc=0;
			srcsample = AUDIO_OUT_BUFFER_SIZE/8;
			Generate_Grain(v);
			u16_Cpt_BufferDst=MyTransposeStereo(v,(int16_t*)&BufferCtl.buff[pos*AUDIO_OUT_BUFFER_SIZE/2+u16_Cpt_BufferDst*4],(int16_t*)&str_Sound_Work.str_VOICE_Tab[v].u8_buffcumul[u16_Cpt_BufferSrc*4],AUDIO_OUT_BUFFER_SIZE/8,&srcsample);
			u16_Cpt_BufferSrc = (AUDIO_OUT_BUFFER_SIZE/8)-srcsample;
		}
		//--------------------------------
		// INPUT buffer is empty
		//--------------------------------
		if(srcsample==0)
		{
			u16_Cpt_BufferSrc=0;
			srcsample = AUDIO_OUT_BUFFER_SIZE/8;
			Generate_Grain(v);
		}
	}
	str_Sound_Work.str_VOICE_Tab[v].u16_cpt_buffersrc = u16_Cpt_BufferSrc;
	str_Sound_Work.str_VOICE_Tab[v].u16_cpt_bufferdst = u16_Cpt_BufferDst;
	str_Sound_Work.str_VOICE_Tab[v].i16_srcsample=srcsample;
}

/**************************************************/
/* Init voice                                     */
/* TEST FOR PITCH SHIFTING                        */
/*                                                */
/*                                                */
/**************************************************/
void Audio_Init_Voice()
{
	for(uint8_t v=0;v<GRAIN_MAX_POLY;v++)
	{
		str_Sound_Work.str_VOICE_Tab[v].u32_speed=1000;
		str_Sound_Work.str_VOICE_Tab[v].u32_cumulspeed=0;
		str_Sound_Work.str_VOICE_Tab[v].d_speed=1.0;
		str_Sound_Work.str_ADSR_AMP_onevoice[v].i16_vol=500;
		str_Sound_Work.str_VOICE_Tab[v].d_cumulspeed=0.0;
		str_Sound_Work.str_VOICE_Tab[v].u8_actif=0;
		str_Sound_Work.str_VOICE_Tab[v].u16_cpt_buffersrc=0;
		str_Sound_Work.str_VOICE_Tab[v].i16_srcsample = AUDIO_OUT_BUFFER_SIZE/8;
		Midi_Tab_Note[v]=0xFF;
	}
	str_Sound_Work.str_ADSR_AMP.u16_attack = 100;
	str_Sound_Work.str_ADSR_AMP.u16_deltaA = ADSR_MAX_AMP+30-str_Sound_Work.str_ADSR_AMP.u16_attack;
	str_Sound_Work.str_ADSR_AMP.u16_deltaA /=20;
	str_Sound_Work.str_ADSR_AMP.u16_deltaA++;

	str_Sound_Work.str_ADSR_AMP.u16_decay = 100;
	str_Sound_Work.str_ADSR_AMP.u16_deltaD = ADSR_MAX_AMP+30-str_Sound_Work.str_ADSR_AMP.u16_decay;
	str_Sound_Work.str_ADSR_AMP.u16_deltaD /=20;
	str_Sound_Work.str_ADSR_AMP.u16_deltaD++;

	str_Sound_Work.str_ADSR_AMP.u16_sustain =100;

	str_Sound_Work.str_ADSR_AMP.u16_release =100;
	str_Sound_Work.str_ADSR_AMP.u16_deltaR = ADSR_MAX_AMP+30-str_Sound_Work.str_ADSR_AMP.u16_release;
	str_Sound_Work.str_ADSR_AMP.u16_deltaR /=20;
	str_Sound_Work.str_ADSR_AMP.u16_deltaR++;
}

/**************************************************/
/* Get Half Buffer                                */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
void Audio_Play_Get_HalfBuffer(uint8_t pos)
{
}

/**************************************************/
/* Audio Play Process                             */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
AUDIO_ErrorTypeDef Audio_Play_File_Process()
{
	AUDIO_ErrorTypeDef audio_error = AUDIO_ERROR_NONE;

	switch(AudioState)
	{
		case AUDIO_STATE_PLAY:
		if(BufferCtl.fptr >= BufferCtl.fptrend)
		{
			// We are in the file view
			if(u8_Waveplayer_Mode!=WAVEPLAY_MODE_BIGBUFFERVIEW)
			{
				BufferCtl.fptr = BufferCtl.fptrstart;
				f_lseek(&WavFile,BufferCtl.fptr);
				u32_Grain_PosOffset_BigBuffer=0;
				if(u8_Waveplayer_Mode==WAVEPLAY_MODE_BIGBUFFERINFILEREC)
				{
					u8_Waveplayer_Mode=WAVEPLAY_MODE_BIGBUFFERINFILEFULL;
				}
			}
		}
		break;
	}
	return audio_error;
}

/**
  * @brief  Initializes Audio Interface.
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_PLAYER_Init(void)
{
  if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, uwVolume, I2S_AUDIOFREQ_48K) == 0)
  {
    return AUDIO_ERROR_NONE;
  }
  else
  {
    return AUDIO_ERROR_IO;
  }
}

AUDIO_ErrorTypeDef AUDIO_PLAYER_Go()
{
	/*Adjust the Audio frequency */
	PlayerInit(44100);
	BufferCtl.state = BUFFER_OFFSET_NONE;
	u8_Waveplayer_Mode=WAVEPLAY_MODE_BIGBUFFERVIEW;
	AudioState = AUDIO_STATE_PLAY;

	memcpy(&u8_Tab_BigBuffer[0],&BufferCtl.buff[0],AUDIO_OUT_BUFFER_SIZE);
	if(!half_full)
		BSP_AUDIO_OUT_Play((uint16_t*)&BufferCtl.buff[0], AUDIO_OUT_BUFFER_SIZE);
	else
		BSP_AUDIO_OUT_Play((uint16_t*)&BufferCtl.buff[AUDIO_OUT_BUFFER_SIZE /2], AUDIO_OUT_BUFFER_SIZE);
}


/**
  * @brief  Starts Audio streaming.    
  * @param  idx: File index
  * @retval Audio error
  */ 
AUDIO_ErrorTypeDef AUDIO_PLAYER_Start(uint8_t idx,uint16_t* size)
{
uint32_t bytesread;
uint16_t w;

	GetFileInfo((char *)FileList.file[idx].name, &WaveFormat);

	u32_aff_echperpixel = WaveFormat.FileSize;
	u32_aff_echperpixel /=4;
	u32_aff_echperpixel /=str_Frame_Waveform.u16_aff_width;

	w = GRAIN_BIGBUFFER_SIZE/u32_aff_echperpixel;
	w /=(WaveFormat.BitPerSample/8);
	w /=WaveFormat.NbrChannels;

	if(u8_Type_BigBuffer == BUFFER_MONO)
		w *=2;

	*size=w;

	/*Adjust the Audio frequency */
	PlayerInit(WaveFormat.SampleRate);

	BufferCtl.state = BUFFER_OFFSET_NONE;

	/* Get Data from USB Flash Disk */
	f_lseek(&WavFile,0);

	/* Fill whole buffer at first time */
	if(f_read(&WavFile,&BufferCtl.buff[0],AUDIO_OUT_BUFFER_SIZE,(void *)&bytesread) == FR_OK)
	{
		AudioState = AUDIO_STATE_PLAY;
		if(bytesread != 0)
		{
			BSP_AUDIO_OUT_Play((uint16_t*)&BufferCtl.buff[0], AUDIO_OUT_BUFFER_SIZE);
			BufferCtl.fptr = bytesread;
		}
	}
	return AUDIO_ERROR_IO;
}

/**
  * @brief  Manages Audio process. 
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_PLAYER_Process(void)
{
	  uint32_t bytesread, elapsed_time;
	  AUDIO_ErrorTypeDef audio_error = AUDIO_ERROR_NONE;
	  static uint32_t prev_elapsed_time = 0xFFFFFFFF;
	  uint8_t str[16];

	  half_full = !half_full;

	  switch(AudioState)
	  {
	  	case AUDIO_STATE_PLAY:
	  	// ---------------------------------------------------------------------------------------------
	  	// Play file
	  	// ---------------------------------------------------------------------------------------------
		if(u8_Waveplayer_Mode!=WAVEPLAY_MODE_BIGBUFFERVIEW)
		{
			if(half_full)
			{
			  if(f_read(&WavFile,&BufferCtl.buff[0],AUDIO_OUT_BUFFER_SIZE/2,(void *)&bytesread) != FR_OK)
			  {
				BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
				return AUDIO_ERROR_IO;
			  }
			}

			//if(BufferCtl.state == BUFFER_OFFSET_FULL)
			if(!half_full)
			{
			  if(f_read(&WavFile,&BufferCtl.buff[AUDIO_OUT_BUFFER_SIZE /2],AUDIO_OUT_BUFFER_SIZE/2,(void *)&bytesread) != FR_OK)
			  {
				BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
				return AUDIO_ERROR_IO;
			  }
			}
			//--------------------------------------------------------------------------------------------
			// Catch the big buffer
			//--------------------------------------------------------------------------------------------
			if(u8_Waveplayer_Mode==WAVEPLAY_MODE_BIGBUFFERINFILEREC)
			{
				if(u32_Grain_PosOffset_BigBuffer<(GRAIN_BIGBUFFER_SIZE-AUDIO_OUT_BUFFER_SIZE/4))
				{
					if(u8_Type_BigBuffer == BUFFER_STEREO)
					{
						if(!half_full)
							memcpy(&u8_Tab_BigBuffer[u32_Grain_PosOffset_BigBuffer],&BufferCtl.buff[AUDIO_OUT_BUFFER_SIZE /2],AUDIO_OUT_BUFFER_SIZE/2);
						else
							memcpy(&u8_Tab_BigBuffer[u32_Grain_PosOffset_BigBuffer],&BufferCtl.buff[0],AUDIO_OUT_BUFFER_SIZE/2);
						u32_Grain_PosOffset_BigBuffer += bytesread;
					}
					else
					{
						// Copy only the left channel
						if(!half_full)
						{
							for(int i=0;i<AUDIO_OUT_BUFFER_SIZE/2;i+=2)
							{
								*(int16_t*)&u8_Tab_BigBuffer[u32_Grain_PosOffset_BigBuffer+i] = (*(int16_t*)&BufferCtl.buff[(AUDIO_OUT_BUFFER_SIZE/2)+i*2]);
								//*(int16_t*)&u8_Tab_BigBuffer[u32_Grain_PosOffset_BigBuffer+i] = 0x0000;
							}
						}
						else
						{
							for(int i=0;i<AUDIO_OUT_BUFFER_SIZE/2;i+=2)
							{
								*(int16_t*)&u8_Tab_BigBuffer[u32_Grain_PosOffset_BigBuffer+i] = (*(int16_t*)&BufferCtl.buff[+i*2]);
								//*(int16_t*)&u8_Tab_BigBuffer[u32_Grain_PosOffset_BigBuffer+i] = 0x0000;
							}
						}
						u32_Grain_PosOffset_BigBuffer += AUDIO_OUT_BUFFER_SIZE/4;
					}
				}
			}
		}
		// ---------------------------------------------------------------------------------------------
	  	// Play Big buffer
	  	// ---------------------------------------------------------------------------------------------
		else
		{
			memset(&BufferCtl.buff[half_full*AUDIO_OUT_BUFFER_SIZE/2],0,AUDIO_OUT_BUFFER_SIZE/2);
			for(uint8_t v=0;v<GRAIN_MAX_POLY;v++)
			{
				//if(str_Sound_Work.str_VOICE_Tab[v].u8_actif)
				if(Midi_Tab_Note[v] !=0xFF && Midi_Tab_Note[v] !=0xFE)
					Audio_Add_Voice(v,half_full);
			}
		}
		BufferCtl.fptr += bytesread;
	    break;

	  case AUDIO_STATE_STOP:
	    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
	    AudioState = AUDIO_STATE_IDLE;
	    audio_error = AUDIO_ERROR_IO;
	    break;

	  case AUDIO_STATE_NEXT:
	    break;

	  case AUDIO_STATE_PREVIOUS:
	    break;

	  case AUDIO_STATE_PAUSE:
	    BSP_AUDIO_OUT_Pause();
	    AudioState = AUDIO_STATE_WAIT;
	    break;

	  case AUDIO_STATE_RESUME:
	    BSP_AUDIO_OUT_Resume();
	    AudioState = AUDIO_STATE_PLAY;
	    break;

	  case AUDIO_STATE_VOLUME_UP:
	    if( uwVolume <= 90)
	    {
	      uwVolume += 10;
	    }
	    BSP_AUDIO_OUT_SetVolume(uwVolume);
//	    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
//	    sprintf((char *)str,  "Volume : %lu ", uwVolume);
//	    BSP_LCD_DisplayStringAtLine(9, str);
	    AudioState = AUDIO_STATE_PLAY;
	    break;

	  case AUDIO_STATE_VOLUME_DOWN:
	    if( uwVolume >= 10)
	    {
	      uwVolume -= 10;
	    }
	    BSP_AUDIO_OUT_SetVolume(uwVolume);
//	    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
//	    sprintf((char *)str,  "Volume : %lu ", uwVolume);
//	    BSP_LCD_DisplayStringAtLine(9, str);
	    AudioState = AUDIO_STATE_PLAY;
	    break;

	  case AUDIO_STATE_WAIT:
	  case AUDIO_STATE_IDLE:
	  case AUDIO_STATE_INIT:
	  default:
	    /* Update audio state machine according to touch acquisition */
	    //AUDIO_AcquireTouchButtons();
	    break;
	  }
	  return audio_error;

}

/**
  * @brief  Calculates the remaining file size and new position of the pointer.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	BSP_AUDIO_OUT_ChangeBuffer((uint16_t*)BufferCtl.buff[0], AUDIO_OUT_BUFFER_SIZE /2);
}

/**
  * @brief  Manages the DMA Half Transfer complete interrupt.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	BSP_AUDIO_OUT_ChangeBuffer((uint16_t*)BufferCtl.buff[AUDIO_OUT_BUFFER_SIZE /2], AUDIO_OUT_BUFFER_SIZE /2);
}
/*******************************************************************************
                            Static Functions
*******************************************************************************/

AUDIO_ErrorTypeDef SoundSave(char* name)
{
	UINT counter;
	uint16_t block;
	uint16_t rest;
	uint16_t b;
	if(f_open(&SndFile,name,FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
	{
		// Write the Display uint16_t	Tab_AffBigBuffer[AFF_FILE_MAXPIXEL]; 800*2 bytes
		f_write(&SndFile,&Tab_AffBigBuffer[0],sizeof(Tab_AffBigBuffer),&counter);

		// Write the Sound
		rest = &str_Sound_Work.endsave - &str_Sound_Work.beginsave;
		// TROP DE DATA !!!!!!!!
		f_write(&SndFile,&str_Sound_Work,rest,&counter);

		// Write the Matrix Tab_Matrix_Val
		f_write(&SndFile,&Tab_Matrix_Val,sizeof(Tab_Matrix_Val),&counter);

		// Write the BIGBUFFER GRAIN_BIGBUFFER_SIZE
		block = GRAIN_BIGBUFFER_SIZE/16000;
		for(b=0;b<block;b++)
		{
			f_write(&SndFile,&u8_Tab_BigBuffer[b*16000],16000,&counter);
		}
		// write the rest
		rest = GRAIN_BIGBUFFER_SIZE - block*16000;
		f_write(&SndFile,&u8_Tab_BigBuffer[b*16000],rest,&counter);

		f_write(&SndFile,&Tab_Val[0],sizeof(Tab_Val),&counter);
		f_write(&SndFile,&Tab_Circle[0],sizeof(Tab_Circle),&counter);

		f_close(&SndFile);
		return AUDIO_ERROR_NONE;
	}
	return AUDIO_ERROR_IO;
}

AUDIO_ErrorTypeDef SoundRead(char* name)
{
	UINT counter;
	uint16_t block;
	uint16_t rest;
	uint16_t b;
	if(f_open(&SndFile,name,FA_READ) == FR_OK)
	{
		// Read the Display uint16_t	Tab_AffBigBuffer[AFF_FILE_MAXPIXEL]; 800*2 bytes
		f_read(&SndFile,&Tab_AffBigBuffer[0],sizeof(Tab_AffBigBuffer),&counter);

		// Read the Sound
		rest = &str_Sound_Work.endsave - &str_Sound_Work.beginsave;
		f_read(&SndFile,&str_Sound_Work,rest,&counter);

		// Read the Marix Tab_Matrix_Val
		f_read(&SndFile,&Tab_Matrix_Val,sizeof(Tab_Matrix_Val),&counter);

		// Read the BIGBUFFER GRAIN_BIGBUFFER_SIZE
		block = GRAIN_BIGBUFFER_SIZE/16000;
		for(b=0;b<block;b++)
		{
			f_read(&SndFile,&u8_Tab_BigBuffer[b*16000],16000,&counter);
		}
		// Read the rest
		rest = GRAIN_BIGBUFFER_SIZE - block*16000;
		f_read(&SndFile,&u8_Tab_BigBuffer[b*16000],rest,&counter);

		f_read(&SndFile,&Tab_Val[0],sizeof(Tab_Val),&counter);
		f_read(&SndFile,&Tab_Circle[0],sizeof(Tab_Circle),&counter);


		f_close(&SndFile);
		return AUDIO_ERROR_NONE;
	}
	return AUDIO_ERROR_IO;
}



/**
  * @brief  Gets the file info.
  * @param  file_idx: File index
  * @param  info: Pointer to WAV file info
  * @retval Audio error
  */
AUDIO_ErrorTypeDef GetFileInfo(char* filename, WAVE_FormatTypeDef *info)
{
	 uint32_t bytesread;
	  f_close(&WavFile);
	  if(f_open(&WavFile,filename, FA_OPEN_EXISTING | FA_READ) == FR_OK)
	  {
	    /* Fill the buffer to Send */
	    if(f_read(&WavFile, info, sizeof(WaveFormat), (void *)&bytesread) == FR_OK)
	    {
	      return AUDIO_ERROR_NONE;
	    }
	    f_close(&WavFile);
	  }
	  return AUDIO_ERROR_IO;
}

/**
  * @brief  Initializes the Wave player.
  * @param  AudioFreq: Audio sampling frequency
  * @retval None
  */
uint8_t PlayerInit(uint32_t AudioFreq)
{ 
  /* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */  
  if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, uwVolume, AudioFreq) != 0)
  {
    return 1;
  }
  else
  {
    BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
    return 0;
  } 
}

/***********************************************************************/
/*
  * @brief  Stops Audio streaming.
  * @param  None
  * @retval Audio error
/***********************************************************************/
AUDIO_ErrorTypeDef AUDIO_PLAYER_Stop(void)
{
AudioState = AUDIO_STATE_STOP;
FilePos = 0;

	BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
	//f_close(&WavFile);
	return AUDIO_ERROR_NONE;
}

AUDIO_ErrorTypeDef AUDIO_PLAYER_Pause()
{
	BSP_AUDIO_OUT_Pause();
}

AUDIO_ErrorTypeDef AUDIO_PLAYER_Resume()
{
	BSP_AUDIO_OUT_Resume();
}

/**
  * @brief  Display interface touch screen buttons
  * @param  None
  * @retval None
  */
static void AUDIO_PlaybackDisplayButtons(void)
{
}

/**
  * @brief  Test touch screen state and modify audio state machine according to that
  * @param  None
  * @retval None
  */
static void AUDIO_AcquireTouchButtons(void)
{
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
