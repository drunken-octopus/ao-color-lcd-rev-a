/*******************
 * ARD_LCD_Test1.c *
 *******************/

/****************************************************************************
 *   Written By Mark Pelletier 2017 - Aleph Objects, Inc.                   *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   To view a copy of the GNU General Public License, go to the following  *
 *   location: <http://www.gnu.org/licenses/>.                              *
 ****************************************************************************/

// Note - File saved as a .c file for gedit highlights.  Paste code into Arduino .ino sketch to run on Arduino Uno.


#include <SPI.h>
#include <EEPROM.h>
#include "FT800_REG.h"
#include "FT800_CMD.h"
#include "ARD_LCD_Test1.h"

// GLOBAL CONSTANTS

const int LCD_cs = 9;                                           // Arduino Uno Pin-9 (ATmega328 Port B1)
const int LCD_pwrdown = 8;                                      // Arduino Uno Pin-8 (ATmega328 Port B0)
const int Test_LED = 6;                                         // Arduino Uno Pin-6 (ATmega328 Port D6)

// GLOBAL STRUCTURES

struct Button_Struct Print_Button, Pause_Button, Continue_Button;

struct Text_Struct Text1_Struct;

// OTHER GLOBALS

SPISettings LCDsettings(1000000, MSBFIRST, SPI_MODE0);          // SPI Settings for LCD

unsigned char bIsTouched = 0;

unsigned long Tag_Value, Touch_X_Value, Touch_Y_Value;

unsigned long Touch_Transform_A, Touch_Transform_B, Touch_Transform_C, Touch_Transform_D, Touch_Transform_E, Touch_Transform_F;

// FUNCTIONS

void LCD_Init (void)
{
     unsigned char Device_ID;

     digitalWrite(LCD_pwrdown, LOW);                            // Power Down and Power Up LCD in a Reset Mode

     delay(50);

     digitalWrite(LCD_pwrdown, HIGH);

     delay(50);

     LCD_Host_Cmd(ACTIVE);                                      // Wake Up the LCD

     delay(50);

     LCD_Host_Cmd(CLKEXT);                                      // Use External Crystal Oscillator on 4D Board

     LCD_Host_Cmd(CLK48M);                                      // Use 48 MHz PLL (default)

     Device_ID = (unsigned char) LCD_Mem_Read8(REG_ID);         // Read Device ID, Should Be 0x7C

     if(Device_ID == 0x7C)
     {
          digitalWrite(Test_LED, HIGH);
     }
     else
     {
          digitalWrite(Test_LED, LOW);
     }

     delay(1000);

     LCD_Mem_Write8(REG_GPIO, 0x00);                            // Turn OFF Display Enable (GPIO Bit 7);

     LCD_Mem_Write8(REG_PCLK, 0x00);                            // Turn OFF LCD PCLK


     LCD_Mem_Write16(REG_HCYCLE, Hcycle);                       // Configure the LCD

     LCD_Mem_Write16(REG_HOFFSET, Hoffset);

     LCD_Mem_Write16(REG_HSYNC0, Hsync0);

     LCD_Mem_Write16(REG_HSYNC1, Hsync1);

     LCD_Mem_Write16(REG_VCYCLE, Vcycle);

     LCD_Mem_Write16(REG_VOFFSET, Voffset);

     LCD_Mem_Write16(REG_VSYNC0, Vsync0);

     LCD_Mem_Write16(REG_VSYNC1, Vsync1);

     LCD_Mem_Write16(REG_HSIZE, Hsize);

     LCD_Mem_Write16(REG_VSIZE, Vsize);

     LCD_Mem_Write8(REG_SWIZZLE, Swizzle);

     LCD_Mem_Write8(REG_PCLK_POL, Pclkpol);

     LCD_Mem_Write8(REG_CSPREAD, 1);


     LCD_Mem_Write8(REG_TOUCH_MODE, 0x03);                      // Configure the Touch Screen

     LCD_Mem_Write8(REG_TOUCH_ADC_MODE, 0x01);

     LCD_Mem_Write8(REG_TOUCH_OVERSAMPLE, 0x0F);

     LCD_Mem_Write16(REG_TOUCH_RZTHRESH, 5000);


     LCD_Mem_Write8(REG_VOL_SOUND, 0x00);                       // Turn Synthesizer Volume Off


     LCD_Mem_Write8(REG_GPIO_DIR, 0x80);                        // Turn ON Display Enable

     LCD_Mem_Write8(REG_GPIO, 0x80);
}


void LCD_Set_Backlight (unsigned int Freq, unsigned char Duty)
{
     LCD_Mem_Write16(REG_PWM_HZ, Freq);
     LCD_Mem_Write8(REG_PWM_DUTY, Duty);

}


