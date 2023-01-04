/*
 * Grain.c
 *
 *  Created on: 19 mars 2020
 *      Author: Gilles
 */
#define __GRAIN__
#include "Grain.h"
#include "Waveplayer.h"
#include "stm32469i_discovery.h"

#include "C:/TouchGFXProjects/MyApplication/Granulaire/Matrix.h"
#include "C:/TouchGFXProjects/MyApplication/Granulaire/Midi.h"


uint32_t test;

extern RNG_HandleTypeDef hrng;

/**************************************************/
/* Grain Inc the LFO                              */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
void Grain_Set_LFO(stLfo* ptLFO)
{
	uint32_t rand;

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
	}

}


float SmoothADSR(uint16_t threshold,uint8_t step,float val,uint16_t delta)
{
//uint8_t div,max;
float ret,div,max;

	if(val<threshold)
	{
		div = (float)val/(float)step;
		max= (float)threshold/(float)step;
		max +=1;
		ret = (float)delta/(max-div);
		//ret +=1;
		return(ret);
	}
	else
		return((float)delta);
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
	float ret;
	//--------------------------------------------------------------------
	// ENV
	//--------------------------------------------------------------------
	cptenv++;
	if(cptenv >4)
	{
		// A Value of 500 for the Attack is 5s
		cptenv=0;

		for(uint8_t v=0;v<GRAIN_MAX_POLY;v++)
		{
			if(str_Sound_Work.str_ADSR_AMP_onevoice[v].u8_stateADSR != GRAIN_ADSR_STATE_STANDBY)
			{
				str_Sound_Work.str_ADSR_AMP_onevoice[v].u16_cpt++;
				vol = str_Sound_Work.str_ADSR_AMP_onevoice[v].i16_vol;
				switch(str_Sound_Work.str_ADSR_AMP_onevoice[v].u8_stateADSR)
				{
					case GRAIN_ADSR_STATE_ATTACK	:
					ret = SmoothADSR(400,400,str_Sound_Work.str_ADSR_AMP_onevoice[v].f_vol,str_Sound_Work.str_ADSR_AMP.u16_deltaA);
					//vol +=str_Sound_Work.str_ADSR_AMP.u16_deltaA;
					str_Sound_Work.str_ADSR_AMP_onevoice[v].f_vol +=ret;
					vol = (int16_t)str_Sound_Work.str_ADSR_AMP_onevoice[v].f_vol;
					if(vol > ADSR_MAX_AMP)
					{
						str_Sound_Work.str_ADSR_AMP_onevoice[v].u8_stateADSR=GRAIN_ADSR_STATE_DECAY;
						vol=ADSR_MAX_AMP;
					}
					break;
					case GRAIN_ADSR_STATE_DECAY		:
					ret = SmoothADSR((ADSR_MAX_AMP*str_Sound_Work.str_ADSR_AMP.u16_sustain/100)+100,400,str_Sound_Work.str_ADSR_AMP_onevoice[v].f_vol,str_Sound_Work.str_ADSR_AMP.u16_deltaD);
					str_Sound_Work.str_ADSR_AMP_onevoice[v].f_vol -=ret;
					vol = (int16_t)str_Sound_Work.str_ADSR_AMP_onevoice[v].f_vol;
					if(vol < ADSR_MAX_AMP*str_Sound_Work.str_ADSR_AMP.u16_sustain/100)
					{
						str_Sound_Work.str_ADSR_AMP_onevoice[v].u8_stateADSR=GRAIN_ADSR_STATE_SUSTAIN;
						vol = ADSR_MAX_AMP*str_Sound_Work.str_ADSR_AMP.u16_sustain/100;
					}
					break;

					case GRAIN_ADSR_STATE_SUSTAIN	:
					break;

					case GRAIN_ADSR_STATE_RELEASE	:
					ret = SmoothADSR(400,400,str_Sound_Work.str_ADSR_AMP_onevoice[v].f_vol,str_Sound_Work.str_ADSR_AMP.u16_deltaR);
					str_Sound_Work.str_ADSR_AMP_onevoice[v].f_vol -=ret;
					vol = (int16_t)str_Sound_Work.str_ADSR_AMP_onevoice[v].f_vol;
					if(vol < 0)
					{
						str_Sound_Work.str_ADSR_AMP_onevoice[v].u8_stateADSR=GRAIN_ADSR_STATE_STANDBY;
						Midi_Tab_Note[v]=0xFF;
						vol=0;
						// kill all the grains for this voice
						for(uint8_t grain=0;grain<GRAIN_MAX;grain++)
						 {
							  str_Sound_Work.str_tabgrain[v][grain].u16_cptAR = 0;
							  str_Sound_Work.str_tabgrain[v][grain].u8_stateAR =GRAIN_AR_STATE_STANDBY;
						  }
					}
					break;
				}
				str_Sound_Work.str_ADSR_AMP_onevoice[v].i16_vol =vol;
			}
		}
	}
}



