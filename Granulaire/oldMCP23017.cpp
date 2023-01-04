/* MCP23017 Library for mbed
 * Copyright (c) 2014, Takuya Urakawa
 * 
 * This library is released under the MIT License
 * See http://opensource.org/licenses/mit-license.php
 */
#ifdef __cplusplus
    extern "C" {
    #endif

#define __MCP2307__
#include "MCP23017.h"

#include "stm32f4xx_hal_i2c.h"

extern I2C_HandleTypeDef hi2c1;

//***************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void MCP23017_i2cSend3(stMCP23017* mcp,char _command, char _data1, char _data2)
{
uint8_t commands[3];

	commands[0] = _command;
	commands[1] = _data1;
	commands[2] = _data2;
	SendToMCP = true;
	HAL_I2C_Master_Transmit(&hi2c1,mcp->mWriteAddr,&commands[0],3,1000);
	SendToMCP = false;
}
//***************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void MCP23017_i2cSend2(stMCP23017* mcp,char _command, char _data1)
{
uint8_t commands[2];

	commands[0] = _command;
	commands[1] = _data1;
	SendToMCP = true;
	HAL_I2C_Master_Transmit(&hi2c1,mcp->mWriteAddr,&commands[0],2,1000);
	SendToMCP = false;
}
//***************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
char MCP23017_i2cRead(stMCP23017* mcp,char _command)
{
uint8_t commands[2];

	commands[0] = _command;
	SendToMCP = true;
	HAL_I2C_Master_Transmit(&hi2c1,mcp->mWriteAddr,&commands[0],2,1000);
	HAL_I2C_Master_Receive(&hi2c1,mcp->mReadAddr,&commands[0],2,1000);
	SendToMCP = false;
	return commands[0];
}
//***************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
char MCP23017_read(stMCP23017* mcp,char _port)
{
	//return MCP23017_i2cRead(mcp,REG_GPIO + _port);
}
//***************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void MCP23017_write(stMCP23017* mcp,char _port, char _value)
{
	//MCP23017_i2cSend2(mcp,REG_GPIO + _port, _value);
}

//***************************************************/
/* IT on pin PC_7 and PB_2                          */
/* PC_7 don't work fine ?                           */
/*                                                  */
/*                                                  */
//***************************************************/
void MCP23017_ITA_Fall()
{
}
//***************************************************/
/*  IT on pin PC_6                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void MCP23017_ITB_Fall()
{
}


//***************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void MCP23017_ITinit()
{
}
//***************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void MCP23017_init(stMCP23017* mcp,char add)
{
	mcp->_addr = add;
	mcp->mWriteAddr = mcp->_addr & 0xFE;
	mcp->mReadAddr = mcp->_addr | 0x01;

	// All pins input mode A&B
	 MCP23017_i2cSend2(mcp,REG_IODIRA,0x12);
	 MCP23017_i2cSend2(mcp,REG_IODIRB,0x34);

	// All pins pull up
	 MCP23017_i2cSend2(mcp,REG_PULLUPA,0xFF);
	 MCP23017_i2cSend2(mcp,REG_PULLUPB,0xFF);

	 // Interrupt on change for all pins
	MCP23017_i2cSend2(mcp,REG_INTA,0xFF);
	MCP23017_i2cSend2(mcp,REG_INTB,0xFF);

}
//***************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void MCP23017_setInterruptConfig(stMCP23017* mcp)
{
//	MCP23017_i2cSend2(mcp,REG_GPINTEN + MCP23017_PORTB,0xFF);		// Enable all Bit IT for PORTB
//	MCP23017_i2cSend2(mcp,REG_DEFVAL + MCP23017_PORTB,0x00);		// Set the ref value to 0x00
//	MCP23017_i2cSend2(mcp,REG_INTCON + MCP23017_PORTB,0xFF);		// set IT On compare defval
}
#ifdef __cplusplus
}
#endif