void LCD_Enable (void)
{
     LCD_Mem_Write8(REG_PCLK, 0x05);
}

void LCD_Disable (void)
{
     LCD_Mem_Write8(REG_PCLK, 0x00);
}


void LCD_SPI_Select (void)                                              // Pull LCD_cs Line LOW
{
     digitalWrite(LCD_cs, LOW);
}


void LCD_SPI_Deselect (void)                                            // Pull LCD_cs Line HIGH
{
     digitalWrite(LCD_cs, HIGH);
}


// HOST COMMAND FUNCTIONS

void LCD_Host_Cmd (unsigned char Host_Command)                          // Sends 24-Bit Host Command to LCD
{
     if(Host_Command != ACTIVE)
     {
          Host_Command |= 0x40;
     }

     LCD_SPI_Select();

     SPI.transfer(Host_Command);

     SPI.transfer(0x00);

     SPI.transfer(0x00);

     LCD_SPI_Deselect();
}


// CO_PROCESSOR COMMANDS

void LCD_Command32 (unsigned long cmd32)                                // Writes a 32-bit (4 Bytes) Value to the Co-processor Command Buffer FIFO
{
     Serial.println("");
     Serial.println("START Command");

     unsigned long Command_Read_Ptr = 0;
     unsigned long Command_Write_Ptr = 0;
     unsigned long Buffer_Bytes;

     unsigned int n;

     for(n = 0; n < 100; n++)                                          // Check for Previous Command Finish
     {
          Command_Read_Ptr = LCD_Mem_Read32(REG_CMD_READ);              // Check Buffer Pointers Being Equal
          Command_Write_Ptr = LCD_Mem_Read32(REG_CMD_WRITE);

          if(Command_Read_Ptr == Command_Write_Ptr)
          {
               break;
          }
     }

     Serial.print("Cmd Write Ptr : 0x");
     Serial.println(Command_Write_Ptr, HEX);

     Serial.print("Cmd Read Ptr :  0x");
     Serial.println(Command_Read_Ptr, HEX);

     Buffer_Bytes = Command_Write_Ptr - Command_Read_Ptr;

     if((4096 - Buffer_Bytes) > 4)                                      // If Enough Space, Process Command
     {
          LCD_Mem_Write32((RAM_CMD + Command_Write_Ptr), cmd32);
          LCD_Mem_Write32(REG_CMD_WRITE, (Command_Write_Ptr + 4));
     }

     Serial.print("LCD Command32 :  0x");
     Serial.println(cmd32, HEX);
     Command_Write_Ptr = LCD_Mem_Read32(REG_CMD_WRITE);
     Serial.print("NEW Write Ptr :  0x");
     Serial.println(Command_Write_Ptr, HEX);
     Serial.println("");
}







// MEMORY READ FUNCTIONS

unsigned char LCD_Mem_Read8 (unsigned long Reg_Address)                 // Write 4-Byte Address, Read 1-Byte Data
{
     unsigned char R_Data;

     LCD_SPI_Select();

     SPI.transfer((unsigned char) ((Reg_Address >> 16) & 0x3F));        // Address [21:16]

     SPI.transfer((unsigned char) ((Reg_Address >> 8) & 0xFF));         // Address [15:8]

     SPI.transfer((unsigned char) Reg_Address);                         // Address [7:0]

     SPI.transfer(0x00);                                                // Dummy Byte
 
     R_Data = SPI.transfer(0x00);                                       // Read 1 Byte

     LCD_SPI_Deselect();

     return(R_Data);
}


unsigned long LCD_Mem_Read16 (unsigned long Reg_Address)                // Write 4-Byte Address, Read 2-Bytes Data
{
     unsigned long R_Data;

     LCD_SPI_Select();

     SPI.transfer((unsigned char) ((Reg_Address >> 16) & 0x3F));        // Address [21:16]

     SPI.transfer((unsigned char) ((Reg_Address >> 8) & 0xFF));         // Address [15:8]

     SPI.transfer((unsigned char) Reg_Address);                         // Address [7:0]

     SPI.transfer(0x00);                                                // Dummy Byte

     R_Data = (unsigned long) SPI.transfer(0x00);                       // Read Byte 1

     R_Data |= ((unsigned long)SPI.transfer(0x00) << 8);                // Read Byte 2

     LCD_SPI_Deselect();

     return(R_Data);
}


