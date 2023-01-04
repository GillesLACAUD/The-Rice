#ifdef __cplusplus
    extern "C" {
    #endif

//---------------------------------------------------------------------------------
//
// Enc_Timer(ENCODER_X) 		in a timer funtion
// Enc_GetValue(Encoder* enc)	in a task
//
//
//
//---------------------------------------------------------------------------------

#define __ENCODER__
#include "Encoder.h"

extern uint8_t u8_selectsection;

// ----------------------------------------------------------------------------
// Button configuration (values for 1ms timer service calls)
//
#define ENC_BUTTONINTERVAL    10  	// check button every x milliseconds, also debouce time
#define ENC_DOUBLECLICKTIME  6000  	// second click within 600ms
#define ENC_HOLDTIME        20000  	// report held button after 2s

// ----------------------------------------------------------------------------
// Acceleration configuration (for 1000Hz calls to ::service())
//
#define ENC_ACCEL_TOP        300   // max. acceleration: *12 (val >> 8)
#define ENC_ACCEL_INC        80
#define ENC_ACCEL_DEC        80


/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
Button_S Enc_GetButton(Encoder* enc)
{
}
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void Enc_SetDoubleClickEnabled(Encoder* enc,uint8_t b)
{
}
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
uint8_t Enc_GetDoubleClickEnabled(Encoder* enc)
{
}
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void Enc_SetAccelerationEnabled(Encoder* enc,uint8_t a)
{
}
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
uint8_t Enc_GetAccelerationEnabled(Encoder* enc)
{
}
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void Enc_Init()
{
GPIO_InitTypeDef GPIO_InitStructure = { 0 };
Encoder* enc;
Encoder_Data* encdata;
uint8_t en,i;

	for(en=0;en<8;en++)
	{
		switch(en)
		{
			case 0:	encdata = &TabEncoder1_Data[0];break;
			case 1:	encdata = &TabEncoder2_Data[0];break;
			case 2:	encdata = &TabEncoder3_Data[0];break;
			case 3:	encdata = &TabEncoder4_Data[0];break;
			case 4:	encdata = &TabEncoder5_Data[0];break;
			case 5:	encdata = &TabEncoder6_Data[0];break;
			case 6:	encdata = &TabEncoder7_Data[0];break;
			case 7:	encdata = &TabEncoder8_Data[0];break;
		}
		for(i=0;i<6;i++)
		{
			encdata->Delta = encdata->MaxData - encdata->MinData;
			encdata->CoeffCircle = 1000/(encdata->Delta);
			encdata++;
		}
	}
	// TODO Set the GPIO in the IOC file 

	// TODO INIT THE ENCODER NAME
	for(uint8_t e=0;e<8;e++)
	{
		strcpy(TabEncoder1_Data[e].Name,Tab_Label[e+0*8]);
		strcpy(TabEncoder2_Data[e].Name,Tab_Label[e+1*8]);
		strcpy(TabEncoder3_Data[e].Name,Tab_Label[e+2*8]);
		strcpy(TabEncoder4_Data[e].Name,Tab_Label[e+3*8]);
		strcpy(TabEncoder5_Data[e].Name,Tab_Label[e+4*8]);
		strcpy(TabEncoder6_Data[e].Name,Tab_Label[e+5*8]);
		strcpy(TabEncoder7_Data[e].Name,Tab_Label[e+6*8]);
		strcpy(TabEncoder8_Data[e].Name,Tab_Label[e+7*8]);
	}
	
	// Can be done in the io file
	if(1)
	{
		// Set the GPIO in Pull up mode
		// PA01 PA02 PA05 PA07 PA08 PA15
		// PB01 PB04 PB05 PB13 PB15
		// PC02 PC06 PC07
		// PG10 PG12 PG13
		// PH06
		__HAL_RCC_GPIOA_CLK_ENABLE();
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		GPIO_InitStructure.Pin   = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_15;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitStructure.Pin   = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_13 | GPIO_PIN_15;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

		__HAL_RCC_GPIOC_CLK_ENABLE();
		GPIO_InitStructure.Pin   = GPIO_PIN_2 | GPIO_PIN_6 | GPIO_PIN_7;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

		__HAL_RCC_GPIOG_CLK_ENABLE();
		GPIO_InitStructure.Pin   = GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_13;
		HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

		__HAL_RCC_GPIOH_CLK_ENABLE();
		GPIO_InitStructure.Pin   = GPIO_PIN_6;
		HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
	}
}


//                           _______         _______
//               Pin1 ______|       |_______|       |______ Pin1
// negative <---         _______         _______         __      --> positive
//               Pin2 __|       |_______|       |_______|   Pin2
//					  0	  1	  2	  3   4   5   6   7   8

		// 		old	old	New	New
		// 		P1	P2	P1	P2
		// 00	0   0   0   1	0x01	Positive
		// 01	0	1	1 	1	0x07
		// 02	1	1	1	0	0x0E
		// 03	1	0	0	0	0x08

		// 04	0	0	1	0	0x02	Negative
		// 05   1	0	1	1	0x0B
		// 06	1	1	0	1	0x0C
		// 07	0	1	0	0	0x04

uint8_t NewA=0;
uint8_t NewB=0;
uint8_t	tab_index=0;
uint8_t	oldtab_index=0;

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
uint8_t bifEnc_Timer(Encoder* enc)
{
Encoder_Data *ptenc;

	tab_index=0;

	ptenc = &Enc_1.data;
	ptenc +=(u8_selectsection);

	NewA = HAL_GPIO_ReadPin(enc->GPIO_A,enc->GPIO_Pin_A);
	NewB = HAL_GPIO_ReadPin(enc->GPIO_B,enc->GPIO_Pin_B);

	// Get the next inputs only if there is a change on the A or B pin
	if((NewA != enc->OldA) || (NewB != enc->OldB) && NewA==1)
	{
		// Build the tab index with old and new values
		if(NewA)
			tab_index +=2;
		if(NewB)
			tab_index +=1;
		if(enc->OldB)
			tab_index +=8;
		if(enc->OldA)
			tab_index +=4;

		// Set old val to new val
		enc->OldA = NewA;
		enc->OldB = NewB;

		if((tab_index==0x01 && oldtab_index==0x08)|| (tab_index==0x07 && oldtab_index==0x01) || (tab_index==0x0E && oldtab_index==0x07)|| (tab_index==0x08 && oldtab_index==0x0E))
		{
			Tab_Val[enc->id+u8_selectsection*8]++;
			Tab_Enc_Move[enc->id]=1;
		}
		if((tab_index==0x02 && oldtab_index==0x04)|| (tab_index==0x0B && oldtab_index==0x02) || (tab_index==0x0C && oldtab_index==0x0B)|| (tab_index==0x04 && oldtab_index==0x0C))
		{
			Tab_Val[enc->id+u8_selectsection*8]--;
			Tab_Enc_Move[enc->id]=1;
		}

		oldtab_index = tab_index;

		if(Tab_Enc_Move[enc->id])
		{
			// Check the min and the max
			/*
			if(enc!=&Enc_M)
			{
				if(Tab_Val[enc->id+u8_selectsection*8]<ptenc->MinData)
				{
					Tab_Val[enc->id+u8_selectsection*8]=ptenc->MinData;
					Tab_Enc_Move[enc->id]=0;
				}
				if(Tab_Val[enc->id+u8_selectsection*8]>ptenc->MaxData)
				{
					Tab_Val[enc->id+u8_selectsection*8]=ptenc->MaxData;
					Tab_Enc_Move[enc->id]=0;
				}
			}
			else
			{
				ptenc = enc->data;
				int min=ptenc->MinData;
				int max=ptenc->MaxData;
				if(Tab_Section<min)
				{
					Tab_Section=min;
					Tab_Enc_Move[enc->id]=0;
				}
				if(Tab_Section>max)
				{
					Tab_Section=max;
					Tab_Enc_Move[enc->id]=0;
				}
			}
			*/
		}
	}
}
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
uint8_t Enc_Timer(Encoder* enc)
{
uint32_t t2;

uint8_t NewB=0;
uint8_t inc;
Encoder_Data *ptenc;

	NewA = HAL_GPIO_ReadPin(enc->GPIO_A,enc->GPIO_Pin_A);
	if(NewA !=enc->OldA)
	{
		enc->OldA=NewA;
		enc->CptEdge++;
		if(enc->CptEdge==1)
		{
			if(NewA)
			{
				enc->t1 = HAL_GetTick();
				NewB=HAL_GPIO_ReadPin(enc->GPIO_B,enc->GPIO_Pin_B);
			}
			else
			{
				enc->CptEdge=0;
			}
		}
		if(enc->CptEdge==2 && !NewA)
		{
			if(!NewA)
			{
				NewB=HAL_GPIO_ReadPin(enc->GPIO_B,enc->GPIO_Pin_B);
				enc->CptEdge=0;
				moved = 1;
				Tab_Enc_Move[enc->id]=1;
				t2 = HAL_GetTick();
				t2 -=enc->t1;

				ptenc = enc->data;
				ptenc +=(u8_selectsection);

				if(t2<5)
					inc=ptenc->Delta/8;			//
				else if(t2<10)
					inc=ptenc->Delta/10;		//
				else if(t2 < 20)
					inc=ptenc->Delta/15;		//
				else if(t2 < 40)
					inc=ptenc->Delta/20;		//
				else
					inc=ptenc->Delta/1000;		//

				if(!inc)
					inc=1;

				if(enc==&Enc_M)
					inc=1;

				if(NewB)
				{
					if(enc==&Enc_M)
						Tab_Section+=inc;
					else
					{
						if(enc->OldB==NewB)
						{
							Tab_Val[enc->id+u8_selectsection*8]-=inc;
						}
						else
							Tab_Val[enc->id+u8_selectsection*8]-=1;
					}
				}
				else
				{
					if(enc==&Enc_M)
						Tab_Section-=inc;
					else
					{
						if(enc->OldB==NewB)
						{
							Tab_Val[enc->id+u8_selectsection*8]+=inc;
						}
						else
							Tab_Val[enc->id+u8_selectsection*8]+=1;
					}
				}

				// Check the min and the max
				if(enc!=&Enc_M)
				{
					if(Tab_Val[enc->id+u8_selectsection*8]<ptenc->MinData)
					{
						Tab_Val[enc->id+u8_selectsection*8]=ptenc->MinData;
					}
					if(Tab_Val[enc->id+u8_selectsection*8]>ptenc->MaxData)
					{
						Tab_Val[enc->id+u8_selectsection*8]=ptenc->MaxData;
					}
				}
				else
				{
					ptenc = enc->data;
					int min=ptenc->MinData;
					int max=ptenc->MaxData;
					if(Tab_Section<min)
					{
						Tab_Section=min;
					}
					if(Tab_Section>max)
					{
						Tab_Section=max;
					}
				}
				enc->OldB=NewB;
			}
		}
	}
	return(moved);
}

int16_t tstenc;

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
int8_t Enc_TestClk(Encoder* enc)
{
uint8_t moved=0;

	return(moved);
}



/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void Enc_GetValue(Encoder* enc)
{
//int16_t val;
//uint8_t param;
//int signe;
//int tmp;


//	if(enc->delta !=0)
//	{
//		param = Tab_Enc_Param[enc->id];
//		enc->val +=enc->delta*enc->data[param].Step;
//
//		//---------------------------------------
//		// Min Max Value
//		//---------------------------------------
//		if(enc->val < enc->data[param].MinData)
//		{
//			enc->val = enc->data[param].MinData;
//			enc->delta = enc->data[param].MinData*enc->steps;
//		}
//		if(enc->val > enc->data[param].MaxData)
//		{
//			enc->val = enc->data[param].MaxData;
//			enc->delta = enc->data[param].MaxData*enc->steps;
//		}
//		if(enc->oldval != enc->val)
//		{
//			enc->oldval = enc->val;
//			enc->data[param].ptrfunctValueChange(enc->id);
//			*enc->data->Data = enc->val;
//		}
//		enc->delta = 0;
//	}
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
int Fct_ADRS1_Attack(int val)
{
	return val;
}

#ifdef __cplusplus
}
#endif

