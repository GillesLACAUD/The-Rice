#define __MIDI__
#include "Midi.h"

#include "Grain.h"

uint16_t nextionnb=0;
uint16_t byte_cpt=0;

extern UART_HandleTypeDef huart6;
extern DMA_HandleTypeDef hdma_usart6_rx;
extern DMA_HandleTypeDef hdma_usart6_tx;

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
//void MIDIUART_DMA_Configuration(void)
//{
//DMA_InitTypeDef DMA_InitStructure;
//
//	/* UART2_RX */
//
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
//
//	DMA_DeInit(DMA1_Channel6);
//
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
//	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)MidiUartBufferRx;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//	DMA_InitStructure.DMA_BufferSize = sizeof(MidiUartBufferRx) - 1;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
//
//	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
//
//	NVIC_EnableIRQ(DMA1_Channel6_IRQn);
//
//	/* Enable DMA Stream Transfer Complete interrupt */
//	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);
//	DMA_Cmd(DMA1_Channel6, ENABLE);
//
//}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
void MIDIUART_Send(uint32_t* send,uint32_t size)
{
	HAL_UART_Transmit(&huart6,send,size,0xFFFF);
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
void MIDIUART_SendCC(uint8_t cc,uint8_t val)
{
	MidiUartBufferTx[0] = MIDI_CONTROL_CHANGE;
	MidiUartBufferTx[1] = cc;
	MidiUartBufferTx[2] = val;

	MIDIUART_Send(MidiUartBufferTx[0],3);
}


/**************************************************/
/* Trig a Key for test                            */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint16_t Fct_Trig_Key(uint8_t voice,uint16_t val)
{
	// Key On
	if(val)
	{
		str_Sound_Work.str_ADSR_AMP_onevoice[voice].u8_stateADSR = GRAIN_ADSR_STATE_ATTACK;
		str_Sound_Work.str_ADSR_AMP_onevoice[voice].u16_cpt = 0;
		str_Sound_Work.str_ADSR_AMP_onevoice[voice].i16_vol = 0;
		str_Sound_Work.str_ADSR_AMP_onevoice[voice].f_vol = 0;
	}
	// Key Off
	else
	{
		str_Sound_Work.str_ADSR_AMP_onevoice[voice].u8_stateADSR = GRAIN_ADSR_STATE_RELEASE;
	}
}

/*****************************************************************************************/
/* MIDI FUNCTIONS                                                                        */
/*****************************************************************************************/
double calculateMidiNoteRatio( int midiNote )
{
    int distanceFromCenter = midiNote - 60; // 60 is the central midi note

    if ( distanceFromCenter < 0 )
    {
        int diffAmount = -distanceFromCenter;
        int octaves = diffAmount / 12;
        int intervals = diffAmount % 12;

        return pow( 0.5, octaves ) / chromaticRatios[intervals];
    }
    else
    {
        int octaves = distanceFromCenter / 12;
        int intervals = distanceFromCenter % 12;
        return pow( 2, octaves ) * chromaticRatios[intervals];
    }
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
void MIDISendNote(uint8_t note,uint8_t vel,uint8_t slot)
{
float pitch;

	if(vel)
	{
		// Drone Mode
		if(slot<0xFF)
		{
			pitch =calculateMidiNoteRatio(str_Sound_Work.MIDIDroneNote[slot]);
			pitch *=(1+(float)str_Sound_Work.MIDIDroneDetune[slot]/100);
			str_Sound_Work.str_VOICE_Tab[slot].d_speed=pitch;
			str_Sound_Work.str_VOICE_Tab[slot].u32_speed=pitch*1000;
			for(uint8_t grain=0;grain<GRAIN_MAX_POLY;grain++)
			 {
				str_Sound_Work.str_tabgrain[slot][grain].u16_cptAR = 0;
				//str_Sound_Work.str_tabgrain[0][0].u8_stateAR=GRAIN_AR_STATE_STANDBY;
			}
			for(uint8_t grain=0;grain<str_Sound_Work.u8_grainnb;grain++)
				str_Sound_Work.str_tabgrain[slot][grain].u8_stateAR=GRAIN_AR_STATE_ATTACK;

			str_Sound_Work.str_VOICE_Tab[slot].u8_actif = 1;
			Fct_Trig_Key(slot,1);
			Midi_Tab_Note[slot] = str_Sound_Work.MIDIDroneNote[slot];
		}
		else
		{
			// Search the first free slot
			for(uint8_t n=0;n<GRAIN_MAX_POLY;n++)
			{
				if(Midi_Tab_Note[n]==0xFF)
				{
					pitch =calculateMidiNoteRatio(note);
					pitch *=(1+(float)str_Sound_Work.MIDIDroneDetune[n]/100);
					str_Sound_Work.str_VOICE_Tab[n].d_speed=pitch;
					str_Sound_Work.str_VOICE_Tab[n].u32_speed=pitch*1000;
					for(uint8_t grain=0;grain<GRAIN_MAX_POLY;grain++)
					 {
						str_Sound_Work.str_tabgrain[n][grain].u16_cptAR = 0;
						//str_Sound_Work.str_tabgrain[0][0].u8_stateAR=GRAIN_AR_STATE_STANDBY;
					}
					for(uint8_t grain=0;grain<str_Sound_Work.u8_grainnb;grain++)
						str_Sound_Work.str_tabgrain[n][grain].u8_stateAR=GRAIN_AR_STATE_ATTACK;

					str_Sound_Work.str_VOICE_Tab[n].u8_actif = 1;
					Fct_Trig_Key(n,1);
					Midi_Tab_Note[n]=note;
					break;
				}
			}
		}

	}
	else
	{
		//search the voice to release
		if(slot<0xFF)
		{
			Fct_Trig_Key(slot,0);
		}
		else
		{
			for(uint8_t n=0;n<GRAIN_MAX_POLY;n++)
			{
				if(note==Midi_Tab_Note[n])
				{
					Fct_Trig_Key(n,0);
					break;
				}
			}
		}
	}
	return;
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
void MIDIChangeProgram(uint8_t prog)
{
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
void MIDISend_PitchBend(uint8_t source,uint8_t lsb,uint8_t msb)
{
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
void MIDIRelease_FootSwitch()
{
}


/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
void MIDIControlChange(uint8_t control,uint8_t val)
{
	switch(control)
	{
		//-----------------------------------------------------------------
		// GLOBAL
		//-----------------------------------------------------------------
		case CC_SUSPEDAL:
		if( val<=63)
		{
			FootSwitch=0;
			MIDIRelease_FootSwitch();
		}
		else
			FootSwitch=1;
		break;

		case CC_MOD_WHEEL:
		break;

		//! Aftertouch
		case 0x99:
		break;

	}
	return;
}



/*****************************************************************************************/
/* MIDI PARSER                                                                           */
/*****************************************************************************************/

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
uint8_t MIDITailleMessage(uint8_t code)
{
uint8_t ret;

	switch(code)
	{
		case MIDI_NOTE_ON:
		case MIDI_NOTE_OFF:
		case MIDI_AFTERTOUCHPOLY:
		case MIDI_CONTROL_CHANGE:
		case MIDI_PITCH_BEND:
		case MIDI_SONGPOS:
		ret=2;
		break;

		case MIDI_PROGRAM_CHANGE:
		case MIDI_SONGSELECT:
		case MIDI_TIMECODE:
		case MIDI_AFTERTOUCH:
		ret =1;
		break;

		case MIDI_SYSTEM_EXCLUSIVE:
		ret=0;
		break;

		case MIDI_CLOCK:
		case MIDI_SYSTEM_RESET:
		case MIDI_END_OF_EXCLUSIVE:
		case MIDI_START:
		case MIDI_CONTINUE:
		case MIDI_STOP:
		case MIDI_ACTIVESENS:
		case MIDI_TUNEREQUEST:
		ret = 0;
		break;

		default:
		ret = 0;
		break;
	}
	return(ret);
}


/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
void MIDIReceptionMessage()
{
int  i = 0;
uint16_t remainbuff;
static uint16_t lastremainbuff = MIDI_UART_BUFFER_SIZE;
uint8_t getnewcar[MIDI_UART_BUFFER_SIZE];
uint16_t nbcar;

	//-----------------------------------------------------
	// GET THE NEW INPUT CAR IN THE getnewcar tab
	//-----------------------------------------------------
	//HAL_UART_Receive(&huart6,&MidiUartBufferRx[0],3,0x01);
	// Get the remaining data in the DMA Buffer RX USART2
	remainbuff = hdma_usart6_rx.Instance->NDTR;

	nbcar = 0;
	// Circular buffer -> restart the buffer
	if(remainbuff > lastremainbuff)
	{
		for(i=0;i<lastremainbuff;i++)
		{
			getnewcar[i] = MidiUartBufferRx[i+(MIDI_UART_BUFFER_SIZE-lastremainbuff)-1];
			nbcar++;
		}
		for(i=0;i<(MIDI_UART_BUFFER_SIZE-remainbuff-1);i++)
		{
			getnewcar[i+lastremainbuff] = MidiUartBufferRx[i];
			nbcar++;
		}
	}
	else
	{
		for(i=0;i<(lastremainbuff-remainbuff);i++)
		{
			getnewcar[i] = MidiUartBufferRx[i+(MIDI_UART_BUFFER_SIZE-lastremainbuff)-1];
			nbcar++;
		}
	}
	lastremainbuff = remainbuff;

	//-----------------------------------------------------
	// Parse the getnewcar
	//-----------------------------------------------------
	for(i=0;i<nbcar;i++)
	{
		//if(getnewcar[i] != MIDI_CLOCK)
		//	MIDIUART_Send(&getnewcar[i],1);	// Echo Midi soft Thru - Not in DMA Mode can be long !
		HAL_UART_Transmit(&huart6,&getnewcar[i],1,0xFFFF);


		/*
		if(!gu8_MidiIn)
		{
			gu16_TimerMidiIn=0;
			gu8_MidiIn=1;
			sprintf(messnex,"page1.Midi.pco=49154");
			Nextion_Send(messnex);
		}
		*/

		// COMMAND MESSAGE
		if((getnewcar[i] & 0x080))
		{
			// No sysex or Clock or Real Time message
			if(getnewcar[i] < 0x0f0)
			{
				Midi_Status_Message = getnewcar[i] & 0xF0;	// MSB is the message
				Midi_Rx_Channel		= getnewcar[i] & 0x0F;	// LSB is the midi channel
				CptFinMess = MIDITailleMessage(Midi_Status_Message);
			}
			// Sysex Midi clock and Real time
			else
			{

			}
		}
		// DATA MESSAGE
		else
		{
			if(OctReception	< CptFinMess)
			{
				recArray[OctReception] = getnewcar[i];
				OctReception++;
			}
			if(OctReception	== CptFinMess)
			{
				// it's the good midi channel
				if((gui8_MidiRxCh-1) == Midi_Rx_Channel)
				{
					if(OctReception == 2)
					{
						switch(Midi_Status_Message)
						{
							case MIDI_NOTE_OFF:
							MIDISendNote(recArray[0],0,0xFF);
							OctReception = 0;
							break;

							case MIDI_NOTE_ON:
							MIDISendNote(recArray[0],recArray[1],0xFF);
							OctReception = 0;
							break;

							case MIDI_CONTROL_CHANGE:
							MIDIControlChange(recArray[0],recArray[1]);
							OctReception = 0;
							break;

							case MIDI_PITCH_BEND:
							MIDISend_PitchBend(1,recArray[0],recArray[1]);
							OctReception = 0;
							break;

							default:
							OctReception = 0;
							break;
						}
					}
					if(OctReception == 1)
					{
						switch(Midi_Status_Message)
						{
							case MIDI_PROGRAM_CHANGE:
							MIDIChangeProgram(recArray[0]);
							OctReception = 0;
							break;

							case MIDI_AFTERTOUCH:
							MIDIControlChange(0x99,recArray[0]);
							OctReception = 0;
							break;

							default:
							OctReception = 0;
							break;
						}
					}
				}
				else
					OctReception = 0;
			}
		}
	}
	return;
}