/**************************************************/
/* Grain Set AR                                   */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
void Grain_Set_AR()
{
	u32_Grain_Slice=u32_Mod_Grain_Size;
	u32_Grain_Slice /=AUDIO_OUT_BUFFER_SIZE;
	if(u8_Type_BigBuffer == BUFFER_STEREO)
		u32_Grain_Slice *=2;	// 2 inc for Half DMA and Full DMA
	else
		u32_Grain_Slice *=4;

	if(!u32_Grain_Slice)
		u32_Grain_Slice=1;

	if(u16_Mod_Attack==0 || u32_Grain_Slice==0 || u16_Mod_Release==0)
	{
		u16_Mod_Attack=1;
		u32_Grain_Slice=1;
		u16_Mod_Release=1;
	}

	if(str_Sound_Work.u16_attack>20)
		str_Sound_Work.u32_deltaA = (10000*1000)/(u16_Mod_Attack*u32_Grain_Slice);			// MAX 1000
	else
		str_Sound_Work.u32_deltaA = 50000;

	if(str_Sound_Work.u32_deltaA>50000)
		str_Sound_Work.u32_deltaA = 50000;

	if(str_Sound_Work.u16_release>20)
		str_Sound_Work.u32_deltaR = (10000*1000)/(u16_Mod_Release*u32_Grain_Slice);			// MAX 1000
	else
		str_Sound_Work.u32_deltaR = 50000;

	if(str_Sound_Work.u32_deltaR>50000)
		str_Sound_Work.u32_deltaR = 50000;

	str_Sound_Work.u32_deltaA *=2;
	str_Sound_Work.u32_deltaR *=2;


	str_Sound_Work.u16_TrigR = u32_Grain_Slice - (u16_Mod_Release*u32_Grain_Slice)/(2*100);			// Trig at sz/2 for Release=100
	// Test release
	test = (u32_Grain_Slice-str_Sound_Work.u16_TrigR)*str_Sound_Work.u32_deltaR;

//	if(str_Sound_Work.u16_attack)
//		str_Sound_Work.u32_deltaA = (10000*1000)/(str_Sound_Work.u16_attack*u32_Grain_Slice);			// MAX 1000
//	else
//		str_Sound_Work.u32_deltaA = 50000;
//
//	if(str_Sound_Work.u32_deltaA>50000)
//		str_Sound_Work.u32_deltaA = 50000;
//
//	if(str_Sound_Work.u16_release)
//		str_Sound_Work.u32_deltaR = (10000*1000)/(str_Sound_Work.u16_release*u32_Grain_Slice);			// MAX 1000
//	else
//		str_Sound_Work.u32_deltaR = 50000;
//
//	if(str_Sound_Work.u32_deltaR>50000)
//		str_Sound_Work.u32_deltaR = 50000;
//
//	str_Sound_Work.u32_deltaA *=2;
//	str_Sound_Work.u32_deltaR *=2;
//
//
//	str_Sound_Work.u16_TrigR = u32_Grain_Slice - (str_Sound_Work.u16_release*u32_Grain_Slice)/(2*100);			// Trig at sz/2 for Release=100
//	//str_Sound_Work.u16_TrigR = sz-(1000/str_Sound_Work.u32_deltaR);			// Trig at sz/2 for Release=100
//
//	// Test release
//	test = (u32_Grain_Slice-str_Sound_Work.u16_TrigR)*str_Sound_Work.u32_deltaR;
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



/**************************************************/
/* Display the grains on the screen               */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Grain_Display(str_Sound* sound,uint8_t grain)
{
	return(GRAIN_RET_OK);
}

/**************************************************/
/* Change all the grains position                 */
/* with equidistant space                         */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Grain_Set_Space(str_Sound* sound)
{
//uint8_t cpt;

	// Only if all the grain are selected
//	if(!u8_isgrain_allselect)
//		GRAIN_RET(GRAIN_RET_ALLSEL);
//
//	for(cpt=0;cpt<sound->u8_grainnb;cpt++)
//	{
//		sound->str_tabgrain[cpt].u32_pos += cpt*u16_grain_space;
//
//		// Too much Grain for this wave file reduce the nb grain
//		if(sound->str_tabgrain[cpt].u32_pos + sound->str_tabgrain[cpt].u16_size > sound->u32_filesize)
//		{
//			sound->u8_grainbreduce = cpt-1;
//			GRAIN_RET(GRAIN_RET_REDUCE);
//		}
//		Grain_Display(sound,cpt);
//	}
	return(GRAIN_RET_OK);
}

/**************************************************/
/* Change one grain position		              */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Grain_Set_Pos(str_Sound* sound,uint8_t grain,uint32_t pos)
{
//uint8_t cpt;

	// Only if one grain is selected
//	if(u8_isgrain_allselect)
//		GRAIN_RET(GRAIN_RET_ALLSEL);
//
//	// if it is not the last grain
//	if(grain < sound->u8_grainbreduce)
//	{
//		if(sound->str_tabgrain[grain].u32_pos + sound->str_tabgrain[grain].u16_size > sound->str_tabgrain[grain+1].u32_pos)
//		{
//			GRAIN_RET(GRAIN_RET_OVERPOS);
//		}
//		Grain_Display(sound,grain);
//	}
//	else
//	{
//		if(sound->str_tabgrain[grain].u32_pos + sound->str_tabgrain[grain].u16_size > sound->u32_filesize)
//		{
//			GRAIN_RET(GRAIN_RET_OVERPOS);
//		}
//		Grain_Display(sound,grain);
//	}
	return(GRAIN_RET_OK);
}