unsigned long LCD_Mem_Read32 (unsigned long Reg_Address)                // Write 4-Byte Address, Read 2-Bytes Data
{
     unsigned long R_Data;

     LCD_SPI_Select();

     SPI.transfer((unsigned char) ((Reg_Address >> 16) & 0x3F));        // Address [21:16]

     SPI.transfer((unsigned char) ((Reg_Address >> 8) & 0xFF));         // Address [15:8]

     SPI.transfer((unsigned char) Reg_Address);                         // Address [7:0]

     SPI.transfer(0x00);                                                // Dummy Byte

     R_Data = (unsigned long) SPI.transfer(0x00);                       // Read Byte 1

     R_Data |= ((unsigned long)SPI.transfer(0x00) << 8);                // Read Byte 2

     R_Data |= ((unsigned long)SPI.transfer(0x00) << 16);               // Read Byte 3

     R_Data |= ((unsigned long)SPI.transfer(0x00) << 24);               // Read Byte 4

     LCD_SPI_Deselect();

     return(R_Data);
}


// MEMORY WRITE FUNCTIONS

void LCD_Mem_Write8 (unsigned long Reg_Address, unsigned char W_Data)   // Write 3-Byte Address, Write 1-Byte Data
{
     LCD_SPI_Select();

     SPI.transfer((unsigned char) ((Reg_Address >> 16) | 0x80));       // Address [21:16]

     SPI.transfer((unsigned char) ((Reg_Address >> 8) & 0xFF));        // Address [15:8]

     SPI.transfer((unsigned char) (Reg_Address & 0xFF));               // Address [7:0]

     SPI.transfer(W_Data);                                             // Write 1 Byte

     LCD_SPI_Deselect();
}


void LCD_Mem_Write16 (unsigned long Reg_Address, int W_Data)          // Write 3-Byte Address, Write 2-Bytes Data
{
     LCD_SPI_Select();

     SPI.transfer((unsigned char) ((Reg_Address >> 16) | 0x80));      // Address [21:16]

     SPI.transfer((unsigned char) ((Reg_Address >> 8) & 0xFF));       // Address [15:8]

     SPI.transfer((unsigned char) (Reg_Address & 0xFF));              // Address [7:0]

     SPI.transfer((unsigned char) (W_Data & 0x00FF));                 // Write Byte 0

     SPI.transfer((unsigned char) ((W_Data >> 8) & 0x00FF));          // Write Byte 1

     LCD_SPI_Deselect();
}


void LCD_Mem_Write32 (unsigned long Reg_Address, unsigned long W_Data)   // Write 3-Byte Address, Write 4-Bytes Data
{
     LCD_SPI_Select();

     SPI.transfer((unsigned char) ((Reg_Address >> 16) | 0x80));         // Address [21:16]

     SPI.transfer((unsigned char) ((Reg_Address >> 8) & 0xFF));          // Address [15:8]

     SPI.transfer((unsigned char) (Reg_Address & 0xFF));                 // Address [7:0]

     SPI.transfer((unsigned char) (W_Data & 0x000000FF));                // Write Byte 0

     SPI.transfer((unsigned char) ((W_Data >> 8) & 0x000000FF));         // Write Byte 1

     SPI.transfer((unsigned char) ((W_Data >> 16) & 0x000000FF));        // Write Byte 2

     SPI.transfer((unsigned char) ((W_Data >> 24) & 0x000000FF));        // Write Byte 3

     LCD_SPI_Deselect();
}


// HELPER FUNCTIONS

unsigned long Clear_Color_RGB (unsigned char red, unsigned char grn, unsigned char blu)
{
     unsigned long Out_Value32, Color_Buffer;

     Out_Value32 = CLEAR_COLOR_RGB;
     
     Color_Buffer = 0x00000000;
     
     Color_Buffer |= red;
     
     Color_Buffer <<= 8;
     
     Color_Buffer |= grn;
     
     Color_Buffer <<= 8;
     
     Color_Buffer |= blu;
     
     Out_Value32 |= Color_Buffer;

     Serial.print("CLEAR_COLOR_RGB = 0x");
     Serial.println(Out_Value32, HEX);

     return(Out_Value32);
}


unsigned long Clear (unsigned char Color_Buffer, unsigned char Stencil_Buffer, unsigned char Tag_Buffer)
{
     unsigned long Out_Value32;

     Out_Value32 = CLEAR | (unsigned long) ((Color_Buffer << 2) & CLEAR_COLOR_BUFFER)
                         | (unsigned long) ((Stencil_Buffer << 1) & CLEAR_STENCIL_BUFFER)
                         | (unsigned long) (Tag_Buffer & CLEAR_TAG_BUFFER);

     Serial.print("CLEAR = 0x");
     Serial.println(Out_Value32, HEX);

     return(Out_Value32);
}


