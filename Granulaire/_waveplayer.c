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

#include "C:/TouchGFXProjects/MyApplication/Granulaire/Matrix.h"

extern RNG_HandleTypeDef hrng;
extern uint32_t gui_Random_Number;
extern xSemaphoreHandle 	xSemDmaSai;

extern FIL WavFile;
extern FILELIST_FileTypeDef FileList;

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

AUDIO_PLAYBACK_StateTypeDef AudioState;

static int16_t FilePos = 0;
static __IO uint32_t uwVolume = 70;

//WAVE_FormatTypeDef WaveFormat;
FIL WavFile;
//extern FILELIST_FileTypeDef FileList;

/* Private function prototypes -----------------------------------------------*/
static uint8_t PlayerInit(uint32_t AudioFreq);
static void AUDIO_PlaybackDisplayButtons(void);
static void AUDIO_AcquireTouchButtons(void);

/* Private functions ---------------------------------------------------------*/

uint32_t posfile;

uint8_t bufftmp[AUDIO_OUT_BUFFER_SIZE];			// Temp buffer to appli the ENV AR
uint8_t buffcumul[AUDIO_OUT_BUFFER_SIZE*2];		//
//uint8_t bufftranspose[AUDIO_OUT_BUFFER_SIZE*2];


/**************************************************/
/* Audio Play File                                */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
AUDIO_ErrorTypeDef Audio_Play_File(uint8_t idx,uint32_t startpos)
{
}

/*******************************************************************/
/* FOR THE ENV AR FOR EACH GRAIN								   */
/*******************************************************************/
//	u16_cptAR from 0 to u16_size
//  vol = (u16_cptAR*2*100)/(Attack*NbSlice) evolution vol a la fin d'un slice
//
//  Pour un slice de AUDIO_OUT_BUFFER_SIZE
//  Delta = (2*100)/(Attack*NbSlice)
//
//	Pour un demi slide -> D2 = 100/(Attack*NbSlice)
//  i*D2/AUDIO_OUT_BUFFER_SIZE
//
//	for(i=0;i<AUDIO_OUT_BUFFER_SIZE/2;i+=4)
//	{
//		*(int16_t*)&BufferCtl.buff[0+i] = (*(int16_t*)&bufftmp[0+i])/1;
//		*(int16_t*)&BufferCtl.buff[2+i] = (*(int16_t*)&bufftmp[2+i])/1;
//	}
//
//

/**************************************************/
/* AR ENV                                         */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
float Audio_Env_Grain(uint8_t grain)
{
float t=1;

	switch(str_Sound_Work.str_tabgrain[grain].u8_stateAR)
	{
		case GRAIN_AR_STATE_ATTACK:
		if(str_Sound_Work.str_tabgrain[grain].u32_volA<100000)
			str_Sound_Work.str_tabgrain[grain].u32_volA +=str_Sound_Work.u32_deltaA;
		else
		{
			str_Sound_Work.str_tabgrain[grain].u32_volA = 90000;
			str_Sound_Work.str_tabgrain[grain].u8_stateAR=GRAIN_AR_STATE_SUSTAIN;
		}
		t = (float)str_Sound_Work.str_tabgrain[grain].u32_volA/100000;
		break;

		case GRAIN_AR_STATE_SUSTAIN:
		str_Sound_Work.str_tabgrain[grain].s32_volR = 100000;
		// Trig the Release
		if(str_Sound_Work.str_tabgrain[grain].u16_cptAR > str_Sound_Work.u16_TrigR)
		{
			str_Sound_Work.str_tabgrain[grain].u8_stateAR = GRAIN_AR_STATE_RELEASE;
		}
		t=1;
		break;

		case GRAIN_AR_STATE_RELEASE:
		if(str_Sound_Work.str_tabgrain[grain].s32_volR>str_Sound_Work.u32_deltaR*2)
		{
			str_Sound_Work.str_tabgrain[grain].s32_volR -=str_Sound_Work.u32_deltaR;
			t = (float)str_Sound_Work.str_tabgrain[grain].s32_volR/100000;
		}
		else
		{
			str_Sound_Work.str_tabgrain[grain].u8_stateAR=GRAIN_AR_STATE_STANDBY;
			//str_Sound_Work.str_tabgrain[grain].u8_stateAR=GRAIN_AR_STATE_ATTACK;
			//str_Sound_Work.str_tabgrain[grain].u16_cptAR=0;
			str_Sound_Work.str_tabgrain[grain].u32_volA=0;
			str_Sound_Work.str_tabgrain[grain].s32_volR=0;
			t=0;
		}
		break;

		case GRAIN_AR_STATE_STANDBY:
		t=0;
		break;
	}
//	if(str_Sound_Work.str_tabgrain[grain].u16_cptAR > u32_Grain_Slice)
//	{
//		str_Sound_Work.str_tabgrain[grain].u16_cptAR =0;
//		str_Sound_Work.str_tabgrain[grain].u8_stateAR=GRAIN_AR_STATE_STANDBY;
//	}
	return(t);
}

