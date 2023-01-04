/*
 * Grain.c
 *
 *  Created on: 19 mars 2020
 *      Author: Gilles
 */
#define __GRAIN__
#include "Grain.h"
#include "wave.h"
#include "stm32469i_discovery.h"
#include "stm32469i_discovery.h"

#include "C:/TouchGFXProjects/MyApplication/Granulaire/Matrix.h"

uint32_t test;

extern RNG_HandleTypeDef hrng;

/**************************************************/
/* Grain Inc the LFO                              */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
void Grain_Set_LFO()
{
	uint32_t rand;
	stLfo* 		ptLFO;

	ptLFO =&str_Sound_Work.str_LFO1;

	//--------------------------------------------------------------------
	// LFO
	//--------------------------------------------------------------------
	ptLFO->ui16_Cpt+=2;
	if(ptLFO->ui16_Cpt>ptLFO->ui16_Freq)
	{
		ptLFO->ui16_Cpt=0;
		ptLFO->ui16_Phase++;
		if(ptLFO->ui16_Phase==360)
		{
			BSP_LED_Toggle(LED_BLUE);
			ptLFO->ui16_Phase=0;
		}
		switch(ptLFO->ui8_Type)
		{
			case LFO_TYPE_SINE:
			ptLFO->ui32_Val=gui16_TabSineWave360[ptLFO->ui16_Phase];
			break;
			case LFO_TYPE_SQUARE:
			if(ptLFO->ui16_Phase<180)
				ptLFO->ui32_Val=0;
			else
				ptLFO->ui32_Val=MAX_LFO_VAL;
			break;
			case LFO_TYPE_TRI:
			if(ptLFO->ui16_Phase<180)
				ptLFO->ui32_Val = ptLFO->ui16_Phase*MAX_LFO_VAL/180;
			else
				ptLFO->ui32_Val = MAX_LFO_VAL-(ptLFO->ui16_Phase-180)*MAX_LFO_VAL/180;
			break;
			case LFO_TYPE_SAW:
			ptLFO->ui32_Val = ptLFO->ui16_Phase*MAX_LFO_VAL/360;
			break;
			case LFO_TYPE_INVSAW:
			ptLFO->ui32_Val = MAX_LFO_VAL-ptLFO->ui16_Phase*MAX_LFO_VAL/360;
			break;
			case LFO_TYPE_RANDOM:
			HAL_RNG_GenerateRandomNumber(&hrng,&rand);
			ptLFO->ui32_Val=rand & MAX_LFO_VAL;
			break;
		}
		//ptLFO->ui32_Val *=ptLFO->ui16_Amount;
		//ptLFO->ui32_Val /=100;
		//u32_Grain_AfterLFO = u32_Grain_Begin + ptLFO->ui32_Val*30;


//		if(u8_Waveplayer_Mode==WAVEPLAY_MODE_BIGBUFFERVIEW)
//		{
//			//Aff_Display_Play(u32_Grain_AfterLFO+u32_Grain_PosOffset_BigBuffer,u32_aff_echperpixel);
//			Aff_Display_Play(u32_Grain_AfterLFO,u32_aff_echperpixel);
//		}
	}

}