void Make_Button_Structures (void)
{
     Print_Button.x_pos = 40;
     Print_Button.y_pos = 20;
     Print_Button.x_size = 160;
     Print_Button.y_size = 60;
     Print_Button.font = 29;
     Print_Button.options = 0;
     strcpy(Print_Button.text, "PRINT");

     Pause_Button.x_pos = 40;
     Pause_Button.y_pos = 100;
     Pause_Button.x_size = 160;
     Pause_Button.y_size = 60;
     Pause_Button.font = 29;
     Pause_Button.options = 0;
     strcpy(Pause_Button.text, "PAUSE");

     Continue_Button.x_pos = 40;
     Continue_Button.y_pos = 180;
     Continue_Button.x_size = 160;
     Continue_Button.y_size = 60;
     Continue_Button.font = 29;
     Continue_Button.options = 0;
     strcpy(Continue_Button.text, "CONTINUE");
}


void Make_Text_Structures (void)
{
     Text1_Struct.x_pos = 50;
     Text1_Struct.y_pos = 50;
     Text1_Struct.font = 18;
     Text1_Struct.options = 0;
     strcpy(Text1_Struct.text, "TEST1");
}




// CO-PROCESSOR COMMAND FUNCTIONS - These functions write to the Command FIFO

void LCD_Cmd_Buf_Reset (void)                                                            // Rest the REG_CMD_WRITE and REG_CMD_READ Pointer Registers
{
          LCD_Mem_Write32(REG_CMD_WRITE, 0x00000000);
          LCD_Mem_Write32(REG_CMD_READ, 0x00000000);
}


void LCD_Cmd_DL_Start (void)                                                              // Add DL_START to the Display List (DL)
{
     LCD_Command32(CMD_DLSTART);
}


void LCD_Cmd_Clear_Color_RGB (unsigned char Red, unsigned char Grn, unsigned char Blu)    // Set Clear Screen Color to the DL
{
     unsigned long Value32;

     Value32 = (unsigned long) Red;
     Value32 <<= 8;
     Value32 |= (unsigned long) Grn;
     Value32 <<= 8;
     Value32 |= ((unsigned long) Blu | CLEAR_COLOR_RGB);

     LCD_Command32(Value32);
}


void LCD_Cmd_Clear (unsigned char Clr, unsigned char Stl, unsigned char Tag)
{
     unsigned long Value32;

     Value32 = (unsigned long) (Clr & 0x01);
     Value32 <<= 1;
     Value32 |= (unsigned long) (Stl & 0x01);
     Value32 <<= 1;
     Value32 |= (((unsigned long) (Tag & 0x01)) | CLEAR);

     LCD_Command32(Value32);
}


void LCD_Cmd_Display (void)
{
     LCD_Command32(DISPLAY);
}

void LCD_Cmd_End (void)
{
     LCD_Command32(END);
}


void LCD_Cmd_Swap (void)
{
     LCD_Command32(CMD_SWAP);
}


void LCD_Cmd_Set_Foreground_Color (unsigned char Red, unsigned char Grn, unsigned char Blu)
{
     unsigned long Value32;

     Value32 = (unsigned long) Red;
     Value32 <<= 8;
     Value32 |= (unsigned long) Grn;
     Value32 <<= 8;
     Value32 |= (unsigned long) Blu;

     LCD_Command32(CMD_FGCOLOR);

     LCD_Command32(Value32);
}


void LCD_Cmd_Set_Background_Color (unsigned char Red, unsigned char Grn, unsigned char Blu)
{
     unsigned long Value32;

     Value32 = (unsigned long) Red;
     Value32 <<= 8;
     Value32 |= (unsigned long) Grn;
     Value32 <<= 8;
     Value32 |= ((unsigned long) Blu | CMD_BGCOLOR);

     LCD_Command32(Value32);
}

void LCD_Cmd_Set_Draw_Color (unsigned long Color_Value_RGB)
{
     unsigned long DL_Cmd;

     DL_Cmd = 0x04000000 | Color_Value_RGB;

     Serial.print("CMD - Set Draw Color : 0x");
     Serial.print(DL_Cmd, HEX);
     Serial.print(",  0x");
     Serial.println(Color_Value_RGB, HEX);

     LCD_Command32(DL_Cmd);
}