//***********************************************************
// LIN
//***********************************************************
#define SCALE    65536

// For linear interpolation
int32_t iRate;
int32_t iFract=0;

void ResetRegistersLin()
{
    iFract = 0;
}

void SetRateLin(double newRate)
{
    iRate = (int32_t)(newRate * SCALE + 0.5);
}

static const float _coeffs[]=
{ -0.5f,  1.0f, -0.5f, 0.0f,
   1.5f, -2.5f,  0.0f, 1.0f,
  -1.5f,  2.0f,  0.5f, 0.0f,
   0.5f, -0.5f,  0.0f, 0.0f};
double fract;
double rate=2.0;
int TransposeMonoCubic(int16_t *pdest,int16_t *psrc,int srcSamples)
{
    int i;
    int srcSampleEnd = srcSamples - 4;
    int srcCount = 0;

    i = 0;
    while (srcCount < srcSampleEnd)
    {
        float out;
        const float x3 = 1.0f;
        const float x2 = (float)fract;    // x
        const float x1 = x2*x2;           // x^2
        const float x0 = x1*x2;           // x^3
        float y0, y1, y2, y3;


        y0 =  _coeffs[0] * x0 +  _coeffs[1] * x1 +  _coeffs[2] * x2 +  _coeffs[3] * x3;
        y1 =  _coeffs[4] * x0 +  _coeffs[5] * x1 +  _coeffs[6] * x2 +  _coeffs[7] * x3;
        y2 =  _coeffs[8] * x0 +  _coeffs[9] * x1 + _coeffs[10] * x2 + _coeffs[11] * x3;
        y3 = _coeffs[12] * x0 + _coeffs[13] * x1 + _coeffs[14] * x2 + _coeffs[15] * x3;

        out = y0 * psrc[0] + y1 * psrc[1] + y2 * psrc[2] + y3 * psrc[3];

        pdest[i] = (int)out;
        i ++;

        // update position fraction
        fract += rate;
        // update whole positions
        int whole = (int)fract;
        fract -= whole;
        psrc += whole;
        srcCount += whole;
    }
    srcSamples = srcCount;
    return i;
}



int16_t TransposeMonoLin(int16_t *dest,int16_t *src,int16_t srcsmp)
{
	int16_t i;
	//int16_t srcSampleEnd = srcsmp-1;
	int16_t srcSampleEnd = srcsmp;
	int16_t srcCount = 0;
	int16_t *savsrc;
	int16_t cumul=0;

    savsrc=src;
    i = 0;
    while(srcCount<srcSampleEnd)
    {
        long temp;

        temp = (SCALE - iFract) * src[0] + iFract * src[2];
        dest[i] = (int16_t)(temp / SCALE);
        i++;

        iFract += iRate;

        int iWhole = iFract / SCALE;
        if(iWhole && i<(srcsmp))
            cumul++;
        iFract -= iWhole * SCALE;
        //srcCount += iWhole;
        srcCount++;
        src += iWhole;
    }
    srcsmp = srcCount;
    return cumul+1;
}


double speed=1.5;
double rest=0;
double whole=0;
double cumulspeed=0;
uint16_t cptdest=0;

