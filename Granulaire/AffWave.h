#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "grain.h"
#include "wave.h"



#ifndef APPLICATION_USER_AFF_H_
#define APPLICATION_USER_AFF_H_

#define	MAX_AFF_WAX_X	600
#define	MAX_AFF_WAX_Y	100



uint32_t	u32_aff_filesize;
uint16_t	u16_aff_factory;

uint32_t	u32_aff_echperpixel;
uint16_t 	u16_aff_buffercapture;

float		f_aff_divx;
float		f_aff_pixelperpoint;

uint32_t	ui32_aff_pos_buffer;

typedef struct
{
uint32_t	u32_aff_backcolor;
uint32_t	u32_aff_color;
uint32_t	u32_frame_color;

uint8_t		u8_aff_grid;

uint16_t	u16_aff_offsetx;			// Start point
uint16_t	u16_aff_offsety;			// Start point
uint16_t	u16_aff_width;				// Wide
uint16_t	u16_aff_height;				// Height

}str_Frame;

#define AFF_FILE_MAXPIXEL	800

uint16_t	Tab_AffFile[AFF_FILE_MAXPIXEL];
uint16_t	Tab_AffBigBuffer[AFF_FILE_MAXPIXEL];
uint32_t	ui32_PowerBuffer;

str_Frame	str_Frame_Waveform;
str_Frame	str_Frame_Grain;

uint32_t	u32_aff_nbslice;	// Slice number in the file
uint32_t	u32_aff_oneslice; 	// For the Display warning *1000

uint8_t 	u8_aff_cptgrain;


uint8_t Aff_Display_BigBufferPos(uint16_t pos);								// Display the postion of the big buffer in the file view
uint8_t Aff_Display_BigBufferSelection(uint32_t* pos,uint32_t* size,uint8_t modulator);		// Display the selected part of the big buffer in the buffer view


uint8_t Aff_Display_Grain(uint8_t g);
uint8_t Aff_Display_Play(uint32_t x,uint32_t echppx);
uint8_t Aff_Display_BigBuffer();
uint8_t Aff_Display_OutBuffer();
uint8_t Aff_Display_Redraw(uint16_t* buff);
uint8_t Aff_Display_Label(uint8_t* l1,uint8_t* l2,uint8_t* l3,uint8_t* l4);
uint8_t Aff_Set_Param_Waveform();
uint8_t Aff_Display_Framework(str_Frame* frame);
uint8_t Aff_Display_AllValues();
uint8_t Aff_Display_FileNames();
uint8_t Aff_Display_WavetoBuffer(uint8_t id,WAVE_FormatTypeDef format);

#endif /* APPLICATION_USER_AFF_H_ */

#ifdef __cplusplus
}
#endif