void Test_Button_Cmd(struct Button_Struct *Btn)
{
     unsigned long Value32;

     unsigned int Str_Length, Str_Remain;

     unsigned char i;

     char C_Buf[4];

     char* Str_Ptr;

 // Test Code to Draw Button

     Serial.println("BUTTON Cmd");
     LCD_Command32(CMD_BUTTON);

     Serial.println("BUTTON XY Pos");
     Value32 = (unsigned long) Btn->y_pos;
     Value32 <<= 16;
     Value32 |= (unsigned long) Btn->x_pos;
     LCD_Command32(Value32);

     Serial.println("BUTTON XY Size");
     Value32 = (unsigned long) Btn->y_size;
     Value32 <<= 16;
     Value32 |= (unsigned long) Btn->x_size;
     LCD_Command32(Value32);

     Serial.println("BUTTON Font and Options");
     Value32 = (unsigned long) Btn->options;
     Value32 <<= 16;
     Value32 |= (unsigned long) Btn->font;
     LCD_Command32(Value32);

     // 4 Characted Text String as a 32-Bit Value
    
     Str_Length = strlen(Btn->text);

     Str_Remain = Str_Length % 4;

     // Make Value32s for Full 4-Character Strings

     for(i = 0; i < (Str_Length / 4); i++)
     {
          Str_Ptr = Btn->text + (i * 4);

          C_Buf[3] = *Str_Ptr;
          C_Buf[2] = *(Str_Ptr + 1);
          C_Buf[1] = *(Str_Ptr + 2);
          C_Buf[0] = *(Str_Ptr + 3);

          Value32 = (unsigned long) C_Buf[0];
          Value32 <<= 8;
          Value32 |= (unsigned long) C_Buf[1];
          Value32 <<= 8;
          Value32 |= (unsigned long) C_Buf[2];
          Value32 <<= 8;
          Value32 |= (unsigned long) C_Buf[3];

          LCD_Command32(Value32);
     }

     // Make Value32 for Any Remaining Characters

     C_Buf[3] = 0;
     C_Buf[2] = 0;
     C_Buf[1] = 0;
     C_Buf[0] = 0;

     Str_Ptr = Str_Length - Str_Remain;

     for(i = 0; i < Str_Remain; i++)
     {
          C_Buf[i] = *(Str_Ptr + i);
     }

     Value32 = (unsigned long) C_Buf[3];
     Value32 <<= 8;
     Value32 |= (unsigned long) C_Buf[2];
     Value32 <<= 8;
     Value32 |= (unsigned long) C_Buf[1];
     Value32 <<= 8;
     Value32 |= (unsigned long) C_Buf[0];

     LCD_Command32(Value32);

    
}



void LCD_Cmd_Draw_Text(struct Text_Struct *txt)
{
     unsigned long Value32;

     LCD_Command32(CMD_TEXT);

     Value32 = (unsigned long) txt->x_pos << 16;
     Value32 |= (unsigned long) txt->y_pos;

     LCD_Command32(Value32);

     Value32 = (unsigned long) txt->font << 16;
     Value32 |= (unsigned long) txt->options;

     LCD_Command32(Value32);

     Value32 = 0x45464700;

     LCD_Command32(Value32);

}


void LCD_Cmd_Draw_Button(struct Button_Struct *Btn)
{
     unsigned long Value32;

     unsigned int Str_Length, Str_Remain;

     unsigned char i;

     char C_Buf[4];

     char* Str_Ptr;

     // Send BUTTON Command, Position, Size, Font and Options

     LCD_Command32(CMD_BUTTON);

     Value32 = (unsigned long) Btn->y_pos;
     Value32 <<= 16;
     Value32 |= (unsigned long) Btn->x_pos;
     LCD_Command32(Value32);

     Value32 = (unsigned long) Btn->y_size;
     Value32 <<= 16;
     Value32 |= (unsigned long) Btn->x_size;
     LCD_Command32(Value32);

     Value32 = (unsigned long) Btn->options;
     Value32 <<= 16;
     Value32 |= (unsigned long) Btn->font;
     LCD_Command32(Value32);

     // 4 Characted Text String as a 32-Bit Value
    
     Str_Length = strlen(Btn->text);

     Str_Remain = Str_Length % 4;

     // Make Value32s for Full 4-Character Strings

     for(i = 0; i < (Str_Length / 4); i++)
     {
          Str_Ptr = Btn->text + (i * 4);

          C_Buf[3] = *Str_Ptr;
          C_Buf[2] = *(Str_Ptr + 1);
          C_Buf[1] = *(Str_Ptr + 2);
          C_Buf[0] = *(Str_Ptr + 3);

          Value32 = (unsigned long) C_Buf[0];
          Value32 <<= 8;
          Value32 |= (unsigned long) C_Buf[1];
          Value32 <<= 8;
          Value32 |= (unsigned long) C_Buf[2];
          Value32 <<= 8;
          Value32 |= (unsigned long) C_Buf[3];

          LCD_Command32(Value32);
     }

     // Make Value32 for Any Remaining Characters

     C_Buf[3] = 0;
     C_Buf[2] = 0;
     C_Buf[1] = 0;
     C_Buf[0] = 0;

     Str_Ptr = Btn->text + Str_Length - Str_Remain;

     for(i = 0; i < Str_Remain; i++)
     {
          C_Buf[i] = *(Str_Ptr + i);
     }

     Value32 = (unsigned long) C_Buf[3];
     Value32 <<= 8;
     Value32 |= (unsigned long) C_Buf[2];
     Value32 <<= 8;
     Value32 |= (unsigned long) C_Buf[1];
     Value32 <<= 8;
     Value32 |= (unsigned long) C_Buf[0];

     LCD_Command32(Value32);
}


