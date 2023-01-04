#ifdef __cplusplus
    extern "C" {
    #endif

#ifdef __ENCODER__
#define ENCODER_EXTRN
#else
#define ENCODER_EXTRN 	extern
#endif


#include "main.h"
#include "stm32f4xx_hal_gpio.h"

#include "lfo.h"
#include "matrix.h"

typedef enum Button_e{
    Open = 0,
    Closed,

    Pressed,
    Held,
    Released,

    Clicked,
    DoubleClicked

  }Button_S;


enum   { ENC1,ENC2,ENC3,ENC4,ENC5,ENC6,ENC7,ENC8};

#define TYPE_DATA	0
#define TYPE_LIST	1

typedef struct
{
	char 		Name[16];
	uint8_t		Type;
	const char	*ptTabList;		// pt to the list
	char		Index;			// Nb item in list
	int16_t*	Data;
	uint8_t		SizeData;
	int 		MinData;
	int 		MaxData;
	int 		Step;

	int			Delta;			// Max-Min
	int			CoeffCircle;	// Delta to 0-1000

	int 		(*ptrfunctValueChange) (int);
	int 		(*ptrfunctBPOn) (int);
	int 		(*ptrfunctBPOff) (int);
	int 		(*ptrfunctBPHold) (int);
	int 		(*ptrfunctBPDoubleClick) (int);

}Encoder_Data;

Encoder_Data	DataNull;

typedef struct
{
	uint8_t 			id;
	// Define  the GPIO
	GPIO_TypeDef* 		GPIO_A;
	uint16_t 			GPIO_Pin_A;
	GPIO_TypeDef* 		GPIO_B;
	uint16_t 			GPIO_Pin_B;

	uint8_t 			OldA;
	uint8_t 			OldB;
	uint8_t 			CptEdge;
	uint32_t 			t1;

	Encoder_Data*		data;

}Encoder;

int16_t trash;
char TabListNull[0][0];

#define	MAX_SECTION			6


#define	MAX_ENCODER1_DATA	MAX_SECTION
#define	MAX_ENCODER2_DATA	MAX_SECTION
#define	MAX_ENCODER3_DATA	MAX_SECTION
#define	MAX_ENCODER4_DATA	MAX_SECTION
#define	MAX_ENCODER5_DATA	MAX_SECTION
#define	MAX_ENCODER6_DATA	MAX_SECTION
#define	MAX_ENCODER7_DATA	MAX_SECTION
#define	MAX_ENCODER8_DATA	MAX_SECTION

#define	MAX_ENCODERM_DATA	MAX_SECTION

ENCODER_EXTRN	uint8_t	Tab_Enc_Param[9];
ENCODER_EXTRN	uint8_t	Tab_Enc_Move[9];

#define MAX_DATA	16

ENCODER_EXTRN	int16_t Tab_Val[8*MAX_SECTION];			// Only for ENC1 to 8
ENCODER_EXTRN	int16_t Tab_Section;					// Only for ENC Menu
ENCODER_EXTRN	uint16_t Tab_Circle[8*MAX_SECTION];		// Only for ENC1 to 8



ENCODER_EXTRN	uint8_t Tab_PBState[8*MAX_SECTION];		// Only for ENC1 to 8


#ifdef __ENCODER__

char Tab_Section_Name[MAX_SECTION][20]=
{
		"FILE","GRAIN","LFO","ADSR","MATRIX","DRONE"
};

char Tab_Label[8*MAX_SECTION][10]=
{
		//1			2			3				4				5			6			7			8
		"BANK"  	,"SOUND"	,"--------"		,"MIDI RX"		,"FILE"		,"--------"	,"GAIN"		,"VOL",
		"POS"   	,"SIZE"		,"ATTACK"		,"RELEASE"		,"TRIG "	,"DENS"		,"PAN" 		,"SPACE",
		"FREQ"		,"SHAPE"	,"--------"		,"--------"		,"FREQ"		,"SHAPE"	,"--------"	,"--------",
		"ATT"  		,"DEC"		,"SUS"			,"REL"			,"ATT"		,"DEC"		,"SUS"		,"REL",
		"SRC"		,"DEST "	,"VALUE"		,"--------"		,"--------"	,"--------"	,"--------"	,"--------",
		"PITCH"		,"PITCH"	,"PITCH"		,"PITCH"		,"FINE"		,"FINE"		,"FINE"		,"FINE"
};


char Tab_Label_PB[2*8*MAX_SECTION][10]=
{
		//-----------------------------------------------------------------------------------------------------
		// ON
		//-----------------------------------------------------------------------------------------------------
		//1			2			3				4				5			6			7			8
		"LOAD"    	,"SAVE"		,"--------"		,"THRU"			,"VALID"   	,"--------"	,"--------"	,"--------",
		"LOOP"    	,"MEMORY"	,"S ON"  		,"--------"		,"--------"	,"TUNE"		,"PITCH"	,"GILLES  ",
		"--------"	,"--------"	,"LOOP"    		,"SYNC"			,"--------"	,"--------"	,"LOOP"		,"SYNC",
		"ON"        ,"--------"	,"--------"		,"--------"		,"ON"	    ,"--------"	,"--------"	,"--------",
		"--------"	,"--------"	,"--------"		,"ESC"			,"--------"	,"--------"	,"--------"	,"--------",
		"ON"  		,"ON"		,"ON"			,"ON"			,"--------"	,"--------"	,"--------"	,"--------",

		//-----------------------------------------------------------------------------------------------------
		// OFF
		//-----------------------------------------------------------------------------------------------------
		//1			2			3				4				5			6			7			8
		"LOAD"    	,"SAVE"		,"--------"		,"THRU"			,"VALID"   	,"--------"	,"--------"	,"--------",
		"LOOP"    	,"MEMORY"	,"S OFF"		,"--------"		,"--------"	,"TUNE"		,"PITCH"	,"FIXED",
		"--------"	,"--------"	,"LOOP"    		,"SYNC"			,"--------"	,"--------"	,"LOOP"		,"SYNC",
		"OFF"       ,"--------"	,"--------"		,"--------"		,"OFF"	    ,"--------"	,"--------"	,"--------",
		"--------"	,"--------"	,"--------"		,"ESC"			,"--------"	,"--------"	,"--------"	,"--------",
		"OFF"  		,"OFF"		,"OFF"			,"OFF"			,"--------"	,"--------"	,"--------"	,"--------"
};



int FctNull(int val)
{
	return val;
}

Encoder_Data	TabEncoder1_Data[MAX_ENCODER1_DATA] =
{
		"A0"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[0+MAX_SECTION*ENC1],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// FILE
		"A1"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[1+MAX_SECTION*ENC1],2,	0,1000		     ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// GRAIN	POS
		"A2"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[2+MAX_SECTION*ENC1],2,	1,100            ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// LFO		FREQ
		"A3"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[3+MAX_SECTION*ENC1],2,	0,100            ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// ADSR		ATTACK
		"A4"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[4+MAX_SECTION*ENC1],2,	0,MAX_ROUTE_MOD-1,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// MATRIX	MODULATOR
		"A5"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[5+MAX_SECTION*ENC1],2,	0,48       		 ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// DRONE
};
Encoder_Data	TabEncoder2_Data[MAX_ENCODER2_DATA] =
{
		"B0"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[0+MAX_SECTION*ENC2],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// FILE
		"B1"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[1+MAX_SECTION*ENC2],2,	0,1000		     ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// GRAIN	SIZE
		"B2"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[2+MAX_SECTION*ENC2],2,	0,LFO_SHAPEMAX   ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// LFO		SHAPE
		"B3"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[3+MAX_SECTION*ENC2],2,	0,100            ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// ADSR		DECAY
		"B4"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[4+MAX_SECTION*ENC2],2,	0,MAX_ROUTE_DST-1,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// MATRIX	DESTINATION
		"B5"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[5+MAX_SECTION*ENC2],2,	0,48      		 ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// DRONE
};                                                                                              
Encoder_Data	TabEncoder3_Data[MAX_ENCODER3_DATA] =                                           
{                                                                                               
		"C0"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[0+MAX_SECTION*ENC3],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// FILE
		"C1"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[1+MAX_SECTION*ENC3],2,	0,100	         ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// GRAIN	ATTACK
		"C2"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[2+MAX_SECTION*ENC3],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// LFO		NONE
		"C3"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[3+MAX_SECTION*ENC3],2,	0,100            ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// ADSR		SUSTAIN
		"C4"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[4+MAX_SECTION*ENC3],2,	0,99             ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,			// MATRIX	AMOUNT
		"C5"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[5+MAX_SECTION*ENC3],2,	0,48 		     ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// DRONE
};                                                                                              
Encoder_Data	TabEncoder4_Data[MAX_ENCODER4_DATA] =                                           
{                                                                                               
		"D0"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[0+MAX_SECTION*ENC4],2,	1,16	         ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// FILE		MIDI RX
		"D1"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[1+MAX_SECTION*ENC4],2,	0,100		     ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// GRAIN	RELEASE
		"D2"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[2+MAX_SECTION*ENC4],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// LFO		NONE
		"D3"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[3+MAX_SECTION*ENC4],2,	0,100            ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// ADSR		RELEASE
		"D4"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[4+MAX_SECTION*ENC4],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// MATRIX	NONE
		"D5"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[5+MAX_SECTION*ENC4],2,	0,48 		     ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// DRONE
};                                                                                              
Encoder_Data	TabEncoder5_Data[MAX_ENCODER5_DATA] =
{
		"E0"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[0+MAX_SECTION*ENC5],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// FILE
		"E1"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[1+MAX_SECTION*ENC5],2,	0,100		     ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// GRAIN	TRIG
		"E2"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[2+MAX_SECTION*ENC5],2,	1,100            ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// LFO		FREQ
		"E3"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[3+MAX_SECTION*ENC5],2,	0,100            ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// ADSR		ATTACK
		"E4"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[4+MAX_SECTION*ENC5],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// MATRIX	NONE
		"E5"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[5+MAX_SECTION*ENC5],2, 0,100		     ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// DRONE
};                                                                                              
Encoder_Data	TabEncoder6_Data[MAX_ENCODER6_DATA] =                                           
{                                                                                               
		"F0"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[0+MAX_SECTION*ENC6],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// FILE
		"F1"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[1+MAX_SECTION*ENC6],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// GRAIN	DENSITY
		"F2"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[2+MAX_SECTION*ENC6],2,	0,LFO_SHAPEMAX   ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// LFO SHAPE
		"F3"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[3+MAX_SECTION*ENC6],2,	0,100            ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// ADSR		DECAY
		"F4"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[4+MAX_SECTION*ENC6],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// MATRIX	NONE
		"F5"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[5+MAX_SECTION*ENC6],2,	0,100		     ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// DRONE
};                                                                                              
Encoder_Data	TabEncoder7_Data[MAX_ENCODER7_DATA] =                                           
{                                                                                               
		"G0"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[0+MAX_SECTION*ENC7],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// FILE
		"G1"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[1+MAX_SECTION*ENC7],2,	0,100		     ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// GRAIN	PAN
		"G2"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[2+MAX_SECTION*ENC7],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// LFO		NONE
		"G3"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[3+MAX_SECTION*ENC7],2,	0,100            ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// ADSR		SUSTAIN
		"G4"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[4+MAX_SECTION*ENC7],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// MATRIX	NONE
		"G5"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[5+MAX_SECTION*ENC7],2,	0,100		     ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// DRONE
};                                                                                              
Encoder_Data	TabEncoder8_Data[MAX_ENCODER8_DATA] =                                           
{                                                                                               
		"H0"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[0+MAX_SECTION*ENC8],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// FILE
		"H1"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[1+MAX_SECTION*ENC8],2,	0,100		     ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// GRAIN	SPACE
		"H2"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[2+MAX_SECTION*ENC8],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// LFO		NONE
		"H3"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[3+MAX_SECTION*ENC8],2,	0,100            ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// ADSR		RELEASE
		"H4"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[4+MAX_SECTION*ENC8],2,	0,MAX_DATA       ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// MATRIX	NONE
		"H5"	,TYPE_DATA,&TabListNull[0][0],0,&Tab_Val[5+MAX_SECTION*ENC8],2,	0,100           ,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// DRONE
};
Encoder_Data	TabEncoderM_Data[1] =
{
		"SX"	,TYPE_DATA,&TabListNull[0][0],0,&trash,2,					0,5,1,0,0,FctNull,FctNull,FctNull,FctNull,FctNull,		// FILE
};



#else
extern char Tab_Section_Name[MAX_SECTION][20];
extern	char Tab_Label[8*MAX_SECTION][10];
extern 	char Tab_Label_PB[8*MAX_SECTION][10];

extern int FctNull(int val);
extern Encoder_Data	TabEncoder1_Data[MAX_ENCODER1_DATA];
extern Encoder_Data	TabEncoder2_Data[MAX_ENCODER2_DATA];
extern Encoder_Data	TabEncoder3_Data[MAX_ENCODER3_DATA];
extern Encoder_Data	TabEncoder4_Data[MAX_ENCODER4_DATA];

extern Encoder_Data	TabEncoder5_Data[MAX_ENCODER5_DATA];
extern Encoder_Data	TabEncoder6_Data[MAX_ENCODER6_DATA];
extern Encoder_Data	TabEncoder7_Data[MAX_ENCODER7_DATA];
extern Encoder_Data	TabEncoder8_Data[MAX_ENCODER8_DATA];

extern Encoder_Data	TabEncoderM_Data[MAX_ENCODERM_DATA];

#endif

// DONE Set the GPIO
// For the push button Encoder get the value of the MCP23017 (I2C1)

// Check 10.04.22
// ENC 1	PG10	PA07
// ENC 2	PH06	PB15
// ENC 3	PG13	PA01
// ENC 4	PG12	PA02
// ENC 5	PA08	PB04
// ENC 6	PC06	PA05
// ENC 7	PC07	PB05
// ENC 8	PB13	PA15
// ENC M	PB01	PC02


#ifdef __ENCODER__
Encoder Enc_1= {0,GPIOG,GPIO_PIN_10,GPIOA,GPIO_PIN_7 ,	0,0,0,0,&TabEncoder1_Data[0]};
Encoder Enc_2= {1,GPIOH,GPIO_PIN_6 ,GPIOB,GPIO_PIN_15,	0,0,0,0,&TabEncoder2_Data[0]};
Encoder Enc_3= {2,GPIOG,GPIO_PIN_13,GPIOA,GPIO_PIN_1 ,	0,0,0,0,&TabEncoder3_Data[0]};
Encoder Enc_4= {3,GPIOG,GPIO_PIN_12,GPIOA,GPIO_PIN_2 ,	0,0,0,0,&TabEncoder4_Data[0]};
Encoder Enc_5= {4,GPIOA,GPIO_PIN_8 ,GPIOB,GPIO_PIN_4 ,	0,0,0,0,&TabEncoder5_Data[0]};
Encoder Enc_6= {5,GPIOC,GPIO_PIN_6 ,GPIOA,GPIO_PIN_5 ,	0,0,0,0,&TabEncoder6_Data[0]};
Encoder Enc_7= {6,GPIOC,GPIO_PIN_7 ,GPIOB,GPIO_PIN_5 ,	0,0,0,0,&TabEncoder7_Data[0]};
Encoder Enc_8= {7,GPIOB,GPIO_PIN_13,GPIOA,GPIO_PIN_15,	0,0,0,0,&TabEncoder8_Data[0]};
Encoder Enc_M= {8,GPIOB,GPIO_PIN_1 ,GPIOC,GPIO_PIN_2 ,	0,0,0,0,&TabEncoderM_Data[0]};
#else
extern Encoder Enc_1;
extern Encoder Enc_2;
extern Encoder Enc_3;
extern Encoder Enc_4;
extern Encoder Enc_5;
extern Encoder Enc_6;
extern Encoder Enc_7;
extern Encoder Enc_8;
extern Encoder Enc_M;
#endif


ENCODER_EXTRN	void	 	Enc_Init();
ENCODER_EXTRN	uint8_t 	Enc_Timer(Encoder* enc);
ENCODER_EXTRN	void 		Enc_GetValue(Encoder* enc);

ENCODER_EXTRN	Button_S	Enc_GetButton(Encoder* enc);
ENCODER_EXTRN	void 		Enc_SetDoubleClickEnabled(Encoder* enc,uint8_t b);
ENCODER_EXTRN	uint8_t 	Enc_GetDoubleClickEnabled(Encoder* enc);
ENCODER_EXTRN	void 		Enc_SetAccelerationEnabled(Encoder* enc,uint8_t a);
ENCODER_EXTRN	uint8_t 	Enc_GetAccelerationEnabled(Encoder* enc);
ENCODER_EXTRN	int8_t 		Enc_TestClk(Encoder* enc);

#ifdef __cplusplus
}
#endif
