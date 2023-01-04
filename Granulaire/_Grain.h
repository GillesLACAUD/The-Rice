/*
 * Grain.h
 *
 *  Created on: 19 mars 2020
 *      Author: Gilles
 */

#ifdef __cplusplus
    extern "C" {
    #endif


#include "stdio.h"


#ifndef APPLICATION_USER_GRAIN_H_
#define APPLICATION_USER_GRAIN_H_


#define GRAIN_MAX_POLY			6		// Max number of polyphonie


#define GRAIN_MAX				32		// Max number of grain
#define MAX_FILENAME_CHAR		20		// Name of the wave file use
#define MAX_NAME_CHAR			20		// Name of the sound

#define 	GRAIN_RET_OK		0		// It is ok





// x = Buffer
// S = u32_Grain_PosStart_BigBuffer in the file
// B = u32_Grain_PosBegin_BigBuffer begin in the buffer
// E = u32_Grain_PosEnd_BigBuffer end in the buffer
// --------------- Sxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ----------------------------
// --------------- SxxBxxxxxxxExxxxxxxxxxxxxxxxxxx ----------------------------

uint32_t	u32_Grain_PosStart_BigBuffer;				// Position of the big buffer in the file
uint32_t	u32_Grain_PosOffset_BigBuffer;				// Compteur when the buffer is play


uint32_t	u32_Grain_Begin;							// Offset Begin of the grain
uint32_t	u32_Grain_AfterLFO;
uint32_t	u32_Grain_Size;								// Size of the Grain in Byte
uint32_t	u32_Grain_Slice;							// Size of the Grain in Slice

#define BUFFER_MONO		1
#define BUFFER_STEREO	2


uint8_t		u8_Type_BigBuffer;
//uint8_t		u8_Tab_BigBuffer[GRAIN_BIGBUFFER_SIZE];		// WARNING VERY BIG BUFFER


#define GRAIN_AR_STATE_ATTACK		0
#define GRAIN_AR_STATE_SUSTAIN		1
#define GRAIN_AR_STATE_RELEASE		2
#define GRAIN_AR_STATE_STANDBY		3


typedef struct
{
	uint8_t		u8_ident;		// Grain Number
	uint32_t	u32_beginpos;	//
	uint32_t	u32_size;		//
	uint16_t 	u16_panleft;	//
	uint16_t 	u16_panright;	//
	uint16_t	u16_cptAR;		// Cpt for the AMP progress
	uint32_t	u32_volA;		// Set the Vol Attack
	int32_t		s32_volR;		// Set the Vol Release
	uint8_t		u8_stateAR;		// State of the AR ENV
}str_Grain;


#define GRAIN_ADSR_STATE_STANDBY	0
#define GRAIN_ADSR_STATE_ATTACK		1
#define GRAIN_ADSR_STATE_DECAY		2
#define GRAIN_ADSR_STATE_SUSTAIN	3
#define GRAIN_ADSR_STATE_RELEASE	4

#define ADSR_MAX_CPT				500
#define ADSR_MAX_AMP				1000	// So min delta is 1000/500 = 2
#define ADSR_DIV_POT				ADSR_MAX_CPT/4095

typedef struct
{
	uint16_t	u16_attack;		// in ms
	uint16_t	u16_decay;		// in ms
	uint16_t	u16_sustain;	// in ms
	uint16_t	u16_release;	// in ms

	uint16_t	u16_deltaA;		// Delta inc for the Attack
	uint16_t	u16_deltaD;		// Delta dec for the Decay
	uint16_t	u16_deltaR;		// Delta dec for the Release


}str_ADSR;

typedef struct
{
	uint16_t	u16_cpt;		//
	uint8_t		u8_stateADSR;	//
	int16_t		i16_vol;		//
}str_ADSR_onevoice;



typedef struct
{
	uint8_t		u8_mode;		// LPF HPF
	uint16_t	u16_cutoff;		// 0-4095
	uint16_t	u16_resonance;	// 0-4095
}str_FILTER;


#define MAX_LFO_VAL		0xFFF
#define SINE_WAVE_360	360			// Taille du cycle du LFO


#ifdef __GRAIN__
//-------------------------------------------------------------------------
// Use by the LFO
//-------------------------------------------------------------------------
const uint16_t gui16_TabSineWave360[SINE_WAVE_360] =
{
	0x7FF, 0x823, 0x846, 0x86A, 0x88E, 0x8B1, 0x8D5, 0x8F9, 0x91C, 0x93F,
	0x963, 0x986, 0x9A9, 0x9CC, 0x9EE, 0xA11, 0xA33, 0xA56, 0xA78, 0xA9A,
	0xABB, 0xADD, 0xAFE, 0xB1F, 0xB40, 0xB60, 0xB81, 0xBA1, 0xBC0, 0xBE0,
	0xBFF, 0xC1E, 0xC3C, 0xC5A, 0xC78, 0xC95, 0xCB2, 0xCCF, 0xCEC, 0xD08,
	0xD23, 0xD3E, 0xD59, 0xD73, 0xD8D, 0xDA7, 0xDC0, 0xDD8, 0xDF1, 0xE08,
	0xE1F, 0xE36, 0xE4C, 0xE62, 0xE77, 0xE8C, 0xEA0, 0xEB4, 0xEC7, 0xEDA,
	0xEEC, 0xEFE, 0xF0F, 0xF1F, 0xF2F, 0xF3F, 0xF4D, 0xF5C, 0xF69, 0xF77,
	0xF83, 0xF8F, 0xF9A, 0xFA5, 0xFAF, 0xFB9, 0xFC2, 0xFCA, 0xFD2, 0xFD9,
	0xFDF, 0xFE5, 0xFEB, 0xFEF, 0xFF3, 0xFF7, 0xFFA, 0xFFC, 0xFFD, 0xFFE,

	0xFFF, 0xFFE, 0xFFD, 0xFFC, 0xFFA, 0xFF7, 0xFF3, 0xFEF, 0xFEB, 0xFE5,
	0xFDF, 0xFD9, 0xFD2, 0xFCA, 0xFC2, 0xFB9, 0xFAF, 0xFA5, 0xF9A, 0xF8F,
	0xF83, 0xF77, 0xF69, 0xF5C, 0xF4D, 0xF3F, 0xF2F, 0xF1F, 0xF0F, 0xEFE,
	0xEEC, 0xEDA, 0xEC7, 0xEB4, 0xEA0, 0xE8C, 0xE77, 0xE62, 0xE4C, 0xE36,
	0xE1F, 0xE08, 0xDF1, 0xDD8, 0xDC0, 0xDA7, 0xD8D, 0xD73, 0xD59, 0xD3E,
	0xD23, 0xD08, 0xCEC, 0xCCF, 0xCB2, 0xC95, 0xC78, 0xC5A, 0xC3C, 0xC1E,
	0xBFF, 0xBE0, 0xBC0, 0xBA1, 0xB81, 0xB60, 0xB40, 0xB1F, 0xAFE, 0xADD,
	0xABB, 0xA9A, 0xA78, 0xA56, 0xA33, 0xA11, 0x9EE, 0x9CC, 0x9A9, 0x986,
	0x963, 0x93F, 0x91C, 0x8F9, 0x8D5, 0x8B1, 0x88E, 0x86A, 0x846, 0x823,

	0x7FF, 0x7DB, 0x7B8, 0x794, 0x770, 0x74D, 0x729, 0x705, 0x6E2, 0x6BF,
	0x69B, 0x678, 0x655, 0x632, 0x610, 0x5ED, 0x5CB, 0x5A8, 0x586, 0x564,
	0x543, 0x521, 0x500, 0x4DF, 0x4BE, 0x49E, 0x47D, 0x45D, 0x43E, 0x41E,
	0x3FF, 0x3E0, 0x3C2, 0x3A4, 0x386, 0x369, 0x34C, 0x32F, 0x312, 0x2F6,
	0x2DB, 0x2C0, 0x2A5, 0x28B, 0x271, 0x257, 0x23E, 0x226, 0x20D, 0x1F6,
	0x1DF, 0x1C8, 0x1B2, 0x19C, 0x187, 0x172, 0x15E, 0x14A, 0x137, 0x124,
	0x112, 0x100, 0x0EF, 0x0DF, 0x0CF, 0x0BF, 0x0B1, 0x0A2, 0x095, 0x087,
	0x07B, 0x06F, 0x064, 0x059, 0x04F, 0x045, 0x03C, 0x034, 0x02C, 0x025,
	0x01F, 0x019, 0x013, 0x00F, 0x00B, 0x007, 0x004, 0x002, 0x001, 0x000,

	0x000, 0x000, 0x001, 0x002, 0x004, 0x007, 0x00B, 0x00F, 0x013, 0x019,
	0x01F, 0x025, 0x02C, 0x034, 0x03C, 0x045, 0x04F, 0x059, 0x064, 0x06F,
	0x07B, 0x087, 0x095, 0x0A2, 0x0B1, 0x0BF, 0x0CF, 0x0DF, 0x0EF, 0x100,
	0x112, 0x124, 0x137, 0x14A, 0x15E, 0x172, 0x187, 0x19C, 0x1B2, 0x1C8,
	0x1DF, 0x1F6, 0x20D, 0x226, 0x23E, 0x257, 0x271, 0x28B, 0x2A5, 0x2C0,
	0x2DB, 0x2F6, 0x312, 0x32F, 0x34C, 0x369, 0x386, 0x3A4, 0x3C2, 0x3E0,
	0x3FF, 0x41E, 0x43E, 0x45D, 0x47D, 0x49E, 0x4BE, 0x4DF, 0x500, 0x521,
	0x543, 0x564, 0x586, 0x5A8, 0x5CB, 0x5ED, 0x610, 0x632, 0x655, 0x678,
	0x69B, 0x6BF, 0x6E2, 0x705, 0x729, 0x74D, 0x770, 0x794, 0x7B8, 0x7DB
};
#else
extern const uint16_t gui16_TabSineWave360[SINE_WAVE_360];
#endif


#define LFO_TYPE_SINE		0		// Sinus
#define LFO_TYPE_SQUARE		1		// Square
#define LFO_TYPE_TRI		2		// Triangle
#define LFO_TYPE_SAW		3		// Saw
#define LFO_TYPE_INVSAW		4		// Saw Invert
#define LFO_TYPE_RANDOM		5		// Random
#define LFO_TYPE_MAX		5		//

#define LFO_DEST_AMP		0		//
#define LFO_DEST_PITCH		1		//
#define LFO_DEST_POSGRAIN	2		//
#define LFO_DEST_NBGRAIN	3		//

typedef struct
{

	uint16_t	ui16_Cpt;			// Internal Compteur
	uint16_t	ui16_Freq;			// LFO Frequency

	uint16_t	ui16_Phase;			// LFO Frequency

	uint8_t		ui8_Route;			// Route the LFO To see LFO_DEST_XX
	uint8_t		ui8_Type;			// Waveform
	uint16_t	ui16_Amount;		// Amount of the LFO 0 to 100%
	uint32_t	ui32_Val;			// Value of the LFO between 0 and 0xFFF

}stLfo;

typedef struct
{
	char		name[MAX_NAME_CHAR];
	char		filename[MAX_FILENAME_CHAR];
	uint32_t	u32_filesize;					// AMP Release in ms
	str_Grain	str_tabgrain[GRAIN_MAX];
	uint8_t		u8_grainnb;						// Number of grain in the sound

	// Fixed at 50% for the moment
	uint8_t		u8_Grain_NextTrig;				// Trig the Next grain after x% of the size play
	uint32_t	u32_offsetnextgrain;			// Offset between the Grain
	uint8_t		u8_rangeshift;					//
	uint8_t		u8_spaceranfixe;				//

	//-------------------------------------
	// ENV AR DEFINE FOR ALL THE GRAIN
	//-------------------------------------
	uint8_t		u8_AR_change;					//
	uint16_t	u16_attack;						// AMP Attack for all the grain  % of the grain size between 0 to 100
	uint16_t	u16_release;					// AMP Release for all the grain % of the grain size between 0 to 100
	uint32_t	u32_deltaA;						//
	uint32_t	u32_deltaR;						//
	uint16_t	u16_TrigR;						// Trig the Release after u16_cptAR = u16_TrigR


	//-------------------------------------
	// PAN
	//-------------------------------------
	uint8_t		u8_pan;							// Change Pan after the end of a grain or like a LFO
	uint32_t	u32_fsample;

	//-------------------------------------
	// VOICE
	//-------------------------------------
	//str_VOICE	str_VOICE_Tab[GRAIN_MAX_POLY];
	//str_Grain	str_tabgrain[GRAIN_MAX_POLY][GRAIN_MAX];

	//-------------------------------------
	// FILTER
	//-------------------------------------

	str_FILTER	str_FILTER_SOUND;

	//-------------------------------------
	// ENV ADSR
	//-------------------------------------

	str_ADSR	str_ADSR_AMP;					// Definition of the ADSR
	str_ADSR	str_ADSR_FILTER;				// Definition of the ADSR
	str_ADSR	str_ADSR_PITCH;					// Definition of the ADSR

	str_ADSR_onevoice	str_ADSR_AMP_onevoice[GRAIN_MAX_POLY];		// Evolution of the ADSR per one voice
	str_ADSR_onevoice	str_ADSR_FILTER_onevoice[GRAIN_MAX_POLY];	// Evolution of the ADSR per one voice
	str_ADSR_onevoice	str_ADSR_PITCH_onevoice[GRAIN_MAX_POLY];	// Evolution of the ADSR per one voice

	//-------------------------------------
	// LFO
	//-------------------------------------
	stLfo 		str_LFO1;
	stLfo 		str_LFO2;


}str_Sound;

str_Sound	str_Sound_Work;
uint8_t		u8_AskChangeGrain;		//

//-------------------------------------
// FUNCTIONS DECLARATIONS
//-------------------------------------
void	Grain_Set_AR();
void	Grain_Set_LFO();
void	Grain_Set_EG();
uint8_t Grain_Init(uint8_t actif,uint8_t nbgr,uint32_t posinit,uint16_t	sizeinit,uint16_t inter);

#endif /* APPLICATION_USER_GRAIN_H_ */

#ifdef __cplusplus
}
#endif