void LCD_Cmd_Set_Tag (unsigned char Tag)
{
     unsigned long Value32;

     Value32 = TAG | (unsigned long) Tag;

     LCD_Command32(Value32);
}


// DISPLAY LIST FUNCTIONS - These function write to an address in the Display List memory space.

void LCD_DL_Begin (unsigned long Addr, unsigned long Primative)
{
     unsigned long Value32;

     Value32 = BEGIN + Primative;

     Serial.print("DL_Begin Value = 0x");
     Serial.println(Value32, HEX);

     LCD_Mem_Write32(Addr, Value32);
}


void LCD_DL_End (unsigned long Addr)
{
     LCD_Mem_Write32(Addr, END);
}


void LCD_DL_Display (unsigned long Addr)
{
     LCD_Mem_Write32(Addr, DISPLAY);
}


void LCD_DL_Vertex_2II (unsigned long Addr, unsigned int X_Coord, unsigned int Y_Coord, unsigned int B_Handle, unsigned int Cell)
{
     unsigned long Value32;

     Value32 |= (unsigned long) X_Coord;

     Value32 <<= 9;

     Value32 |= (unsigned long) Y_Coord;

     Value32 <<= 5;

     Value32 |= (unsigned long) B_Handle;

     Value32 <<= 7;

     Value32 |= Cell;

     Value32 |= 0x80000000;

     Serial.print("DL_Vertex_2II Value = 0x");
     Serial.println(Value32, HEX);

     LCD_Mem_Write32(Addr, Value32);
}


void LCD_DL_Point_Size (unsigned long Addr, unsigned long Size)
{
     unsigned long Value32;

     Value32 = POINT_SIZE | Size;

     LCD_Mem_Write32(Addr, Value32);
}


void LCD_DL_Line_Width (unsigned long Addr, unsigned int Width)
{
     unsigned long Value32;

     Value32 = (unsigned long) Width;

     Value32 |= LINE_WIDTH;

     LCD_Mem_Write32(Addr, Value32);

}


void LCD_DL_Color_RGB (unsigned long Addr, unsigned char Red, unsigned char Grn, unsigned char Blu)
{
     unsigned long Value32;

     Value32 = (unsigned long) Red;

     Value32 <<= 8;

     Value32 |= (unsigned long) Grn;

     Value32 <<= 8;

     Value32 |= (unsigned long) Blu;

     Value32 |= 0x04000000;

     LCD_Mem_Write32(Addr, Value32);
}

void LCD_DL_Tag (unsigned long Addr, unsigned char Tag)
{
     unsigned long Value32;

     Value32 = TAG | (unsigned long) Tag;

     LCD_Mem_Write32(Addr, Value32);
}


void Command_List (void)
{
     LCD_Cmd_Buf_Reset();

     LCD_Command32(CMD_DLSTART);

     LCD_Command32(Clear_Color_RGB(0x00, 0x00, 0x00));

     LCD_Command32(Clear(1,1,1));

 //    LCD_Command32(CMD_CALIBRATE);

     LCD_Cmd_Draw_Button(&Print_Button);

     delay(2000);

     LCD_Command32(CMD_DLSTART);

 //    LCD_Command32(CMD_LOGO);

 //    LCD_Command32(CMD_DLSTART);

 //    LCD_Command32(CMD_CALIBRATE);

 //    LCD_Cmd_Draw_Button(&Print_Button);

/*

     LCD_Cmd_Set_Foreground_Color(0x08FF80);

     LCD_Cmd_Draw_Button(&Print_Button);

     LCD_Command32(0x04FFFFFF);

     LCD_Cmd_Draw_Text(&Text1_Struct);

     LCD_Command32(DISPLAY);

     LCD_Command32(CMD_SWAP);

     delay(5000);

     LCD_Cmd_Set_Background_Color(0x0606A0);

    

     LCD_Cmd_Set_Draw_Color(0xFFFF00);

     LCD_Cmd_Draw_Button(&Print_Button);

     LCD_Cmd_Set_Tag(0x01);

     LCD_Cmd_Draw_Text(&Text1_Struct);

     delay(2000);

      LCD_Command32(CMD_DLSTART);

     LCD_Command32(CMD_LOGO);
*/



}


