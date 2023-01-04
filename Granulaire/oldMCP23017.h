/* MCP23017 Library for mbed
 * Copyright (c) 2014, Takuya Urakawa
 * 
 * This library is released under the MIT License
 * See http://opensource.org/licenses/mit-license.php
 */

#ifdef __cplusplus
    extern "C" {
    #endif

#ifdef __MCP2307__
#define MCP23017_EXTRN
#else
#define MCP23017_EXTRN	extern
#endif

#include "main.h"

/** \def usefull defines */
#define MCP23017_PORTA 0x00
#define MCP23017_PORTB 0x01

#define MCP23017_I2C_ADDRESS		0x40	// A1 A0 to GND  Connect pins 15/16/17

// registors
enum MCP23017_REG {
	// IO Dirrection
	REG_IODIRA = 0x00,
	REG_IODIRB = 0x10,

	REG_PULLUPA = 0x06,
	REG_PULLUPB = 0x16,

	REG_INTA = 0x02,
	REG_INTB = 0x12,

	REG_VALUEA = 0x09,
	REG_VALUEB = 0x19,


};


typedef struct
{
	char _addr;
	char mWriteAddr;
	char mReadAddr;

}stMCP23017;


MCP23017_EXTRN stMCP23017 MCP23017;


MCP23017_EXTRN	uint8_t					SendToMCP;
MCP23017_EXTRN	uint8_t					FlagReadMCP;
MCP23017_EXTRN	uint16_t				MCP23017_Getpb_LSB;
MCP23017_EXTRN	uint16_t				MCP23017_Getpb_MSB;
MCP23017_EXTRN	uint8_t					MCP23017_Getreg;

MCP23017_EXTRN void MCP23017_i2cSend3(stMCP23017* mcp,char _command, char _data1, char _data2);
MCP23017_EXTRN void MCP23017_i2cSend2(stMCP23017* mcp,char _command, char _data1);
MCP23017_EXTRN char MCP23017_i2cRead(stMCP23017* mcp,char _command);
MCP23017_EXTRN char MCP23017_read(stMCP23017* mcp,char _port);
MCP23017_EXTRN void MCP23017_write(stMCP23017* mcp,char _port, char _value);
MCP23017_EXTRN void MCP23017_init(stMCP23017* mcp,char add);
MCP23017_EXTRN void MCP23017_setConfig(stMCP23017* mcpchar,char _value);
MCP23017_EXTRN void MCP23017_setDirrection(stMCP23017* mcp,char _port, char _value);
MCP23017_EXTRN void MCP23017_setPullUp(stMCP23017* mcp,char _port, char _value);
MCP23017_EXTRN void MCP23017_setInputPolarity(stMCP23017* mcp,char _port, char _value);
MCP23017_EXTRN void MCP23017_ITA_Fall();
MCP23017_EXTRN void MCP23017_ITB_Fall();
MCP23017_EXTRN void MCP23017_setInterruptConfig(stMCP23017* mcp);
MCP23017_EXTRN void MCP23017_ITinit();


/** MCP23017 class
 *
 *	Allow access MCP23017 I2C 16bit io expander IC
 *  But this library can not controll interrupt functions
 *
 *  Example:
 *  @code
 *  #include "mbed.h"
 *  #include "MCP23017.h"
 *
 *  I2C i2c(dp5, dp27); // LPC1114
 *  MCP23017::MCP23017 io(&i2c,MCP23017_DEFAULT_ADDR);
 *
 *  int main(void)
 *  {	
 *	  i2c.frequency(400000);
 *	
 *	  // software reset
 *	  io.init();
 *	
 *	  io.setDirrection(MCP23017_PORTA, 0xFF);    // set all of the PORTA pins to input
 *	  io.setDirrection(MCP23017_PORTB, 0x00);    // set all of the PORTB pins to output
 *	  io.setPullUp(MCP23017_PORTA, 0xFF);        // activate all of the PORTA pin pull-ups
 *	  io.setInputPolarity(MCP23017_PORTA, 0xFF); // invert all of the PORTA pins input polarity
 *
 *    while(true){
 *	  	// write PORTA to PORTB
 *  	io.write(1,io.read(0));
 *	  }
 *
 *	  return 0;
 *  }
 *  @endcode
 */

#ifdef __cplusplus
}
#endif
