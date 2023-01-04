//*************************************************************************************
//
// 1 Big buffer with 0.5 to 1s of second
// 1 Buffer to store the display of all the file
// 1 Buffer to store the display of the selected portion
//
//
//*************************************************************************************

#include "main.h"

#define	__WAVE__
#include "wave.h"
#include "AffWave.h"
#include "WavePlayer.h"
#include "UsbStorage.h"
#include "ff.h"
#include "Matrix.h"


FIL WavFile;
FILELIST_FileTypeDef FileList;
FILELIST_FileTypeDef SoundList;

/***************************************************************************************/
/*                                                                                     */
/* FRAMEWORK                                                                           */
/*                                                                                     */
/***************************************************************************************/

/**************************************************/
/* Set the param for the waveform frame           */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Aff_Set_Param_Waveform()
{
	str_Frame_Waveform.u16_aff_offsetx		=0;
	str_Frame_Waveform.u16_aff_offsety		=0;
	str_Frame_Waveform.u16_aff_width		=AFF_FILE_MAXPIXEL;
	str_Frame_Waveform.u16_aff_height		=100;

	f_aff_pixelperpoint = (float)str_Frame_Waveform.u16_aff_width/(float)TOUCHGUI_POT_MAX;
	return(0);
}

/**************************************************/
/* Set the param for a grain frame                */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Aff_Set_Param_Grain()
{
	return(0);
}

/**************************************************/
/* Display the framework                          */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Aff_Display_Framework(str_Frame* frame)
{
	return(0);
}

/***************************************************************************************/
/*                                                                                     */
/* DISPLAY POSITION                                                                    */
/*                                                                                     */
/***************************************************************************************/
/**************************************************/
/* Display the pos of a buffer in the file        */
/* Input:                                         */
/* pos in the file                                */
/*                                                */
/**************************************************/
uint8_t Aff_Display_BigBufferPos(uint16_t pos)
{
uint16_t x1,y1;
uint16_t w,h;

	uint32_t restore = BSP_LCD_GetTextColor();

	pos >>=1;

	x1=str_Frame_Grain.u16_aff_offsetx+2;		// Set the x start
	y1=str_Frame_Grain.u16_aff_offsety+10;		// Set the y start
	h=10;										// Set the height of the display

	//size = 176400 -> 1 sec 16 bits 2 Channel 44100 -> 44100*2*2
	w = GRAIN_BIGBUFFER_SIZE/u32_aff_echperpixel;
	w /=(WaveFormat.BitPerSample/8);
	w /=WaveFormat.NbrChannels;

	if(u8_Type_BigBuffer == BUFFER_MONO)
		w *=2;

	if((x1+pos+w)<str_Frame_Waveform.u16_aff_width)
	{
		// Clear the old pos
//		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
//		BSP_LCD_FillRect(x1,y1,str_Frame_Waveform.u16_aff_width-2,h+3);
//
//		// Display the pos
//		BSP_LCD_SetTextColor(COLOR_BIGBUFFERPOS);
//
//		if(!u8_Grain_TrigBigBuffer_Full)
//			BSP_LCD_DrawRect(x1+pos,y1+1,w,h);
//		else
//		{
//			BSP_LCD_FillRect(x1+pos,y1+1,w,h);
//		}
//
//		BSP_LCD_SetTextColor(restore);
		return(0);
	}
	return(1);
}


/**************************************************/
/* Display the selection in the buffer            */
/* Input:  Modulator                              */
/* Output: Return the pos and the size of the view*/
/*                                                */
/**************************************************/
uint8_t Aff_Display_BigBufferSelection(uint32_t* pos,uint32_t* size,uint8_t modulator)
{
	if(modulator)
	{
		// After the Matrix mod dynamic view of the buffer
		*pos=u32_Mod_Grain_Begin;
		*size=u32_Mod_Grain_Size;
	}
	else
	{
		// Before the Matrix mod static view
		*pos=str_Sound_Work.u32_Grain_Begin;
		*size=str_Sound_Work.u32_Grain_Size;
	}

	if(u8_Type_BigBuffer == BUFFER_MONO)
	{
		*pos 	/=2;
		*size 	/=2;
	}
	else
	{
		*pos 	/=4;
		*size 	/=4;
	}

	*pos 	/=u32_aff_echperpixel;
	*size 	/=u32_aff_echperpixel;

	if((*pos+*size) > str_Frame_Waveform.u16_aff_width-5)
	{
		*size = str_Frame_Waveform.u16_aff_width-*pos-5;
	}
	return(0);
}
/**************************************************/
/* Display the play bar                           */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Aff_Display_Play(uint32_t x,uint32_t echppx)
{
	return(0);
}