void LCD_Write_Display_List (void)
{
     LCD_Mem_Write32((RAM_DL + 0), Clear_Color_RGB(0x00, 0x00, 0x80));

     LCD_Mem_Write32((RAM_DL + 4), Clear(1,0,0));

     LCD_DL_Color_RGB((RAM_DL + 8), 0xFF, 0xFF, 0x00);

     LCD_DL_Begin((RAM_DL + 12), BEGIN_POINTS);

     LCD_DL_Point_Size((RAM_DL + 16), 160);

     LCD_DL_Vertex_2II((RAM_DL + 20), 100, 100, 0, 0);

     LCD_DL_Color_RGB((RAM_DL + 24), 0x00, 0xFF, 0x00);
     
     LCD_DL_Point_Size((RAM_DL + 28), 640);

     LCD_DL_Vertex_2II((RAM_DL + 32), 300, 100, 0, 0);

     LCD_DL_Color_RGB((RAM_DL + 36), 0xFF, 0x00, 0xE0);

     LCD_DL_Begin((RAM_DL + 40), BEGIN_STRIP_RECTS);

     LCD_DL_Point_Size((RAM_DL + 44), 16);

     LCD_DL_Vertex_2II((RAM_DL + 48), 100, 180, 0, 0);

     LCD_DL_Vertex_2II((RAM_DL + 52), 300, 240, 0, 0);

     LCD_DL_Color_RGB((RAM_DL + 56), 0xFF, 0xFF, 0xFF);

     LCD_DL_Begin((RAM_DL + 60), BEGIN_BITMAPS);

     LCD_DL_Vertex_2II((RAM_DL + 64), 20, 20, 30, 'A');

     LCD_DL_Vertex_2II((RAM_DL + 68), 40, 20, 30, 'L');

     LCD_DL_Vertex_2II((RAM_DL + 72), 60, 20, 30, 'E');

     LCD_DL_Vertex_2II((RAM_DL + 76), 80, 20, 30, 'P');

     LCD_DL_Vertex_2II((RAM_DL + 80), 100, 20, 30, 'H');

     LCD_DL_Vertex_2II((RAM_DL + 84), 130, 20, 30, 'O');

     LCD_DL_Vertex_2II((RAM_DL + 88), 150, 20, 30, 'B');

     LCD_DL_Vertex_2II((RAM_DL + 92), 170, 20, 30, 'J');

     LCD_DL_Vertex_2II((RAM_DL + 96), 190, 20, 30, 'E');

     LCD_DL_Vertex_2II((RAM_DL + 100), 210, 20, 30, 'C');

     LCD_DL_Vertex_2II((RAM_DL + 104), 230, 20, 30, 'T');

     LCD_DL_Vertex_2II((RAM_DL + 108), 250, 20, 30, 'S');

     LCD_DL_Begin((RAM_DL + 112), BEGIN_LINES);

     LCD_DL_Line_Width((RAM_DL + 116), 64);

     LCD_DL_Vertex_2II((RAM_DL + 120), 100, 160, 0, 0);

     LCD_DL_Vertex_2II((RAM_DL + 124), 300, 160, 0, 0);

     LCD_DL_Color_RGB((RAM_DL + 128), 0xFF, 0x00, 0x00);

     LCD_DL_Vertex_2II((RAM_DL + 132), 360, 200, 0, 0);

     LCD_DL_Vertex_2II((RAM_DL + 136), 420, 260, 0, 0);

     LCD_DL_Begin((RAM_DL + 140), BEGIN_LINE_STRIP);

     LCD_DL_Line_Width((RAM_DL + 144), 16);

     LCD_DL_Color_RGB((RAM_DL + 148),0x00, 0x80, 0xFF);

     LCD_DL_Vertex_2II((RAM_DL + 152), 360, 20, 0, 0);

     LCD_DL_Vertex_2II((RAM_DL + 156), 420, 80, 0, 0);

     LCD_DL_Vertex_2II((RAM_DL + 160), 460, 200, 0, 0);

     LCD_DL_Tag((RAM_DL + 164), 1);

     LCD_DL_End((RAM_DL + 168));

     LCD_DL_Display((RAM_DL + 172));


}



