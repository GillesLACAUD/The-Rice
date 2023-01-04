#ifdef __cplusplus
    extern "C" {
    #endif

#include "main.h"
#include "Grain.h"

    /******************************************************************************/
    /* MATRIX MODULATION                                                          */
    /******************************************************************************/

    #define MAX_ROUTE_DST	12
    #define MAX_ROUTE_MOD	18

	#define MAX_LABEL_TXT	15


#ifdef	__MATRIX__
#define MATRIX_EXTRN
#else
#define MATRIX_EXTRN	extern
#endif


#ifdef __MATRIX__
    enum { 	MATRIX_DST_GR_POS,MATRIX_DST_GR_SIZE,MATRIX_DST_GR_ATT,MATRIX_DST_GR_REL,
    		MATRIX_DST_GR_TRIG,MATRIX_DST_GR_DEN,MATRIX_DST_GR_PAN,MATRIX_DST_GR_SPACE,
    		MATRIX_DST_FL_CUT,MATRIX_DST_FL_RES,MATRIX_DST_AMP,MATRIX_DST_PITCH,
    		MATRIX_DST_LFO_AMT,MATRIX_DST_LFO1_FR,MATRIX_DST_LFO2_AMT,MATRIX_DST_LFO2_FR,
    		MATRIX_DST_FREE1,MATRIX_DST_FREE2
    };

    enum {	MATRIX_MOD_KEYVEL,MATRIX_MOD_AFTER,MATRIX_MOD_BEND,MATRIX_MOD_WHEEL,
    		MATRIX_MOD_TRACK,MATRIX_MOD_FREE1,MATRIX_MOD_FREE2,MATRIX_MOD_FREE3,
    		MATRIX_MOD_LFO1,MATRIX_MOD_FLO2,MATRIX_MOD_ADSR1,MATRIX_MOD_ADSR2,
    		MATRIX_MOD_PADXP,MATRIX_MOD_PADXM,MATRIX_MOD_PADYP,MATRIX_MOD_PADYM,
    		MATRIX_MOD_FREE4,MATRIX_MOD_FREE5
    };
#endif


MATRIX_EXTRN    uint32_t u32_Mod_Grain_Begin;
MATRIX_EXTRN    uint32_t u32_Mod_Grain_Size;
MATRIX_EXTRN    uint32_t u16_Mod_Attack;
MATRIX_EXTRN    uint32_t u16_Mod_Release;


MATRIX_EXTRN    uint8_t	Current_Route_Src;
MATRIX_EXTRN	uint8_t	Current_Route_Dst;

MATRIX_EXTRN	uint8_t Tab_Matrix_Val[MAX_ROUTE_MOD][MAX_ROUTE_DST];			// Value set by the GUI 0 to 100

// BIT0 (1:Bipolaire 	or 0:unipolaire)	can be set by the GUI
// BIT1 (1:phase180 	or 0:phase0)		can be set by the GUI
// BIT2 (1:Pournent 	or 0:Absolut)
// BIT3 (1:Divide	 	or 0:Multiplie)

// BIT4-5-6 Variable type
// 0x00	uint8_t
// 0x01 int8_t
// 0x02	uint16_t
// 0x03	int16_t
// 0x04 uint32_t
// 0x05 int32_t
MATRIX_EXTRN	uint8_t Tab_Matrix_Conf[MAX_ROUTE_MOD][MAX_ROUTE_DST];


MATRIX_EXTRN	void* Tab_Pt_Dst[MAX_ROUTE_DST];
MATRIX_EXTRN	void* Tab_Pt_Mod[MAX_ROUTE_MOD];



#ifdef	__MATRIX__

    uint32_t varnull=0;

    char Tab_Label_Route_Dst[MAX_ROUTE_DST][MAX_LABEL_TXT]={
    		"GRAIN POS    ","GRAIN SIZE    ","GRAIN ATTACK  ","GRAIN RELEASE ",
    		"GRAIN TRIG   ","GRAIN DENSITY ","GRAIN PAN     ","GRAIN SPACE   ",
    		"FILTER CUTOFF","FILTER RESO   ","AMPLITUDE     ","PITCH         ",
    		"LFO1 AMOUNT  ","LFO1 FREQ     ","LFO2 AMOUNT   ","LFO2 FREQ     ",
    		"             ","              "
    };

    void* Tab_Pt_Dst[MAX_ROUTE_DST]={
        		&str_Sound_Work.u32_Grain_Begin,&str_Sound_Work.u32_Grain_Size,&str_Sound_Work.u16_attack,&str_Sound_Work.u16_release,
    			&varnull,&varnull,&varnull,&varnull,
    			&varnull,&varnull,&varnull,&varnull,
    			&varnull,&varnull,&varnull,&varnull,
    			&varnull,&varnull
       	   };

    void* Tab_Pt_DstMod[MAX_ROUTE_DST]={
            		&u32_Mod_Grain_Begin,&u32_Mod_Grain_Size,&u16_Mod_Attack,&u16_Mod_Release,
        			&varnull,&varnull,&varnull,&varnull,
        			&varnull,&varnull,&varnull,&varnull,
        			&varnull,&varnull,&varnull,&varnull,
        			&varnull,&varnull
           	   };

    char Tab_Label_Route_Mod[MAX_ROUTE_MOD][MAX_LABEL_TXT]={
    		"KEY VEL      ","KEY AFTER    ","PITCH BEND   ","MOD WHEEL    ",		// Keyboard
    		"KEY TRACK    ","FREE1        ","FREE2        ","FREE3        ",
    		"LFO1         ","LFO2         ","ADSR1        ","ADSR2        ",		// LFO AND EG
    		"PAD X+       ","PAD X-       ","PAD Y+       ","PAD Y-       ",		// PAD ON TOUCH SCREEN
    		"FREE4        ","FREE5        ",
    };


    void* Tab_Pt_Mod[MAX_ROUTE_MOD]={
    		&varnull,&varnull,&varnull,&varnull,
			&varnull,&varnull,&varnull,&varnull,

			&str_Sound_Work.str_LFO1.ui32_Val,&str_Sound_Work.str_LFO2.ui32_Val,&varnull,&varnull,

			&varnull,&varnull,&varnull,&varnull,
			&varnull,&varnull
    };


#else
    MATRIX_EXTRN char Tab_Label_Route_Dst[MAX_ROUTE_DST][MAX_LABEL_TXT];
    MATRIX_EXTRN char Tab_Label_Route_Mod[MAX_ROUTE_MOD][MAX_LABEL_TXT];
#endif


MATRIX_EXTRN 	void Matrix_Apply_Mod();


#ifdef __cplusplus
}
#endif