int MyTransposeStereo(int16_t *dest,int16_t *src,int16_t sizedest,int16_t* ptsrcSamples)
{
uint16_t srcCount=0;
uint16_t srcSampleEnd = *ptsrcSamples-0;

    while(srcCount < srcSampleEnd)
    {
        dest[0] = src[0];
        dest[1] = src[1];
        dest+=2;
        cptdest++;
        cumulspeed +=speed;
        whole = (int16_t)cumulspeed;

        src += 2*(uint16_t)whole;
        rest = cumulspeed - whole;
        cumulspeed = rest;
        srcCount += (uint16_t)whole;

        //--------------------------------
		// The OUTPUT buffer is full
		//--------------------------------
		if(cptdest>=sizedest)
		{
			cptdest=0;
			break;
		}

    }
    //-----------------------------------------------
	// ptsrcSamples is the rest of the INPUT buffer
	//-----------------------------------------------
	if(srcCount==*ptsrcSamples)
	{
		*ptsrcSamples = 0;
	}
	else
		*ptsrcSamples -= srcCount;

	if(*ptsrcSamples<0) *ptsrcSamples=0;

	return cptdest;
}




/**************************************************/
/* Transpose stereo                               */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint16_t TransposeStereo(int16_t *dest,int16_t *src,int16_t sizedest,int16_t* ptsrcSamples)
{
	uint16_t i;
	uint16_t srcSampleEnd = *ptsrcSamples-0;
	uint16_t srcCount = 0;
	static uint16_t cumul=0;


	i = 0;
	while (srcCount < srcSampleEnd)
	{
		long temp0;
		long temp1;

		temp0 = (SCALE - iFract) * src[0] + iFract * src[2];
		temp1 = (SCALE - iFract) * src[1] + iFract * src[3];
		dest[0] = (int16_t)(temp0 / SCALE);
		dest[1] = (int16_t)(temp1 / SCALE);
		dest += 2;
		i++;

		// For half speed  0.5 iRate=scale/2
		// So iwhole=1 every two pass

		iFract += iRate;

		int iWhole = iFract / SCALE;
		if(iWhole)
			cumul++;
		//--------------------------------
		// The OUTPUT buffer is full
		//--------------------------------
		if(i>=sizedest)
		{
			break;
		}

		iFract -= iWhole * SCALE;
		srcCount += iWhole;
		src += 2*iWhole;			// Point to the next L/R samples
	}
	//-----------------------------------------------
	// ptsrcSamples is the rest of the INPUT buffer
	//-----------------------------------------------
	if(srcCount==*ptsrcSamples)
	{
		*ptsrcSamples = 0;
	}
	else
		*ptsrcSamples -= srcCount+1;

	return i;
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
/**************************************************/
/* Generate Grain                                 */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
void Generate_Grain()
{
	uint16_t i;
	float t;

	// Buffer for test
	if(0)
	{
		unsigned char k=0;
		for(i=0;i<AUDIO_OUT_BUFFER_SIZE/2;i+=4)
		{
			*(int16_t*)&buffcumul[0+i] = 0x0001;
			*(int16_t*)&buffcumul[2+i] = 0x0002+k;
			k++;
		}
	}

	// Generate a SAW who fit in the buffer direct in the buff cumul OK
	if(0)
	{
		uint16_t inc = 16384/(AUDIO_OUT_BUFFER_SIZE/2);
		inc *=16;
		uint16_t add=0;
		for(i=0;i<AUDIO_OUT_BUFFER_SIZE;i+=4)
		{
			add +=inc;
			*(int16_t*)&buffcumul[0+i] = 0+add;		//-16000 + add;
			*(int16_t*)&buffcumul[2+i] = 0+add;		//-16000 + add;
			if(add > 16384)
				add=0;
		}
		return;
	}

	// Generate a SAW who do not fit in the buffer direct in the buff cumul OK
	static uint16_t phase=0;
	if(0)
	{
		uint16_t inc = 16000/1500;
		inc *=32;
		uint16_t add=0;
		for(i=0;i<AUDIO_OUT_BUFFER_SIZE;i+=4)
		{
			add =phase*inc;
			*(int16_t*)&buffcumul[0+i] = 0+add;		//-16000 + add;
			*(int16_t*)&buffcumul[2+i] = 0+add;		//-16000 + add;
			phase++;
			if(phase> 250)
				phase=0;
		}
		return;
	}

	//-----------------------------
	// OK Valide le 05.05.2020
	// good one to test the click
	//-----------------------------
	if(0)
	{
		// Generate a TRI in the bigbuffer
		int16_t inc = 16384/(AUDIO_OUT_BUFFER_SIZE/2);
		inc *=32;

		for(i=0;i<AUDIO_OUT_BUFFER_SIZE/2;i+=4)
		{
			addtri +=inc*signe;
			*(int16_t*)&buffcumul[0+i]=addtri;
			*(int16_t*)&buffcumul[2+i]=addtri;
			if(addtri > 16000)
			{
				signe = -1;
			}
			if(addtri <= -16000)
			{
				signe = +1;
			}
		}
		return;
	}

	//-----------------------------
	// OK Valide le 05.05.2020
	// Play the file
	// Re test and OK the 06.12.20
	//-----------------------------
	static uint16_t cpt=0;
	if(0)
	{
		posstart=(cpt*AUDIO_OUT_BUFFER_SIZE)/4;
		cpt++;
		posstart >>=u8_Type_BigBuffer;
		posstart <<=u8_Type_BigBuffer;
		memcpy(&bufftmp[0],&u8_Tab_BigBuffer[posstart],AUDIO_OUT_BUFFER_SIZE/4/*+16*/);		//

		for(i=0;i<AUDIO_OUT_BUFFER_SIZE/2/*+8*/;i+=4)
		{
			*(int16_t*)&buffcumul[0+i] = (*(int16_t*)&bufftmp[0+i/2]);
			*(int16_t*)&buffcumul[2+i] = (*(int16_t*)&bufftmp[0+i/2]);
		}
		if(posstart>GRAIN_BIGBUFFER_SIZE-2*AUDIO_OUT_BUFFER_SIZE)
			cpt=0;
		return;
	}

	if(0)
	{
		uint8_t g;
		memset(&buffcumul[0],0,AUDIO_OUT_BUFFER_SIZE*2);
		posstart=str_Sound_Work.str_tabgrain[g].u32_beginpos+(cpt*AUDIO_OUT_BUFFER_SIZE/4);
		cpt++;
		posstart >>=u8_Type_BigBuffer;
		posstart <<=u8_Type_BigBuffer;
		memcpy(&bufftmp[0],&u8_Tab_BigBuffer[posstart],AUDIO_OUT_BUFFER_SIZE/4);

		t=Audio_Env_Grain(g);

		int divint;
		divint = 1000*((ui32_PowerBuffer/2)+1);
		divint += 120*(str_Sound_Work.u8_grainnb);

		for(i=0;i<AUDIO_OUT_BUFFER_SIZE/2;i+=4)
		{
			*(int16_t*)&buffcumul[0+i] += (*(int16_t*)&bufftmp[0+i/2])*t*str_Sound_Work.str_tabgrain[g].u16_panleft/divint;	// 2000 -12db
			*(int16_t*)&buffcumul[2+i] += (*(int16_t*)&bufftmp[0+i/2])*t*str_Sound_Work.str_tabgrain[g].u16_panright/divint;
		}
		if(posstart>GRAIN_BIGBUFFER_SIZE-2*AUDIO_OUT_BUFFER_SIZE)
			cpt=0;
		return;
	}


	memset(&buffcumul[0],0,AUDIO_OUT_BUFFER_SIZE*2);
	off=str_Sound_Work.u32_offsetnextgrain;

	for(uint8_t g=0;g<str_Sound_Work.u8_grainnb;g++)
	{
		if(str_Sound_Work.str_tabgrain[g].u8_stateAR!=GRAIN_AR_STATE_STANDBY)
		{
			//--------------------------------------------------------------------------------------------
			// Read a part of the big buffer
			//--------------------------------------------------------------------------------------------

			if(u8_Type_BigBuffer == BUFFER_STEREO)
			{
				posstart=str_Sound_Work.str_tabgrain[g].u32_beginpos+(str_Sound_Work.str_tabgrain[g].u16_cptAR-1)*AUDIO_OUT_BUFFER_SIZE/2;
				posstart >>=u8_Type_BigBuffer;
				posstart <<=u8_Type_BigBuffer;
				if(posstart>GRAIN_BIGBUFFER_SIZE-str_Sound_Work.str_tabgrain[g].u32_size)
					posstart=GRAIN_BIGBUFFER_SIZE-str_Sound_Work.str_tabgrain[g].u32_size;
				memcpy(&bufftmp[0],&u8_Tab_BigBuffer[posstart],AUDIO_OUT_BUFFER_SIZE/2);
			}
			else
			{
				//posstart=str_Sound_Work.str_tabgrain[g].u32_beginpos+(str_Sound_Work.str_tabgrain[g].u16_cptAR-1)*AUDIO_OUT_BUFFER_SIZE/4;
				posstart=str_Sound_Work.str_tabgrain[g].u32_beginpos+(cpt)*AUDIO_OUT_BUFFER_SIZE/4;	// ONLY FOR ONE GRAIN
				cpt++;

				posstart >>=u8_Type_BigBuffer;
				posstart <<=u8_Type_BigBuffer;
				if(posstart>GRAIN_BIGBUFFER_SIZE-str_Sound_Work.str_tabgrain[g].u32_size)
					posstart=GRAIN_BIGBUFFER_SIZE-str_Sound_Work.str_tabgrain[g].u32_size;
				memcpy(&bufftmp[0],&u8_Tab_BigBuffer[posstart],AUDIO_OUT_BUFFER_SIZE/4);

				if(posstart>str_Sound_Work.str_tabgrain[g].u32_size-2*AUDIO_OUT_BUFFER_SIZE)
					cpt=0;

			}
			//--------------------------------------------------------------------------------------------
			// Apply the AR Env to the buffer cumul
			//--------------------------------------------------------------------------------------------
			t=Audio_Env_Grain(g);
			//t=1; // Bypass the EG

			// ui32_PowerBuffer from 0 low level to 15
			// nb Grain up to 32

			int divint;
//			if(ui32_PowerBuffer>10)
//				divint =   2000+ 30*((str_Sound_Work.u8_grainnb)*ui32_PowerBuffer);
//			else
//				divint = 1000+ 60*((str_Sound_Work.u8_grainnb)*ui32_PowerBuffer);

			divint = 1000*((ui32_PowerBuffer/2)+1);
			divint += 120*(str_Sound_Work.u8_grainnb);

			if(u8_Type_BigBuffer == BUFFER_STEREO)
			{
				for(i=0;i<AUDIO_OUT_BUFFER_SIZE/2+8;i+=4)
				{
					*(int16_t*)&buffcumul[0+i] += (*(int16_t*)&bufftmp[0+i])*t*dv;	// Left
					*(int16_t*)&buffcumul[2+i] += (*(int16_t*)&bufftmp[2+i])*t*dv;	// Right
				}
			}
			else
			{
				// Rebuil a pseudo stereo and apply the pan
				for(i=0;i<AUDIO_OUT_BUFFER_SIZE/2;i+=4)
				{
					*(int16_t*)&buffcumul[0+i] += (*(int16_t*)&bufftmp[0+i/2])*t*str_Sound_Work.str_tabgrain[g].u16_panleft/divint;	// 2000 -12db
					*(int16_t*)&buffcumul[2+i] += (*(int16_t*)&bufftmp[0+i/2])*t*str_Sound_Work.str_tabgrain[g].u16_panright/divint;
				}
			}
			//--------------------------------------------------------------------------------------------
			// Trig the Next Grain ?
			//--------------------------------------------------------------------------------------------
			uint8_t g_actif=0;
			if(str_Sound_Work.str_tabgrain[g].u16_cptAR > str_Sound_Work.u8_Grain_NextTrig)
			{
				// Not the last Grain
				if(g<str_Sound_Work.u8_grainnb-1)
				{
					g_actif = g+1;
					if(str_Sound_Work.str_tabgrain[g_actif].u8_stateAR==GRAIN_AR_STATE_STANDBY)
					{
						uint32_t rand;
						if(!str_Sound_Work.u8_spaceranfixe)
						{
							HAL_RNG_GenerateRandomNumber(&hrng,&rand);
							str_Sound_Work.u32_offsetnextgrain=rand;
							str_Sound_Work.u32_offsetnextgrain &= 0x1FFF*str_Sound_Work.u8_rangeshift;	// Set the range of the random
							str_Sound_Work.u32_offsetnextgrain +=4000;
							//str_Sound_Work.u32_offsetnextgrain >>=4;
							//str_Sound_Work.u32_offsetnextgrain <<=4;
						}
						else
						{
							str_Sound_Work.u32_offsetnextgrain = 4000+g*str_Sound_Work.u8_rangeshift*200; // max 4000+20000
						}


						str_Sound_Work.str_tabgrain[g_actif].u32_beginpos = u32_Grain_Begin+u32_Mod_Grain_Begin+str_Sound_Work.u32_offsetnextgrain;

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
						str_Sound_Work.str_tabgrain[g_actif].u16_panleft 	= panleft;
						str_Sound_Work.str_tabgrain[g_actif].u16_panright 	= panright;

						str_Sound_Work.str_tabgrain[g_actif].u8_stateAR=GRAIN_AR_STATE_ATTACK;
						str_Sound_Work.str_tabgrain[g_actif].u16_cptAR=0;
					}
				}
				// Last Grain
				else
				{
					if(str_Sound_Work.str_tabgrain[0].u8_stateAR==GRAIN_AR_STATE_STANDBY)
					{
						g_actif =0;
						str_Sound_Work.str_tabgrain[0].u8_stateAR=GRAIN_AR_STATE_ATTACK;
						str_Sound_Work.str_tabgrain[0].u16_cptAR=0;

						str_Sound_Work.str_tabgrain[0].u32_beginpos = u32_Grain_Begin+u32_Mod_Grain_Begin;
						if(str_Sound_Work.u8_AR_change== TRUE)
						{
							Grain_Set_AR();
							str_Sound_Work.u8_AR_change=FALSE;
						}
						pantoogle = !pantoogle;
						if(pantoogle)
						{
							panleft 	= 1000 +  str_Sound_Work.u8_pan*1000/100;
							panright	= 1000 - str_Sound_Work.u8_pan*1000/100;
						}
						else
						{
							panright 	= 1000 +  str_Sound_Work.u8_pan*1000/100;
							panleft		= 1000 - str_Sound_Work.u8_pan*1000/100;
						}
						str_Sound_Work.str_tabgrain[g_actif].u16_panleft 	= panleft;
						str_Sound_Work.str_tabgrain[g_actif].u16_panright 	= panright;
					}
				}
			}
			if(str_Sound_Work.str_tabgrain[g_actif].u32_beginpos >(GRAIN_BIGBUFFER_SIZE-u32_Grain_Size))
			{
				str_Sound_Work.str_tabgrain[g_actif].u32_beginpos=GRAIN_BIGBUFFER_SIZE-u32_Grain_Size;
			}
		}
	}// end for grain
}


uint16_t srcsample = AUDIO_OUT_BUFFER_SIZE/8;			// /4 For mono ok /8 Ok for stereo

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
	  if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, uwVolume, I2S_AUDIOFREQ_48K) == 0)
	  {
	    return AUDIO_ERROR_NONE;
	  }
	  else
	  {
	    return AUDIO_ERROR_IO;
	  }
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
			return AUDIO_ERROR_NONE;
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
			ResetRegistersLin();
			SetRateLin(1.0);
			//Generate_Grain();
		}
		// ---------------------------------------------------------------------------------------------
	  	// Play Big buffer
	  	// ---------------------------------------------------------------------------------------------
		else
		{
			//------------------------------------------
			// Play only the bigbuffer mono
			// For test purpose
			//------------------------------------------
			if(0)
			{
				posstart=cptit*AUDIO_OUT_BUFFER_SIZE/4;
				cptit++;	// Max 172
				if(posstart>GRAIN_BIGBUFFER_SIZE-AUDIO_OUT_BUFFER_SIZE/4)
				{
					posstart=0;
					cptit=0;
				}
				for(int i=0;i<AUDIO_OUT_BUFFER_SIZE/2;i+=4)
				{
					*(int16_t*)&BufferCtl.buff[half_full*AUDIO_OUT_BUFFER_SIZE/2+0+i] = (*(int16_t*)&u8_Tab_BigBuffer[posstart+i/2]);
					*(int16_t*)&BufferCtl.buff[half_full*AUDIO_OUT_BUFFER_SIZE/2+2+i] = (*(int16_t*)&u8_Tab_BigBuffer[posstart+i/2]);
				}
				return;
			}

			//-----------------------------------------------------------
			// Inc all the AR EG for all the grains
			//-----------------------------------------------------------
			for(uint8_t grain=0;grain<str_Sound_Work.u8_grainnb;grain++)
			{
				str_Sound_Work.str_tabgrain[grain].u16_cptAR++;
			}
			uint16_t vol = str_Sound_Work.str_ADSR_AMP_onevoice[0].i16_vol;
			vol= 1000;

			dv = (float)vol/1000.0;

			// Interpolation lineaire here
			// AUDIO_BUFFER_SIZE				4096 bytes
			// AUDIO_BUFFER_SIZE/2				2048 bytes demi buffer pour les alternances HALF/FULL du DMA
			// AUDIO_BUFFER_SIZE/4				1024 Car c'est des entiers
			// AUDIO_BUFFER_SIZE/8				0512 Car c'est en stereo

			//memset(&BufferCtl.buff[pos*AUDIO_OUT_BUFFER_SIZE/2+0],0,AUDIO_OUT_BUFFER_SIZE*2);
			u16_Cpt_BufferDst=MyTransposeStereo((int16_t*)&BufferCtl.buff[half_full*AUDIO_OUT_BUFFER_SIZE/2+u16_Cpt_BufferDst*4],(int16_t*)&buffcumul[u16_Cpt_BufferSrc*4],AUDIO_OUT_BUFFER_SIZE/8,&srcsample);
			u16_Cpt_BufferSrc = (AUDIO_OUT_BUFFER_SIZE/8)-srcsample;
			if(speed <= 1.0)
			{
				//--------------------------------
				// INPUT buffer is empty
				//--------------------------------
				if(srcsample==0)
				{
					u16_Cpt_BufferSrc=0;
					srcsample = AUDIO_OUT_BUFFER_SIZE/8;
					Generate_Grain();				// Rempli 2048 bytes donc AUDIO_OUT_BUFFER_SIZE/2
					//--------------------------------
					// If the OUTPUT buffer is not full
					// complete the buffer
					// u16_Cpt_BufferDst = 512 -> Output buffer full
					// u16_Cpt_BufferDst !=512 -> rest 512-u16_Cpt_BufferDst
					//--------------------------------
					if(u16_Cpt_BufferDst != 512)
					{
						u16_Cpt_BufferDst=MyTransposeStereo((int16_t*)&BufferCtl.buff[half_full*AUDIO_OUT_BUFFER_SIZE/2+u16_Cpt_BufferDst*4],(int16_t*)&buffcumul[u16_Cpt_BufferSrc*4],AUDIO_OUT_BUFFER_SIZE/8,&srcsample);
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
					Generate_Grain();
					u16_Cpt_BufferDst=MyTransposeStereo((int16_t*)&BufferCtl.buff[half_full*AUDIO_OUT_BUFFER_SIZE/2+u16_Cpt_BufferDst*4],(int16_t*)&buffcumul[u16_Cpt_BufferSrc*4],AUDIO_OUT_BUFFER_SIZE/8,&srcsample);
					u16_Cpt_BufferSrc = (AUDIO_OUT_BUFFER_SIZE/8)-srcsample;
				}
				//--------------------------------
				// INPUT buffer is empty
				//--------------------------------
				if(srcsample==0)
				{
					u16_Cpt_BufferSrc=0;
					srcsample = AUDIO_OUT_BUFFER_SIZE/8;
					Generate_Grain();
				}
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

/**
  * @brief  Calculates the remaining file size and new position of the pointer.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	BSP_AUDIO_OUT_ChangeBuffer((uint16_t*)BufferCtl.buff[0], AUDIO_OUT_BUFFER_SIZE /2);
//	  if(AudioState == AUDIO_STATE_PLAY)
//	  {
//	    BufferCtl.state = BUFFER_OFFSET_FULL;
//		// Release the semaphore
//	  }
}

/**
  * @brief  Manages the DMA Half Transfer complete interrupt.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	BSP_AUDIO_OUT_ChangeBuffer((uint16_t*)BufferCtl.buff[AUDIO_OUT_BUFFER_SIZE /2], AUDIO_OUT_BUFFER_SIZE /2);

//	  if(AudioState == AUDIO_STATE_PLAY)
//	  {
//	    BufferCtl.state = BUFFER_OFFSET_HALF;
//	  }
}
/*******************************************************************************
                            Static Functions
*******************************************************************************/

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
	if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, uwVolume, AudioFreq) != 0)
	{
		return 1;
	}
	else
	{
		BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
		return 0;
	}
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