/**************************************************/
/* Display the Grain                              */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Aff_Display_Grain(uint8_t g)
{
	return(0);
}



/***************************************************************************************/
/*                                                                                     */
/* DISPLAY WAVEFORM                                                                    */
/*                                                                                     */
/***************************************************************************************/

/**************************************************/
/* Display the waveform of the bigbuffer          */
/* Store the waveform in a buffer                 */
/* Display the buffer                             */
/*                                                */
/**************************************************/
uint8_t Aff_Display_OutBuffer()
{
	return(0);
}



/**************************************************/
/* Display the waveform of the bigbuffer          */
/* Store the waveform in a buffer                 */
/* Display the buffer                             */
/*                                                */
/**************************************************/
uint8_t Aff_Display_BigBuffer()
{
uint32_t bytesread;
uint8_t buff[20];
uint16_t cptech;
int16_t middle=50;
int16_t leftchannel2;
int16_t x1,x2,y1,y2;
uint32_t offset=0;

	u32_aff_nbslice		= GRAIN_BIGBUFFER_SIZE/AUDIO_OUT_BUFFER_SIZE;
	u32_aff_oneslice	= str_Frame_Waveform.u16_aff_width*1000/u32_aff_nbslice;
	f_aff_divx = (float)u32_aff_nbslice/(float)TOUCHGUI_POT_MAX;

	cptech=0;
	if(u8_Type_BigBuffer == BUFFER_STEREO)
	{
		memcpy(&buff[0],&u8_Tab_BigBuffer[cptech*4],4);
		u32_aff_echperpixel = GRAIN_BIGBUFFER_SIZE;
		u32_aff_echperpixel /=4;
		u32_aff_echperpixel /=str_Frame_Waveform.u16_aff_width;
	}
	else
	{
		memcpy(&buff[0],&u8_Tab_BigBuffer[cptech*2],2);
		u32_aff_echperpixel = GRAIN_BIGBUFFER_SIZE;
		u32_aff_echperpixel /=2;
		u32_aff_echperpixel /=str_Frame_Waveform.u16_aff_width;
	}

	// Left channel
	leftchannel2 	=*(int16_t*)&buff[0];
	cptech++;
	y1=middle;

	offset=0;
	while(cptech <AFF_FILE_MAXPIXEL-1)
	{
		if(u8_Type_BigBuffer == BUFFER_STEREO)
			memcpy(&buff[0],&u8_Tab_BigBuffer[offset],4);
		else
			memcpy(&buff[0],&u8_Tab_BigBuffer[offset],2);
		// Left channel only
		leftchannel2 	=*(int16_t*)&buff[0];
		y2=leftchannel2/u16_aff_factory;			// 398/1090 = 8174 ?????  cptech=27
		y2+=middle;
		Tab_AffBigBuffer[cptech]=y2;
		cptech++;
		if(u8_Type_BigBuffer == BUFFER_STEREO)
			offset+=u32_aff_echperpixel*4;
		else
			offset+=u32_aff_echperpixel*2;
	}
	u32_Grain_PosOffset_BigBuffer=0;
	BufferCtl.fptrstart=u32_Grain_PosStart_BigBuffer;
	BufferCtl.fptr=BufferCtl.fptrstart;
	BufferCtl.fptrend=BufferCtl.fptrstart;
	BufferCtl.fptrend +=GRAIN_BIGBUFFER_SIZE;
	BufferCtl.fptrend -=AUDIO_OUT_BUFFER_SIZE;

	//u32_Grain_Begin =0;
	//u32_Grain_Size=GRAIN_BIGBUFFER_SIZE;			// Set the First grain to the max size = full buffer

	Aff_Display_Redraw(&Tab_AffBigBuffer[0]);
	middle = 0;
	return(0);
}

