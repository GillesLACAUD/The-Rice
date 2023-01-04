#ifdef __cplusplus
    extern "C" {
    #endif


#include "main.h"
#include "grain.h"

#ifdef __MIDI__
#define MIDI_EXTRN
#else
#define MIDI_EXTRN 	extern
#endif


// definition messages MIDI

#define MIDI_CONTROL_CHANGE                  0xB0
#define MIDI_SYSTEM_EXCLUSIVE                0xF0
#define MIDI_END_OF_EXCLUSIVE                0xF7
#define MIDI_NOTE_OFF                        0x80
#define MIDI_NOTE_ON                         0x90

#define MIDI_PROGRAM_CHANGE                  0xC0
#define MIDI_AFTERTOUCH		                 0xD0
#define MIDI_PITCH_BEND                      0xE0

#define MIDI_MOD_WHEEL	                     0xD0

#define MIDI_SYSTEM_RESET                    0xFF
#define MIDI_ALLNOTE_OFF                     0xB0

#define MIDI_CLOCK			                 0xF8
#define MIDI_START			                 0xFA
#define MIDI_CONTINUE			             0xFB
#define MIDI_STOP  							 0xFC

#define MIDI_SONGPOS						 0xF2
#define MIDI_SONGSELECT						 0xF3

#define MIDI_ACTIVESENS						 0xFE
#define MIDI_TUNEREQUEST					 0xF6
#define MIDI_SONGSELECT						 0xF3
#define MIDI_AFTERTOUCHPOLY					 0xA0

#define MIDI_TIMECODE						 0xF1


#define MIDI_SYSEX_SOUND					 0x01
#define MIDI_SYSEX_SETUP					 0x02
#define MIDI_SYSEX_SYSTEM					 0x03
#define MIDI_SYSEX_TFT_SIZE					 0x04
#define MIDI_SYSEX_TFT_BLOCK				 0x05
#define MIDI_SYSEX_TFT_LASTBLOCK			 0x06


#define		CC_MOD_WHEEL	1
#define		CC_SUSPEDAL		64
#define		CC_PORTAMENTO	5


#define MIDI_UART_BUFFER_SIZE				256

MIDI_EXTRN volatile char MidiUartBufferRx[MIDI_UART_BUFFER_SIZE];
MIDI_EXTRN uint8_t MidiUartBufferTx[MIDI_UART_BUFFER_SIZE];
MIDI_EXTRN volatile char MidiUartBufferOneTx;

MIDI_EXTRN uint8_t MidiTxinthedma;
MIDI_EXTRN uint8_t MidiMutex;

MIDI_EXTRN uint8_t MidiInTempo;

MIDI_EXTRN uint16_t gu16_TimerMidiClock;
MIDI_EXTRN uint8_t  gu8_MidiClock;
MIDI_EXTRN uint8_t  gu8_MidiCptClock;

MIDI_EXTRN uint8_t	FootSwitch;


#ifdef  __MIDI__
uint8_t	Midi_Status_Message =0;
uint8_t	Midi_Rx_Channel = 1;			// Midi channel receipt
uint8_t	gui8_MidiRxCh = 1;				// Midi rx channel set by the user
uint8_t	CptFinMess=0;
uint8_t	Midi_Echo = 1;
uint8_t	OctReception=0;

double chromaticRatios[] = {
    1,
    1.0594630943591,
    1.1224620483089,
    1.1892071150019,
    1.2599210498937,
    1.3348398541685,
    1.4142135623711,
    1.4983070768743,
    1.5874010519653,
    1.6817928305039,
    1.7817974362766,
    1.8877486253586
};


#else
MIDI_EXTRN double chromaticRatios[];
MIDI_EXTRN uint8_t	Midi_Status_Message;
MIDI_EXTRN uint8_t	Midi_Rx_Channel;
MIDI_EXTRN uint8_t	gui8_MidiRxCh;
MIDI_EXTRN uint8_t	CptFinMess;
MIDI_EXTRN uint8_t	Midi_Echo;
MIDI_EXTRN uint8_t	OctReception;
#endif

MIDI_EXTRN uint8_t	recArray[20];

/*
MIDI_EXTRN void MIDIUART_DMA_Tx(uint8_t size);
*/

#define SYSEX_STANDBY		0
#define SYSEX_GET			1
#define SYSEX_READY			2

MIDI_EXTRN char 	SysEx_Receive;
MIDI_EXTRN uint8_t 	SysEx_Get;
MIDI_EXTRN uint8_t 	SysEx_Cpt;
MIDI_EXTRN char 	SysEx_Tab[255];

MIDI_EXTRN uint8_t Midi_Tab_Note[GRAIN_MAX_POLY];

MIDI_EXTRN double calculateMidiNoteRatio( int midiNote );
MIDI_EXTRN void MIDIUART_Send(uint32_t* send,uint32_t size);
MIDI_EXTRN void MIDIUART_SendCC(uint8_t cc,uint8_t val);
MIDI_EXTRN void MIDISendNote(uint8_t note,uint8_t vel,uint8_t slot);
MIDI_EXTRN void MIDIChangeProgram(uint8_t prog);
MIDI_EXTRN void MIDISend_PitchBend(uint8_t source,uint8_t lsb,uint8_t msb);
MIDI_EXTRN void MIDIRelease_FootSwitch();
MIDI_EXTRN void MIDIControlChange(uint8_t control,uint8_t val);
MIDI_EXTRN uint8_t MIDITailleMessage(uint8_t code);
MIDI_EXTRN void MIDIReceptionMessage();

#ifdef __cplusplus
}
#endif