void setup()
{
     Serial.begin(9600);

     Serial.println("Start of Program");

     Serial.println("");
     
     pinMode(LCD_cs, OUTPUT);                                  // Set Pin-9 as an Output for LCD Chip Select

     pinMode(LCD_pwrdown, OUTPUT);                             // Set Pin-8 as an Output for LCD Power Down

     pinMode(Test_LED, OUTPUT);                                // Set Pin-6 as an Output to drive Test LED

     SPI.begin();                                              // Initialize the Uno SPI

     SPI.beginTransaction(LCDsettings);                        // Set SPI Mode, Bit Order and Clock Speed

     LCD_Init();                                               // Initialize the LCD

     LCD_Enable();                                             // Start the PCLK

     LCD_Mem_Write32(REG_TOUCH_TRANSFORM_A, 0x000080C2);

     LCD_Mem_Write32(REG_TOUCH_TRANSFORM_B, 0x0000003E);

     LCD_Mem_Write32(REG_TOUCH_TRANSFORM_C, 0xFFEE4CCF);

     LCD_Mem_Write32(REG_TOUCH_TRANSFORM_D, 0x00000039);

     LCD_Mem_Write32(REG_TOUCH_TRANSFORM_E, 0xFFFFB170);

     LCD_Mem_Write32(REG_TOUCH_TRANSFORM_F, 0x011F21A6);

     // Set Button Structure Values

     Make_Button_Structures();

     // Reset the Command FIFO Buffer

     LCD_Cmd_Buf_Reset();

     // Initial Display Commands

     Serial.println("LCD_Cmd_DL_Start");
     LCD_Cmd_DL_Start();

     Serial.println("LCD_Cmd_Clear_Color_RGB");
     LCD_Cmd_Clear_Color_RGB(0x80,0x80,0x80);

     Serial.println("LCD_Cmd_Clear");
     LCD_Cmd_Clear(1,1,1);

     // Button Commands

     LCD_Cmd_Set_Foreground_Color(0x20, 0xD0, 0x20);

     LCD_Cmd_Draw_Button(&Print_Button);

     LCD_Cmd_Set_Foreground_Color(0xD0, 0x20, 0x20);

     LCD_Cmd_Draw_Button(&Pause_Button);

     LCD_Cmd_Set_Foreground_Color(0x20, 0x20, 0xD0);

     LCD_Cmd_Draw_Button(&Continue_Button);

     Serial.println("LCD_Cmd_End");
     LCD_Cmd_End();

     Serial.println("LCD_Cmd_Display");
     LCD_Cmd_Display();

     Serial.println("LCD_Cmd_Swap");
     LCD_Cmd_Swap();


     
     Serial.println("End of Program");
     
}


void loop()
{

     delay(1000);

     Tag_Value = LCD_Mem_Read32(REG_TOUCH_TAG);

     Touch_X_Value = LCD_Mem_Read32(REG_TOUCH_SCREEN_XY) >> 16;

     Touch_Y_Value = LCD_Mem_Read32(REG_TOUCH_SCREEN_XY) & 0x0000FFFF;

     Touch_Transform_A = LCD_Mem_Read32(REG_TOUCH_TRANSFORM_A);

     Touch_Transform_B = LCD_Mem_Read32(REG_TOUCH_TRANSFORM_B);

     Touch_Transform_C = LCD_Mem_Read32(REG_TOUCH_TRANSFORM_C);

     Touch_Transform_D = LCD_Mem_Read32(REG_TOUCH_TRANSFORM_D);

     Touch_Transform_E = LCD_Mem_Read32(REG_TOUCH_TRANSFORM_E);

     Touch_Transform_F = LCD_Mem_Read32(REG_TOUCH_TRANSFORM_F);

     Serial.println("");
     Serial.print("Tag Value = 0x");
     Serial.println(Tag_Value, HEX);
     Serial.print("X Value = ");
     Serial.println(Touch_X_Value, DEC);
     Serial.print("Y Value = ");
     Serial.println(Touch_Y_Value, DEC);


/*     
     Serial.print("Transform A = 0x");
     Serial.println(Touch_Transform_A, HEX);
     Serial.print("Transform B = 0x");
     Serial.println(Touch_Transform_B, HEX);
     Serial.print("Transform C = 0x");
     Serial.println(Touch_Transform_C, HEX);
     Serial.print("Transform D = 0x");
     Serial.println(Touch_Transform_D, HEX);
     Serial.print("Transform E = 0x");
     Serial.println(Touch_Transform_E, HEX);
     Serial.print("Transform F = 0x");
     Serial.println(Touch_Transform_F, HEX);



     if(Tag_Value )
     {
          if(!bIsTouched)
          {
               bIsTouched = 1;

               Touch_X_Value = LCD_Mem_Read32(REG_TOUCH_SCREEN_XY) >> 16;

               Touch_Y_Value = LCD_Mem_Read32(REG_TOUCH_SCREEN_XY) & 0x0000FFFF;

               Serial,println("");
               Serial.print("Touch Sceen Pressed, Tag Value = 0x");
               Serial.println(Tag_Value, HEX);
               Serial.print("X Value = 0x");
               Serial.println(Touch_X_Value, HEX);
               Serial.print("Y Value = 0x");
               Serial.println(Touch_Y_Value, HEX);
          }
     }
     else
     {
          bIsTouched = 0;
     }
*/
}




