/***************************************************************************************/
/*                                                                                     */
/* DISPLAY DATA                                                                        */
/*                                                                                     */
/***************************************************************************************/
#define LCD_COLOR_CYAN_TRANS          ((uint32_t) 0xAA00FFFF)
#define LCD_COLOR_BLACK_TRANS         ((uint32_t) 0xAA000000)

/**************************************************/
/* Display the framework                          */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Aff_Display_Label(uint8_t* l1,uint8_t* l2,uint8_t* l3,uint8_t* l4)
{
	return(0);
}

/**************************************************/
/* Display Redraw                                 */
/* Redraw a waveform from a buffer                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Aff_Display_Redraw(uint16_t* buff)
{
}


/**************************************************/
/* Aff_Display_WavetoBuffer                       */
/* Read from a file                               */
/* Store the waveform in a buffer                 */
/* Display the buffer                             */
/**************************************************/
uint8_t Aff_Display_WavetoBuffer(uint8_t id,WAVE_FormatTypeDef format)
{
uint32_t bytesread;
uint8_t buff[20];
uint16_t cptech;
int16_t middle;
int16_t leftchannel2;
int16_t x1,x2,y1,y2;


	middle=str_Frame_Waveform.u16_aff_offsety+str_Frame_Waveform.u16_aff_height/2;
	f_close(&WavFile);
	if(f_open(&WavFile, (char *)FileList.file[id].name, FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{

	    u32_aff_echperpixel = format.FileSize;
	    u32_aff_echperpixel /=4;
	    u32_aff_echperpixel /=str_Frame_Waveform.u16_aff_width;



	    u16_aff_factory = 32000/str_Frame_Waveform.u16_aff_height;
	    u16_aff_factory*=2;

		u32_aff_nbslice		= format.FileSize/AUDIO_OUT_BUFFER_SIZE;
		u32_aff_oneslice	= str_Frame_Waveform.u16_aff_width*1000/u32_aff_nbslice;

		// Pos for the grain in nbslice
		// str_Frame_Waveform.u16_aff_width -> u32_aff_nbslice
		// 4095                             -> u32_aff_nbslice
		// 1								-> u32_aff_nbslice/4095
		//
		//
	    f_aff_divx = (float)u32_aff_nbslice/(float)TOUCHGUI_POT_MAX;

	    /* Get Data from USB Flash Disk */
	    f_lseek(&WavFile,44);	// after the RIFF

	    cptech=0;

	    if(f_read(&WavFile,&buff,4,(void *)&bytesread) == FR_OK)
		{
    		// Left channel
			leftchannel2 	=*(int16_t*)&buff[0];
			// Right Channel
			//rightchannel2 	=*(int16_t*)&buff[2];
			cptech++;
		}
	    y1=middle;

	    //BSP_LCD_DisplayStringAt(350,80,"LOAD...",LEFT_MODE);

	    uint32_t offset=44;
	    while(cptech <AFF_FILE_MAXPIXEL-1)
	    {
	    	if(f_read(&WavFile,&buff,format.NbrChannels*2,(void *)&bytesread) == FR_OK)		// 2 for mono 4 for stereo
	    	{
	    		// Left channel
				leftchannel2 	=*(int16_t*)&buff[0];
		    	y2=leftchannel2/u16_aff_factory;			// 398/1090 = 8174 ?????  cptech=27
		    	y2+=middle;
		    	Tab_AffFile[cptech]=y2;
	    		cptech++;
		    	offset+=u32_aff_echperpixel*4;
		    	f_lseek(&WavFile,offset);
	    	}
	    }

	    u32_Grain_PosStart_BigBuffer=0;
	    u8_Waveplayer_Mode = WAVEPLAY_MODE_FILE;
	    BufferCtl.fptrstart=44;
	    BufferCtl.fptrend=(WaveFormat.FileSize-AUDIO_OUT_BUFFER_SIZE);

	    //Aff_Display_Redraw(&Tab_AffFile[0]);
	    return(0);
	}
	middle = 0;
	return(0);
}

/**************************************************/
/* Display All Values                             */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Aff_Display_AllValues()
{
	return(0);
}
/**************************************************/
/* Display All FileName                           */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t Aff_Display_FileNames()
{
	return(0);
}