/**************************************************/
/* Grain Inc the EG ADSR                          */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
void Grain_Set_EG()
{
	static uint16_t cptenv;
	int16_t vol;
	//--------------------------------------------------------------------
	// ENV
	//--------------------------------------------------------------------
	cptenv++;
	if(cptenv > 30)
	{
		// A Value of 500 for the Attack is 5s
		cptenv=0;
		if(str_Sound_Work.str_ADSR_AMP_onevoice[0].u8_stateADSR != GRAIN_ADSR_STATE_STANDBY)
		{
			str_Sound_Work.str_ADSR_AMP_onevoice[0].u16_cpt++;
			vol = str_Sound_Work.str_ADSR_AMP_onevoice[0].i16_vol;
			switch(str_Sound_Work.str_ADSR_AMP_onevoice[0].u8_stateADSR)
			{
				case GRAIN_ADSR_STATE_ATTACK	:
				vol +=str_Sound_Work.str_ADSR_AMP.u16_deltaA;
				if(vol > ADSR_MAX_AMP)
				{
					str_Sound_Work.str_ADSR_AMP_onevoice[0].u8_stateADSR=GRAIN_ADSR_STATE_DECAY;
					vol=ADSR_MAX_AMP;
				}
				break;
				case GRAIN_ADSR_STATE_DECAY		:
				vol -=str_Sound_Work.str_ADSR_AMP.u16_deltaD;
				if(vol < ADSR_MAX_AMP*str_Sound_Work.str_ADSR_AMP.u16_sustain/100)
				{
					str_Sound_Work.str_ADSR_AMP_onevoice[0].u8_stateADSR=GRAIN_ADSR_STATE_SUSTAIN;
					vol = ADSR_MAX_AMP*str_Sound_Work.str_ADSR_AMP.u16_sustain/100;
				}
				break;
				case GRAIN_ADSR_STATE_SUSTAIN	:
				break;
				case GRAIN_ADSR_STATE_RELEASE	:
				vol -=str_Sound_Work.str_ADSR_AMP.u16_deltaR;
				if(vol < 0)
				{
					str_Sound_Work.str_ADSR_AMP_onevoice[0].u8_stateADSR=GRAIN_ADSR_STATE_STANDBY;
					vol=0;
				}
				break;
			}
			str_Sound_Work.str_ADSR_AMP_onevoice[0].i16_vol =vol;

		}
//		BSP_LCD_SetFont(&Font16);
//		sprintf(aff,"Cpt %04d",str_Sound_Work.str_ADSR_AMP_onevoice[0].u16_cpt);
//		BSP_LCD_DisplayStringAt(400,240+0,(uint8_t*)aff,LEFT_MODE);
//		sprintf(aff,"Sta %04d",str_Sound_Work.str_ADSR_AMP_onevoice[0].u8_stateADSR);
//		BSP_LCD_DisplayStringAt(400,240+20,(uint8_t*)aff,LEFT_MODE);
//		sprintf(aff,"Vol %04d",str_Sound_Work.str_ADSR_AMP_onevoice[0].i16_vol);
//		BSP_LCD_DisplayStringAt(400,240+40,(uint8_t*)aff,LEFT_MODE);
	}

}

/**************************************************/
/* Grain Set AR                                   */
/* EG For the Grain                               */
/*                                                */
/*                                                */
/**************************************************/
void Grain_Set_AR()
{
	u32_Grain_Slice=u32_Grain_Size;
	u32_Grain_Slice /=AUDIO_OUT_BUFFER_SIZE;
	if(u8_Type_BigBuffer == BUFFER_STEREO)
		u32_Grain_Slice *=2;	// 2 inc for Half DMA and Full DMA
	else
		u32_Grain_Slice *=4;

	if(str_Sound_Work.u16_attack)
		str_Sound_Work.u32_deltaA = (10000*1000)/(str_Sound_Work.u16_attack*u32_Grain_Slice);			// MAX 1000
	else
		str_Sound_Work.u32_deltaA = 50000;

	if(str_Sound_Work.u32_deltaA>50000)
		str_Sound_Work.u32_deltaA = 50000;

	if(str_Sound_Work.u16_release)
		str_Sound_Work.u32_deltaR = (10000*1000)/(str_Sound_Work.u16_release*u32_Grain_Slice);			// MAX 1000
	else
		str_Sound_Work.u32_deltaR = 50000;

	if(str_Sound_Work.u32_deltaR>50000)
		str_Sound_Work.u32_deltaR = 50000;

	str_Sound_Work.u32_deltaA *=2;
	str_Sound_Work.u32_deltaR *=2;


	str_Sound_Work.u16_TrigR = u32_Grain_Slice - (str_Sound_Work.u16_release*u32_Grain_Slice)/(2*100);			// Trig at sz/2 for Release=100
	//str_Sound_Work.u16_TrigR = sz-(1000/str_Sound_Work.u32_deltaR);			// Trig at sz/2 for Release=100

	// Test release
	test = (u32_Grain_Slice-str_Sound_Work.u16_TrigR)*str_Sound_Work.u32_deltaR;

	//str_Sound_Work.u8_Grain_NextTrig = u32_Grain_Slice/2;
	//str_Sound_Work.u16_offsetnextgrain=0;
}

/**************************************************/
/* Grain Init                                     */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Grain_Init(uint8_t 	actif,uint8_t 	nbgr,uint32_t posinit,uint16_t	sizeinit,uint16_t inter)
{
    // Set multiple grain

    str_Sound_Work.u8_grainnb = nbgr;
    return(GRAIN_RET_OK);
}





