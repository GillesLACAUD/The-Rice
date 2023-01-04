#ifdef __cplusplus
    extern "C" {
    #endif

#define __MATRIX__
#include "Matrix.h"

#include "waveplayer.h"

int32_t  valueafterMod;

void Matrix_Apply_Mod()
{
int32_t* in;
int32_t* out;
int32_t* mod;

	for(uint8_t Dst=0;Dst<MAX_ROUTE_DST;Dst++)
	{
		in = (int32_t*)Tab_Pt_Dst[Dst];
		out= (int32_t*)Tab_Pt_DstMod[Dst];
		*out=0;
		for(uint8_t Mod=0;Mod<MAX_ROUTE_MOD;Mod++)
		{
			mod = (int32_t*)Tab_Pt_Mod[Mod];
			if(Tab_Matrix_Val[Mod][Dst])
			{
				//*out += *mod*Tab_Matrix_Val[Mod][Dst]*(*in)/100;			// Pourcent
				*out += *mod*Tab_Matrix_Val[Mod][Dst]/100;					// Absolute deviation
			}
		}
		if(in==&str_Sound_Work.u32_Grain_Begin)
			valueafterMod = *out;
	}

	u32_Mod_Grain_Begin	 	*=30*GRAIN_BIG_BUFFER_MULTI;
	u32_Mod_Grain_Size		*=20*GRAIN_BIG_BUFFER_MULTI;
	u16_Mod_Attack			/=30;
	u16_Mod_Release			/=30;

	if(u16_Mod_Attack != 0 || u16_Mod_Release !=0)
		str_Sound_Work.u8_AR_change= 1;

	u32_Mod_Grain_Begin	+=str_Sound_Work.u32_Grain_Begin;
	u32_Mod_Grain_Size  +=str_Sound_Work.u32_Grain_Size;
	u16_Mod_Attack 		+=str_Sound_Work.u16_attack;
	u16_Mod_Release		+=str_Sound_Work.u16_release;

	if(u16_Mod_Attack==0)
		u16_Mod_Attack =1;
	if(u16_Mod_Release==0)
		u16_Mod_Release =1;


}


#ifdef __cplusplus
}
#endif


//#define MOD_MAX 2
//	int16_t in = 0;
//	int16_t out = 0;
//	int16_t Tab_out[100];
//	int16_t	Mod_Val[MOD_MAX][100];
//	int16_t	Mod_Amt[MOD_MAX];
//	int16_t Cutoff = 50;
//
//	Mod_Amt[0] = 75;
//	Mod_Amt[1] = 100;
//
//	// MODULATOR
//	// - Unipolaire/Bipolaire
//	// - Positif/Negatif
//	//
//	// Unipolaire positif	Ex: An Amp EG
//	// Unipolaire negatif   Ex: An Invert filter EG
//	// Bipolaire positif	Ex: An Lfo who start with a positif change
//	// Bipolaire negatif	Ex: An Lfo who start with a negatif change
//
//
//	//-------------------------------------------
//	// Modulators range
//	//-------------------------------------------
//	//                      MIN				MAX
//	// KEY VEL				0				127
//	// KEY AFTER			0				127
//	// MOD WHELL			0				127
//	// LFO1					0				1000
//	// ADSR1				0				1000
//
//	//-------------------------------------------
//	// Value Range
//	//-------------------------------------------
//	//                      MIN				MAX
//	// NB GRAIN				1				32
//	// SPACE				0
//	// SIZE
//	// TRIG
//	// CUTOFF				0				1000
//	// RESONANCE			0				1000
//
//
//	// Bipolaire mod init for test
//	for(uint8_t t=0;t<100;t++)
//	{
//		if(t<50)
//		{
//			Mod_Val[0][t] = -50;
//		}
//		else
//		{
//			Mod_Val[0][t] = 50;
//		}
//	}
//
//	// Unipolaire mod positif init for test
//	for(uint8_t t=0;t<100;t++)
//	{
//			Mod_Val[1][t] = t;
//	}
//
//	//-------------------------------
//	// Compute the output value
//	//-------------------------------
//	for(uint8_t t=0;t<100;t++)
//	{
//		in = Cutoff;
//		for(uint8_t Mod=0;Mod<MOD_MAX;Mod++)
//		{
//			if(Mod_Amt[Mod])
//			{
//				out = in + (Mod_Val[Mod][t] * Mod_Amt[Mod])/100;
//				in=out;
//			}
//		}
//		Tab_out[t]=out;
//	}
