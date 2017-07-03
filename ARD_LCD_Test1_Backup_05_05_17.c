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
#include <avr/pgmspace.h>
#include "FT800_REG.h"
#include "FT800_CMD.h"
#include "ARD_LCD_Test1.h"
#include "AB10ary.h"
#include "AB12ary.h"
#include "Lulzbot_Logo200_GL.h"
#include "TD_Icon.h"

#define FILE_LIST_X_DIM                 400
#define FILE_LIST_Y_DIM                 400
#define FILE_LIST_RAM_G_ADDR            RAM_G + 16384

#define LOGO_RAM_G_ADDR                 RAM_G + 196608

/************************************************
 *                  RAM_G Use                   *
 *                                              *
 * RAM_G          8x8   RGB565 Test Bitmap 1    *
 * RAM_G + 128    8x8   RGB565 Test Bitmap 2    *
 * RAM_G + 256    8x8   RGB565 Test Bitmap 3    *
 * RAM_G + 384    8x8   RGB565 Test Bitmap 4    *
 * RAM_G + 512    8x8   RGB332 Test Bitmap 1    *
 * RAM_G + 576    8x8   RGB332 Test Bitmap 2    *
 * RAM_G + 640    16x16 RGB332 Test Bitmap 1    *
 * RAM_G + 900    Widget Font Metric Block      *
 * RAM_G + 1100   Widget Font Data              *
 *                                              *
 ************************************************/

// GLOBAL CONSTANTS

const int LCD_cs = 9;                                           // Arduino Uno Pin-9 (ATmega328 Port B1)
const int LCD_pwrdown = 8;                                      // Arduino Uno Pin-8 (ATmega328 Port B0)
const int Test_LED = 6;                                         // Arduino Uno Pin-6 (ATmega328 Port D6)

// GLOBAL STRUCTURES

struct Button_Struct Menu_Button, Stop_Button, USB_Button, Combo_Button;

struct Button_Struct Extruder_Temp, Bed_Heat, Fan_Percent, Elapsed_Time, Print_Percent;

struct Button_Struct X_Position, Y_Position, Z_Position;

struct Text_Struct Text1, Text2;

struct Clock_Struct Clock1;

struct Progress_Struct PrgBar1;

struct Slider_Struct Sldr1, File_Select_Sldr;

// OTHER GLOBALS

SPISettings LCDsettings(4000000, MSBFIRST, SPI_MODE0);          // SPI Settings for LCD

unsigned char bIsGrabbed = 0;

int Tag_Value, Touch_X_Value, Touch_Y_Value, Relative_Touch_Y_Value, Y_Grab_Value, Delta_Y_Value;

unsigned long Touch_Transform_A, Touch_Transform_B, Touch_Transform_C, Touch_Transform_D, Touch_Transform_E, Touch_Transform_F;

unsigned long Cmd_DL_Address;

unsigned int Int_Scroll_Position;

float Scroll_Position;

unsigned char Combo_Button_State = 0;
unsigned char Stop_Button_State = 0;
unsigned char Menu_Button_State = 0;

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


     LCD_Set_Backlight (0x00FA, 128);

     delay(1000);

     LCD_Set_Backlight (0x00FA, 0);
     

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


     LCD_Mem_Write8(REG_DLSWAP, 0x02);                          // Swap on New Frame


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

     Buffer_Bytes = Command_Write_Ptr - Command_Read_Ptr;

     if((4096 - Buffer_Bytes) > 4)                                      // If Enough Space, Process Command
     {
          LCD_Mem_Write32((RAM_CMD + Command_Write_Ptr), cmd32);
          LCD_Mem_Write32(REG_CMD_WRITE, (Command_Write_Ptr + 4));
     }
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


void LCD_Mem_Write16 (unsigned long Reg_Address, unsigned int W_Data) // Write 3-Byte Address, Write 2-Bytes Data
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

     return(Out_Value32);
}


unsigned long Clear (unsigned char Color_Buffer, unsigned char Stencil_Buffer, unsigned char Tag_Buffer)
{
     unsigned long Out_Value32;

     Out_Value32 = CLEAR | (unsigned long) ((Color_Buffer << 2) & CLEAR_COLOR_BUFFER)
                         | (unsigned long) ((Stencil_Buffer << 1) & CLEAR_STENCIL_BUFFER)
                         | (unsigned long) (Tag_Buffer & CLEAR_TAG_BUFFER);

     return(Out_Value32);
}


void Make_Button_Structures (void)
{
     Combo_Button.x_pos = 10;
     Combo_Button.y_pos = 230;
     Combo_Button.x_size = 130;
     Combo_Button.y_size = 30;
     Combo_Button.color_r = 0x20;
     Combo_Button.color_g = 0xD0;
     Combo_Button.color_b = 0x20;
     Combo_Button.font = 28;
     Combo_Button.options = 0;
     strcpy(Combo_Button.text, "PRINT");

     Menu_Button.x_pos = 385;
     Menu_Button.y_pos = 230;
     Menu_Button.x_size = 85;
     Menu_Button.y_size = 30;
     Menu_Button.color_r = 0xF0;
     Menu_Button.color_g = 0x00;
     Menu_Button.color_b = 0xA0;
     Menu_Button.font = 28;
     Menu_Button.options = 0;
     strcpy(Menu_Button.text, "MENU");

     Stop_Button.x_pos = 150;
     Stop_Button.y_pos = 230;
     Stop_Button.x_size = 130;
     Stop_Button.y_size = 30;
     Stop_Button.color_r = 0xF0;
     Stop_Button.color_g = 0x20;
     Stop_Button.color_b = 0x20;
     Stop_Button.font = 28;
     Stop_Button.options = 0;
     strcpy(Stop_Button.text, "STOP");

     USB_Button.x_pos = 290;
     USB_Button.y_pos = 230;
     USB_Button.x_size = 85;
     USB_Button.y_size = 30;
     USB_Button.color_r = 0x20;
     USB_Button.color_g = 0x80;
     USB_Button.color_b = 0xD0;
     USB_Button.font = 28;
     USB_Button.options = 0;
     strcpy(USB_Button.text, "");

     Extruder_Temp.x_pos = 10;
     Extruder_Temp.y_pos = 10;
     Extruder_Temp.x_size = 110;
     Extruder_Temp.y_size = 60;
     Extruder_Temp.color_r = 0xB0;
     Extruder_Temp.color_g = 0x90;
     Extruder_Temp.color_b = 0x00;
     Extruder_Temp.font = 28;
     Extruder_Temp.options = OPT_FLAT;
     strcpy(Extruder_Temp.text, "       205 C");

     Bed_Heat.x_pos = 130;
     Bed_Heat.y_pos = 10;
     Bed_Heat.x_size = 110;
     Bed_Heat.y_size = 60;
     Bed_Heat.color_r = 0xD0;
     Bed_Heat.color_g = 0x40;
     Bed_Heat.color_b = 0x00;
     Bed_Heat.font = 28;
     Bed_Heat.options = OPT_FLAT;
     strcpy(Bed_Heat.text, "       174 C");

     Fan_Percent.x_pos = 250;
     Fan_Percent.y_pos = 10;
     Fan_Percent.x_size = 110;
     Fan_Percent.y_size = 60;
     Fan_Percent.color_r = 0x60;
     Fan_Percent.color_g = 0x60;
     Fan_Percent.color_b = 0xD0;
     Fan_Percent.font = 28;
     Fan_Percent.options = OPT_FLAT;
     strcpy(Fan_Percent.text, "          86 %");

     Print_Percent.x_pos = 370;
     Print_Percent.y_pos = 10;
     Print_Percent.x_size = 100;
     Print_Percent.y_size = 25;
     Print_Percent.color_r = 0xE0;
     Print_Percent.color_g = 0x80;
     Print_Percent.color_b = 0x00;
     Print_Percent.font = 28;
     Print_Percent.options = OPT_FLAT;
     strcpy(Print_Percent.text, "  97.5 %");

     Elapsed_Time.x_pos = 370;
     Elapsed_Time.y_pos = 45;
     Elapsed_Time.x_size = 100;
     Elapsed_Time.y_size = 25;
     Elapsed_Time.color_r = 0x40;
     Elapsed_Time.color_g = 0xE0;
     Elapsed_Time.color_b = 0x20;
     Elapsed_Time.font = 28;
     Elapsed_Time.options = OPT_FLAT;
     strcpy(Elapsed_Time.text, " 00 : 05");

     X_Position.x_pos = 10;
     X_Position.y_pos = 80;
     X_Position.x_size = 150;
     X_Position.y_size = 60;
     X_Position.color_r = 0xB0;
     X_Position.color_g = 0x30;
     X_Position.color_b = 0xE0;
     X_Position.font = 29;
     X_Position.options = OPT_FLAT;
     strcpy(X_Position.text, "+100.4 mm");

     Y_Position.x_pos = 170;
     Y_Position.y_pos = 80;
     Y_Position.x_size = 150;
     Y_Position.y_size = 60;
     Y_Position.color_r = 0xA0;
     Y_Position.color_g = 0x20;
     Y_Position.color_b = 0xD0;
     Y_Position.font = 29;
     Y_Position.options = OPT_FLAT;
     strcpy(Y_Position.text, "+ 74.3 mm");

     Z_Position.x_pos = 330;
     Z_Position.y_pos = 80;
     Z_Position.x_size = 150;
     Z_Position.y_size = 60;
     Z_Position.color_r = 0x90;
     Z_Position.color_g = 0x10;
     Z_Position.color_b = 0xC0;
     Z_Position.font = 29;
     Z_Position.options = OPT_FLAT;
     strcpy(Z_Position.text, "+220.8 mm");

     
}


void Make_Text_Structures (void)
{
     Text1.x_pos = 240;
     Text1.y_pos = 50;
     Text1.font = 30;
     Text1.options = 0;
     strcpy(Text1.text, "TEST1");

     Text2.x_pos = 350;
     Text2.y_pos = 50;
     Text2.font = 30;
     Text2.options = 0;
     strcpy(Text2.text, "TEST2");
}


void Make_Clock_Structures (void)
{
     Clock1.x_pos = 340;
     Clock1.y_pos = 180;
     Clock1.radius = 80;
     Clock1.options = 0;
     Clock1.hours = 10;
     Clock1.minutes = 10;
     Clock1.seconds = 28;
     Clock1.milliseconds = 0;
}


void Make_Progress_Bar_Structures (void)
{
     PrgBar1.x_pos = 240;
     PrgBar1.y_pos = 20;
     PrgBar1.width = 100;
     PrgBar1.height = 20;
     PrgBar1.options = 0;
     PrgBar1.value = 40;
     PrgBar1.range = 100;
}


void Make_Slider_Structures (void)
{
     Sldr1.x_pos = 30;
     Sldr1.y_pos = 220;
     Sldr1.width = 140;
     Sldr1.height = 20;
     Sldr1.options = 0;
     Sldr1.value = 55;
     Sldr1.range = 100;

     File_Select_Sldr.x_pos = 442;
     File_Select_Sldr.y_pos = 28;
     File_Select_Sldr.width = 16;
     File_Select_Sldr.height = 184;
     File_Select_Sldr.options = 0;
     File_Select_Sldr.value = 0;
     File_Select_Sldr.range = 180;
}


void Write_Bitmaps (void)
{
     Flash_Write_RGB565_Bitmap(RAM_G, Bitmap1, 64);               // Write 8x8 16-Bit Bitmap (128 Bytes) to RAM_G

     Flash_Write_RGB565_Bitmap((RAM_G + 128), Bitmap2, 64);       // Write 8x8 16-Bit Bitmap (128 Bytes) to RAM_G + 128

     Flash_Write_RGB565_Bitmap((RAM_G + 256), Bitmap3, 64);       // Write 8x8 16-Bit Bitmap (128 Bytes) to RAM_G + 256

     Flash_Write_RGB565_Bitmap((RAM_G + 384), Bitmap4, 64);       // Write 8x8 16-Bit Bitmap (128 Bytes) to RAM_G + 384

     Flash_Write_RGB332_Bitmap((RAM_G + 512), Bitmap8_1, 64);     // Write 8x8 8-Bit Bitmap (64 Bytes) to RAM_G + 512

     Flash_Write_RGB332_Bitmap((RAM_G + 576), Bitmap8_2, 64);     // Write 8x8 8-Bit Bitmap (64 Bytes) to RAM_G + 576

     Flash_Write_RGB332_Bitmap((RAM_G + 640), Bitmap8_3, 256);    // Write 16x16 8-Bit Bitmap (256 Bytes) to RAM_G + 640
}


void Write_RGB565_Bitmap(unsigned long Mem_Address, unsigned int* pRGB565_Array, unsigned int Num_Pixels)
{
     unsigned int i;

     for(i = 0; i < Num_Pixels; i++)
     {
          LCD_Mem_Write16((Mem_Address + (i * 2)), *(pRGB565_Array + i));
     }
}


void Write_RGB332_Bitmap(unsigned long Mem_Address, unsigned char* pRGB332_Array, unsigned int Num_Pixels)
{
     unsigned int i;

     for(i = 0; i < Num_Pixels; i++)
     {
          LCD_Mem_Write8((Mem_Address + i), *(pRGB332_Array + i));
     }
}


void Flash_Write_RGB565_Bitmap(unsigned long Mem_Address, unsigned int* pRGB565_Array, unsigned int Num_Pixels)
{
     unsigned int i;

     unsigned int Flash_Word;

     for(i = 0; i < Num_Pixels; i++)
     {
          Flash_Word = pgm_read_word_near(pRGB565_Array + i);
          
          LCD_Mem_Write16((Mem_Address + (i * 2)), Flash_Word); //Flash_Word);
     }
}


void Flash_Write_RGB332_Bitmap(unsigned long Mem_Address, unsigned char* pRGB332_Array, unsigned int Num_Pixels)
{
     unsigned int i;

     unsigned char Flash_Byte;

     for(i = 0; i < Num_Pixels; i++)
     {
          Flash_Byte = pgm_read_byte_near(pRGB332_Array + i);

          LCD_Mem_Write8((Mem_Address + i), Flash_Byte);
     }
}


void Flash_Write_Font_Metric_Block(unsigned long Mem_Address, unsigned char* pFont_Width_Array, unsigned char Format, unsigned char Line_Stride,
                                   unsigned char Width, unsigned char Height, unsigned long RAM_G_Addr)
{
     unsigned int i;

     unsigned char Flash_Byte;

     // Write 128 Bytes, Each byte is the width of each character.  A Font can define 128 characters.

     for(i = 0; i < 128; i++)
     {
          Flash_Byte = pgm_read_byte_near(pFont_Width_Array + i);

          LCD_Mem_Write8((Mem_Address + i), Flash_Byte);
     }

     // Write 4 Bytes, First byte written is the Format (LSB) followed by 3 bytes of Ox00 (MSBs).

     LCD_Mem_Write8((Mem_Address + 128), Format);              // L1 8 Pixels/Byte, L4 2 Pixels/Byte, L8 1 Pixel/Byte
     LCD_Mem_Write8((Mem_Address + 129), 0);
     LCD_Mem_Write8((Mem_Address + 130), 0);
     LCD_Mem_Write8((Mem_Address + 131), 0);

     // Write 4 Bytes, First byte written is the Line Stride (LSB) followed by 3 bytes of Ox00 (MSBs).

     LCD_Mem_Write8((Mem_Address + 132), Line_Stride);         // Line_Stride in bytes depends on Format
     LCD_Mem_Write8((Mem_Address + 133), 0);
     LCD_Mem_Write8((Mem_Address + 134), 0);
     LCD_Mem_Write8((Mem_Address + 135), 0);

     // Write 4 Bytes, First byte written is the Width (LSB) followed by 3 bytes of Ox00 (MSBs).

     LCD_Mem_Write8((Mem_Address + 136), Width);               // Width in Pixels
     LCD_Mem_Write8((Mem_Address + 137), 0);
     LCD_Mem_Write8((Mem_Address + 138), 0);
     LCD_Mem_Write8((Mem_Address + 139), 0);

     // Write 4 Bytes, First byte written is the Height (LSB) followed by 3 bytes of Ox00 (MSBs).

     LCD_Mem_Write8((Mem_Address + 140), Height);             // Height in Pixels
     LCD_Mem_Write8((Mem_Address + 141), 0);
     LCD_Mem_Write8((Mem_Address + 142), 0);
     LCD_Mem_Write8((Mem_Address + 143), 0);

     // Write 4 Bytes as Mem-Write32, the Address of the Font Data in RAM_G memory.

     LCD_Mem_Write32((Mem_Address + 144), RAM_G_Addr);
}


void Write_Font_Metric_Block_Single_Widget(unsigned long Mem_Address, unsigned char Format, unsigned char Line_Stride,
                                           unsigned char Width, unsigned char Height, unsigned long RAM_G_Addr)
{
     unsigned int i;

     // Write 128 Bytes of 0.

     for(i = 0; i < 128; i++)
     {
          LCD_Mem_Write8((Mem_Address + i), 0);
     }

     // Place Width of Single Widget in Byte 1

     LCD_Mem_Write8((Mem_Address + 1), Width);

     // Write 4 Bytes, First byte written is the Format (LSB) followed by 3 bytes of Ox00 (MSBs).

     LCD_Mem_Write8((Mem_Address + 128), Format);              // L1 8 Pixels/Byte, L4 2 Pixels/Byte, L8 1 Pixel/Byte
     LCD_Mem_Write8((Mem_Address + 129), 0);
     LCD_Mem_Write8((Mem_Address + 130), 0);
     LCD_Mem_Write8((Mem_Address + 131), 0);

     // Write 4 Bytes, First byte written is the Line Stride (LSB) followed by 3 bytes of Ox00 (MSBs).

     LCD_Mem_Write8((Mem_Address + 132), Line_Stride);         // Line_Stride in bytes depends on Format
     LCD_Mem_Write8((Mem_Address + 133), 0);
     LCD_Mem_Write8((Mem_Address + 134), 0);
     LCD_Mem_Write8((Mem_Address + 135), 0);

     // Write 4 Bytes, First byte written is the Width (LSB) followed by 3 bytes of Ox00 (MSBs).

     LCD_Mem_Write8((Mem_Address + 136), Width);               // Width in Pixels
     LCD_Mem_Write8((Mem_Address + 137), 0);
     LCD_Mem_Write8((Mem_Address + 138), 0);
     LCD_Mem_Write8((Mem_Address + 139), 0);

     // Write 4 Bytes, First byte written is the Height (LSB) followed by 3 bytes of Ox00 (MSBs).

     LCD_Mem_Write8((Mem_Address + 140), Height);             // Height in Pixels
     LCD_Mem_Write8((Mem_Address + 141), 0);
     LCD_Mem_Write8((Mem_Address + 142), 0);
     LCD_Mem_Write8((Mem_Address + 143), 0);

     // Write 4 Bytes as Mem-Write32, the Address of the Font Data in RAM_G memory.

     LCD_Mem_Write32((Mem_Address + 144), RAM_G_Addr);
}


void Make_File_List_Bitmap (void)
{
     unsigned int X_Pixels, Y_Pixels;

     for(X_Pixels = 0; X_Pixels < FILE_LIST_X_DIM; X_Pixels++)
     {
          for(Y_Pixels = 0; Y_Pixels < FILE_LIST_Y_DIM; Y_Pixels++)
          {
               if((Y_Pixels / 40) % 2)
               {
                    LCD_Mem_Write8( (FILE_LIST_RAM_G_ADDR + ((unsigned long) Y_Pixels * FILE_LIST_X_DIM) + X_Pixels), 0x0C);
               }
               else
               {
                    LCD_Mem_Write8( (FILE_LIST_RAM_G_ADDR + ((unsigned long) Y_Pixels * FILE_LIST_X_DIM) + X_Pixels), 0x08);
               }
          }
     }

     Draw_Text_String(FILE_LIST_RAM_G_ADDR, 6, 14, 1, 0xFF, "Rocktopus1");
     Draw_Text_String(FILE_LIST_RAM_G_ADDR, 6, 54, 1, 0xFF, "Lulzbot._Keychain.gcode");
     Draw_Text_String(FILE_LIST_RAM_G_ADDR, 6, 94, 1, 0xFF, "stand_3_nGen.gcode");
     Draw_Text_String(FILE_LIST_RAM_G_ADDR, 6, 134, 1, 0xFF, "Lulzbot_keychain-nG.gcode");
     Draw_Text_String(FILE_LIST_RAM_G_ADDR, 6, 174, 1, 0xFF, "Interlocking_Wobulator_Bearing.gcode");
     Draw_Text_String(FILE_LIST_RAM_G_ADDR, 4, 215, 0, 0xFF, "/Lulzbot/Demos/Really_Long_Rocktopus_File_Name.g");
     Draw_Text_String(FILE_LIST_RAM_G_ADDR, 4, 254, 1, 0xFF, "Bearing_Guide.gcode");
}


void Make_Logo_Bitmap (unsigned char Color_2)
{
     unsigned int Array_Index, Pix_Count;

     unsigned char RGB332_Byte, Pixel_Byte, Pix[4];

     unsigned long Test_Address;



     for(Array_Index = 0; Array_Index < 10000; Array_Index++)
     {
          Pixel_Byte = pgm_read_byte_near(&(Lulzbot_Logo200_GL[Array_Index]));

          Pix[0] = (Pixel_Byte >> 6) & 0x03;
          Pix[1] = (Pixel_Byte >> 4) & 0x03;
          Pix[2] = (Pixel_Byte >> 4) & 0x03;
          Pix[3] =  Pixel_Byte & 0x03;

          for(Pix_Count = 0; Pix_Count < 4; Pix_Count++)
          {
               switch(Pix[Pix_Count])
               {
                    case 0:
                         RGB332_Byte = COLOR0;
                         break;
                    case 1:
                         RGB332_Byte = COLOR1;
                         break;
                    case 2:
                         RGB332_Byte = Color_2;
                         break;
                    case 3:
                         RGB332_Byte = COLOR3;
                         break;
                    default:
                         RGB332_Byte = COLOR1;
               }

               LCD_Mem_Write8( (LOGO_RAM_G_ADDR + ((unsigned long) Array_Index * 4) + (unsigned long) Pix_Count), RGB332_Byte);

               if(Array_Index == 1024)
               {
                     Test_Address = (LOGO_RAM_G_ADDR + ((unsigned long) Array_Index * 4) + (unsigned long) Pix_Count);

                     Serial.print("Test_Address = 0x");
                     Serial.println(Test_Address, HEX);
               }
          }
     }
}

// Draw a String of 10-Pixel or 12-Pixel Characters
// Bitmaps of Characters are Arrays in AB10ary.h and AB12ary.h Header Files

int Draw_Text_String (unsigned long Ram_G_Address, unsigned int X_Char_Pos,
                      unsigned int Y_Char_Pos, unsigned char Font_Type, unsigned char Color, char* sptr)
{
     unsigned int X_Dim, Y_Dim, Y_Offset, Char_Offset;

     unsigned int X_Index, Y_Index;

     unsigned int X_Mask;

     unsigned int X_Pattern;

     unsigned int Array_Pointer;

     unsigned char Str_Index;

     unsigned long Test_Address;


     Char_Offset = 0;

     for(Str_Index = 0; Str_Index < strlen(sptr); Str_Index++)
     {
          // Select Font Type

          if (Font_Type == 0)
          {
               X_Dim = pgm_read_byte(&(AB10_Array[(*(sptr + Str_Index) - 0x20)][0]));
               Y_Dim = pgm_read_byte(&(AB10_Array[(*(sptr + Str_Index) - 0x20)][1]));
               Y_Offset = pgm_read_byte(&(AB10_Array[(*(sptr + Str_Index) - 0x20)][2]));
          }

          if (Font_Type == 1)
          {
               X_Dim = pgm_read_byte(&(AB12_Array[(*(sptr + Str_Index) - 0x20)][0]));
               Y_Dim = pgm_read_byte(&(AB12_Array[(*(sptr + Str_Index) - 0x20)][1]));
               Y_Offset = pgm_read_byte(&(AB12_Array[(*(sptr + Str_Index) - 0x20)][2]));
          }

          Array_Pointer = 3;

          for(Y_Index = 0; Y_Index < Y_Dim; Y_Index++)
          {
               if(Font_Type == 0)
               {
                    X_Pattern = pgm_read_byte(&(AB10_Array[(*(sptr + Str_Index) - 0x20)][Array_Pointer]));
               }

               if(Font_Type == 1)
               {
                    X_Pattern = pgm_read_byte(&(AB12_Array[(*(sptr + Str_Index) - 0x20)][Array_Pointer]));
               }

               X_Pattern <<= 8;
               Array_Pointer++;

               if(X_Dim > 8)
               {
                    if(Font_Type == 0)
                    {
                         X_Pattern += pgm_read_byte(&(AB10_Array[(*(sptr + Str_Index) - 0x20)][Array_Pointer]));
                    }

                    if(Font_Type == 1)
                    {
                         X_Pattern += pgm_read_byte(&(AB12_Array[(*(sptr + Str_Index) - 0x20)][Array_Pointer]));
                    }

                    Array_Pointer++;
               }

               X_Mask = 0x8000;

               for(X_Index = 0; X_Index < X_Dim; X_Index++)
               {
                     if(X_Pattern & X_Mask)
                    {
                        LCD_Mem_Write8((Ram_G_Address + ((unsigned long)(Y_Char_Pos + Y_Index) * FILE_LIST_X_DIM) + (X_Char_Pos + X_Index + Char_Offset)), Color);
                    }

                    X_Mask >>= 1;
               }
          }

          Char_Offset += (X_Dim + 1);
     }

     return(X_Dim);
}


void Calibrate_LCD (void)
{

 //    Touch_Transform_A = LCD_Mem_Read32(REG_TOUCH_TRANSFORM_A);

 //    Touch_Transform_B = LCD_Mem_Read32(REG_TOUCH_TRANSFORM_B);

 //    Touch_Transform_C = LCD_Mem_Read32(REG_TOUCH_TRANSFORM_C);

 //    Touch_Transform_D = LCD_Mem_Read32(REG_TOUCH_TRANSFORM_D);

 //    Touch_Transform_E = LCD_Mem_Read32(REG_TOUCH_TRANSFORM_E);

 //    Touch_Transform_F = LCD_Mem_Read32(REG_TOUCH_TRANSFORM_F);




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


void LCD_Cmd_Color_RGB (unsigned char Red, unsigned char Grn, unsigned char Blu)    // Set Clear Screen Color to the DL
{
     unsigned long Value32;

     Value32 = (unsigned long) Red;
     Value32 <<= 8;
     Value32 |= (unsigned long) Grn;
     Value32 <<= 8;
     Value32 |= ((unsigned long) Blu | COLOR_RGB);

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
     Value32 |= (unsigned long) Blu;

     LCD_Command32(CMD_BGCOLOR);

     LCD_Command32(Value32);
}

void LCD_Cmd_Set_Draw_Color (unsigned long Color_Value_RGB)
{
     unsigned long DL_Cmd;

     DL_Cmd = 0x04000000 | Color_Value_RGB;

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



void LCD_Cmd_Draw_Text(struct Text_Struct *Txt)
{
     unsigned long Value32;

     unsigned int Str_Length, Str_Remain;

     unsigned char i;

     char C_Buf[4];

     char* Str_Ptr;

     // Send TEXT Command, Position, Font and Options

     LCD_Command32(CMD_TEXT);

     Value32 = (unsigned long) Txt->y_pos;
     Value32 <<= 16;
     Value32 |= (unsigned long) Txt->x_pos;

     LCD_Command32(Value32);

     Value32 = (unsigned long) Txt->options;
     Value32 <<= 16;
     Value32 |= (unsigned long) Txt->font;
     LCD_Command32(Value32);

     // 4 Characted Text String as a 32-Bit Value
    
     Str_Length = strlen(Txt->text);

     Str_Remain = Str_Length % 4;

     // Make Value32s for Full 4-Character Strings

     for(i = 0; i < (Str_Length / 4); i++)
     {
          Str_Ptr = Txt->text + (i * 4);

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

     Str_Ptr = Txt->text + Str_Length - Str_Remain;

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


void LCD_Cmd_Draw_Clock (struct Clock_Struct *Clk)
{
     unsigned long Value32;

     // Send CLOCK Command, Position, Radius, Options, Hours, Minutes, Seconds and Milliseconds

     LCD_Command32(CMD_CLOCK);

     Value32 = (unsigned long) Clk->y_pos;
     Value32 <<= 16;
     Value32 |= (unsigned long) Clk->x_pos;
     LCD_Command32(Value32);

     Value32 = (unsigned long) Clk->options;
     Value32 <<= 16;
     Value32 |= (unsigned long) Clk->radius;
     LCD_Command32(Value32);

     Value32 = (unsigned long) Clk->minutes;
     Value32 <<= 16;
     Value32 |= (unsigned long) Clk->hours;
     LCD_Command32(Value32);

     Value32 = (unsigned long) Clk->milliseconds;
     Value32 <<= 16;
     Value32 |= (unsigned long) Clk->seconds;
     LCD_Command32(Value32);
}


void LCD_Cmd_Draw_Progress_Bar (struct Progress_Struct *Prg)
{
     unsigned long Value32;

     // Send PROGRESS Command, Position, Width, Height, Options, Value and Range

     LCD_Command32(CMD_PROGRESS);

     Value32 = (unsigned long) Prg->y_pos;
     Value32 <<= 16;
     Value32 |= (unsigned long) Prg->x_pos;
     LCD_Command32(Value32);

     Value32 = (unsigned long) Prg->height;
     Value32 <<= 16;
     Value32 |= (unsigned long) Prg->width;
     LCD_Command32(Value32);

     Value32 = (unsigned long) Prg->value;
     Value32 <<= 16;
     Value32 |= (unsigned long) Prg->options;
     LCD_Command32(Value32);

     Value32 |= (unsigned long) Prg->range;
     LCD_Command32(Value32);
}


void LCD_Cmd_Draw_Slider (struct Slider_Struct *Sldr)
{
     unsigned long Value32;

     // Send PROGRESS Command, Position, Width, Height, Options, Value and Range

     LCD_Command32(CMD_SLIDER);

     Value32 = (unsigned long) Sldr->y_pos;
     Value32 <<= 16;
     Value32 |= (unsigned long) Sldr->x_pos;
     LCD_Command32(Value32);

     Value32 = (unsigned long) Sldr->height;
     Value32 <<= 16;
     Value32 |= (unsigned long) Sldr->width;
     LCD_Command32(Value32);

     Value32 = (unsigned long) Sldr->value;
     Value32 <<= 16;
     Value32 |= (unsigned long) Sldr->options;
     LCD_Command32(Value32);

     Value32 |= (unsigned long) Sldr->range;
     LCD_Command32(Value32);
}


void LCD_Cmd_Set_Tag (unsigned char Tag)
{
     unsigned long Value32;

     Value32 = TAG | (unsigned long) Tag;

     LCD_Command32(Value32);
}


void LCD_Cmd_Bitmap_Source (unsigned long RAM_G_Addr)
{
     unsigned long Value32;

     Value32 = RAM_G_Addr & 0x000FFFFF;
     Value32 |= BITMAP_SOURCE;

     LCD_Command32(Value32);
}


void LCD_Cmd_Bitmap_Layout (unsigned char format, unsigned int linestride, unsigned int height)
{
     unsigned long Value32;

     Value32 = (unsigned long) format;
     Value32 <<= 10;
     Value32 |= (unsigned long) linestride;
     Value32 <<= 9;
     Value32 |= (unsigned long) height;
     Value32 |= BITMAP_LAYOUT;

     LCD_Command32(Value32);
}


void LCD_Cmd_Bitmap_Size(unsigned char filter, unsigned char wrapx, unsigned char wrapy, unsigned int width, unsigned char height)
{
     unsigned long Value32;

     Value32 = (unsigned long) filter;
     Value32 <<= 1;
     Value32 |= (unsigned long) wrapx;
     Value32 <<= 1;
     Value32 |= (unsigned long) wrapy;
     Value32 <<= 1;
     Value32 |= (unsigned long) width;
     Value32 <<= 9;
     Value32 |= (unsigned long) height;
     Value32 |= BITMAP_SIZE;

     LCD_Command32(Value32);
}


void LCD_Cmd_Bitmap_Handle (unsigned int Handle)
{
     unsigned long Value32;

     Value32 = BITMAP_HANDLE | (unsigned long) Handle;

     LCD_Command32(Value32);
}


void LCD_Cmd_Set_Font (unsigned long Font, unsigned long Font_RAM_G_Addr)
{
     LCD_Command32(CMD_SETFONT);

     LCD_Command32(Font);

     LCD_Command32(Font_RAM_G_Addr);
}


void LCD_Cmd_Begin (unsigned long Primative)
{
     unsigned long Value32;

     Value32 = BEGIN + Primative;

     LCD_Command32(Value32);
}


void LCD_Cmd_Vertex_2II (unsigned int X_Coord, unsigned int Y_Coord, unsigned int B_Handle, unsigned int Cell)
{
     unsigned long Value32;

     Value32 |= (unsigned long) X_Coord;
     Value32 <<= 9;
     Value32 |= (unsigned long) Y_Coord;
     Value32 <<= 5;
     Value32 |= (unsigned long) B_Handle;
     Value32 <<= 7;
     Value32 |= Cell;
     Value32 |= VERTEX2II;

     LCD_Command32(Value32);
}


// DISPLAY LIST FUNCTIONS - These function write to an address in the Display List memory space.

void LCD_DL_Begin (unsigned long Addr, unsigned long Primative)
{
     unsigned long Value32;

     Value32 = BEGIN + Primative;

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


void LCD_DL_Bitmap_Source (unsigned long Addr, unsigned long RAM_G_Addr)
{
     unsigned long Value32;

     Value32 = RAM_G_Addr & 0x000FFFFF;
     Value32 |= BITMAP_SOURCE;

     LCD_Mem_Write32(Addr,Value32);
}


void LCD_DL_Bitmap_Layout (unsigned long Addr, unsigned char format, unsigned int linestride, unsigned int height)
{
     unsigned int Value32;

     Value32 = (unsigned long) format;
     Value32 <<= 5;
     Value32 |= (unsigned long) linestride;
     Value32 <<= 11;
     Value32 |= (unsigned long) height;
     Value32 |= BITMAP_LAYOUT;

     LCD_Mem_Write32(Addr,Value32);
}


void LCD_DL_Bitmap_Size(unsigned long Addr, unsigned char filter, unsigned char wrapx, unsigned char wrapy, unsigned int width, unsigned char height)
{
     unsigned int Value32;

     Value32 = (unsigned long) filter;
     Value32 <<= 1;
     Value32 |= (unsigned long) wrapx;
     Value32 <<= 1;
     Value32 |= (unsigned long) wrapy;
     Value32 <<= 1;
     Value32 |= (unsigned long) width;
     Value32 <<= 9;
     Value32 |= (unsigned long) height;
     Value32 |= BITMAP_SIZE;

     LCD_Mem_Write32(Addr,Value32);     
}

void LCD_DL_Tag (unsigned long Addr, unsigned char Tag)
{
     unsigned long Value32;

     Value32 = TAG | (unsigned long) Tag;

     LCD_Mem_Write32(Addr, Value32);
}


void Main_Screen (void)
{
     // Initial Display Commands

     LCD_Cmd_DL_Start();

     LCD_Cmd_Clear_Color_RGB(0x80, 0x80, 0x80);

     LCD_Cmd_Clear(1,1,1);

     // Button Commands

     // Set Combo Button

     switch(Combo_Button_State)
     {
          case 0:                                         // PRINT Button - Not Pressed
               Combo_Button.color_r = 0x20;
               Combo_Button.color_g = 0xD0;
               Combo_Button.color_b = 0x20;
               Combo_Button.options = OPT_3D;
               strcpy(Combo_Button.text, "PRINT");
               break;
          case 1:                                         // PAUSE Button - Not Pressed
               Combo_Button.color_r = 0xD0;
               Combo_Button.color_g = 0x80;
               Combo_Button.color_b = 0x20;
               Combo_Button.options = OPT_3D;
               strcpy(Combo_Button.text, "PAUSE");
               break;
          case 2:                                         // CONTINUE Button - Not Pressed
               Combo_Button.color_r = 0x20;
               Combo_Button.color_g = 0xD0;
               Combo_Button.color_b = 0x80;
               Combo_Button.options = OPT_3D;
               strcpy(Combo_Button.text, "CONTINUE");
               break;
          case 3:                                         // PRINT Button - Pressed
               Combo_Button.color_r = 0x20;
               Combo_Button.color_g = 0xD0;
               Combo_Button.color_b = 0x20;
               Combo_Button.options = OPT_FLAT;
               strcpy(Combo_Button.text, "PRINT");
               break;
          case 4:                                         // PAUSE Button - Pressed
               Combo_Button.color_r = 0xD0;
               Combo_Button.color_g = 0x80;
               Combo_Button.color_b = 0x20;
               Combo_Button.options = OPT_FLAT;
               strcpy(Combo_Button.text, "PAUSE");
               break;
          case 5:                                         // CONTINUE Button - Pressed
               Combo_Button.color_r = 0x20;
               Combo_Button.color_g = 0xD0;
               Combo_Button.color_b = 0x80;
               Combo_Button.options = OPT_FLAT;
               strcpy(Combo_Button.text, "CONTINUE");
               break;
          default:                                        // PRINT Button - Not Pressed
               Combo_Button.color_r = 0x20;
               Combo_Button.color_g = 0xD0;
               Combo_Button.color_b = 0x20;
               Combo_Button.options = OPT_3D;
               strcpy(Combo_Button.text, "PRINT");
     }

     switch(Stop_Button_State)
     {
          case 0:                                         // Stop Button - Not Pressed
               Stop_Button.color_r = 0xF0;
               Stop_Button.color_g = 0x20;
               Stop_Button.color_b = 0x20;
               Stop_Button.options = OPT_3D;
               strcpy(Stop_Button.text, "STOP");
               break;
          case 1:                                         // Stop Button - Pressed
               Stop_Button.color_r = 0xF0;
               Stop_Button.color_g = 0x20;
               Stop_Button.color_b = 0x20;
               Stop_Button.options = OPT_FLAT;
               strcpy(Stop_Button.text, "STOP");
               break;
          default:                                        // STOP Button - Not Pressed
               Stop_Button.color_r = 0xF0;
               Stop_Button.color_g = 0x20;
               Stop_Button.color_b = 0x20;
               Stop_Button.options = OPT_3D;
               strcpy(Stop_Button.text, "STOP");
     }

     switch(Menu_Button_State)
     {
          case 0:                                         // MENU Button - Not Pressed
               Menu_Button.color_r = 0xF0;
               Menu_Button.color_g = 0x20;
               Menu_Button.color_b = 0x20;
               Menu_Button.options = OPT_3D;
               strcpy(Menu_Button.text, "MENU");
               break;
          case 1:                                         // MENU Button - Pressed
               Menu_Button.color_r = 0xF0;
               Menu_Button.color_g = 0x20;
               Menu_Button.color_b = 0x20;
               Menu_Button.options = OPT_FLAT;
               strcpy(Menu_Button.text, "MENU");
               break;
          default:                                        // MENU Button - Not Pressed
               Menu_Button.color_r = 0xF0;
               Menu_Button.color_g = 0x20;
               Menu_Button.color_b = 0x20;
               Menu_Button.options = OPT_3D;
               strcpy(Menu_Button.text, "MENU");
     }


     // Draw Combo (PRINT - PAUSE - CONTINUE) Button

     LCD_Cmd_Set_Foreground_Color(Combo_Button.color_r, Combo_Button.color_g, Combo_Button.color_b);

     LCD_Cmd_Draw_Button(&Combo_Button);


     // Draw STOP Button

     LCD_Cmd_Set_Foreground_Color(Stop_Button.color_r, Stop_Button.color_g, Stop_Button.color_b);

     LCD_Cmd_Draw_Button(&Stop_Button);


     // Draw MENU Button

     LCD_Cmd_Set_Foreground_Color(Menu_Button.color_r, Menu_Button.color_g, Menu_Button.color_b);

     LCD_Cmd_Draw_Button(&Menu_Button);


     // Draw USB Button

     LCD_Cmd_Set_Foreground_Color(USB_Button.color_r, USB_Button.color_g, USB_Button.color_b);

     LCD_Cmd_Draw_Button(&USB_Button);


     // Draw Extruder Temperature Button as Flat Button for Display Only

     LCD_Cmd_Set_Foreground_Color(Extruder_Temp.color_r, Extruder_Temp.color_g, Extruder_Temp.color_b);

     LCD_Cmd_Draw_Button(&Extruder_Temp);


     // Draw Bed Heat Button as Flat Button for Display Only

     LCD_Cmd_Set_Foreground_Color(Bed_Heat.color_r, Bed_Heat.color_g, Bed_Heat.color_b);

     LCD_Cmd_Draw_Button(&Bed_Heat);


     // Draw Fan Percent Button as Flat Button for Display Only

     LCD_Cmd_Set_Foreground_Color(Fan_Percent.color_r, Fan_Percent.color_g, Fan_Percent.color_b);

     LCD_Cmd_Draw_Button(&Fan_Percent);


     // Draw Print Percent Button as Flat Button for Display Only

     LCD_Cmd_Set_Foreground_Color(Print_Percent.color_r, Print_Percent.color_g, Print_Percent.color_b);

     LCD_Cmd_Draw_Button(&Print_Percent);


     // Draw Elapsed Time Button as Flat Button for Display Only

     LCD_Cmd_Set_Foreground_Color(Elapsed_Time.color_r, Elapsed_Time.color_g, Elapsed_Time.color_b);

     LCD_Cmd_Draw_Button(&Elapsed_Time);


     // Draw X Position Button as Flat Button for Display Only

     LCD_Cmd_Set_Foreground_Color(X_Position.color_r, X_Position.color_g, X_Position.color_b);

     LCD_Cmd_Draw_Button(&X_Position);


     // Draw Y Position Button as Flat Button for Display Only

     LCD_Cmd_Set_Foreground_Color(Y_Position.color_r, Y_Position.color_g, Y_Position.color_b);

     LCD_Cmd_Draw_Button(&Y_Position);


     // Draw Z Position Button as Flat Button for Display Only

     LCD_Cmd_Set_Foreground_Color(Z_Position.color_r, Z_Position.color_g, Z_Position.color_b);

     LCD_Cmd_Draw_Button(&Z_Position);



     // Load USB Thumb Drive Bitmap into RAM_G + 900

     Flash_Write_RGB332_Bitmap((RAM_G + 900), TD_Widget, 140);


     // Load Extruder Bitmap into RAM_G + 1040

     Flash_Write_RGB332_Bitmap((RAM_G + 1040), Extruder_Widget, 69);


     // Load Bed Heat Bitmap into RAM_G + 1109

     Flash_Write_RGB332_Bitmap((RAM_G + 1109), Bed_Heat_Widget,92);


     // Load Fan Percent Bitmap into RAM_G + 1201

     Flash_Write_RGB332_Bitmap((RAM_G + 1201), Fan_Widget,128);


     // Draw Thumb Drive Bitmap on USB Button

     LCD_Cmd_Bitmap_Source(RAM_G + 900);

     LCD_Cmd_Bitmap_Layout(L1, 7, 20);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 50, 20);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(308, 235, 0, 0);




     // Draw Extruder Bitmap on Extruder Temperature Button

     LCD_Cmd_Bitmap_Source(RAM_G + 1040);

     LCD_Cmd_Bitmap_Layout(L1, 3, 23);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 24, 23);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(25, 30, 0, 0);



     // Draw Bed Heat Bitmap on Bed Heat Button

     LCD_Cmd_Bitmap_Source(RAM_G + 1109);

     LCD_Cmd_Bitmap_Layout(L1, 4, 23);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 32, 23);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(140, 30, 0, 0);


     // Draw Fan Percent Bitmap on Bed Heat Button

     LCD_Cmd_Bitmap_Source(RAM_G + 1201);

     LCD_Cmd_Bitmap_Layout(L1, 4, 32);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 32, 32);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(261, 25, 0, 0);


     


     LCD_Cmd_End();

     LCD_Cmd_Display();

     LCD_Cmd_Swap();
}


void Command_List (void)
{
     // Initial Display Commands

     LCD_Cmd_DL_Start();

     LCD_Cmd_Clear_Color_RGB(0x80,0x80,0x80);

     LCD_Cmd_Clear(1,1,1);

     // Button Commands

     LCD_Cmd_Color_RGB(0x00, 0xFF, 0xFF);

     LCD_Cmd_Draw_Text(&Text1);

     LCD_Cmd_Draw_Text(&Text2);

     LCD_Cmd_Color_RGB(0xFF, 0x80, 0x00);

     LCD_Cmd_Draw_Clock(&Clock1);

     LCD_Cmd_Color_RGB(0xFF, 0xFF, 0x20);

     LCD_Cmd_Draw_Progress_Bar(&PrgBar1);

     LCD_Cmd_Set_Foreground_Color(0x20, 0xD0, 0x20);

     LCD_Cmd_Set_Background_Color(0x10, 0xB0, 0x10);

     LCD_Cmd_Color_RGB(0x80, 0xFF, 0x80);

     LCD_Cmd_Draw_Slider(&Sldr1);

     LCD_Cmd_End();
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
     

     LCD_DL_Bitmap_Source((RAM_DL + 168), RAM_G);

     LCD_DL_Bitmap_Layout((RAM_DL + 172), RGB565, 16, 8);

     LCD_DL_Bitmap_Size((RAM_DL + 176), NEAREST, BORDER, BORDER, 8, 8);

     LCD_DL_Begin((RAM_DL + 180), BEGIN_BITMAPS);

     LCD_DL_Vertex_2II((RAM_DL + 184), 220, 100, 0, 0);

     

     LCD_DL_End((RAM_DL + 188));

     LCD_DL_Display((RAM_DL + 192));


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

     // Set Text Structure Values

     Make_Text_Structures();

     // Set Clock Structure Values

     Make_Clock_Structures();

     // Set Progress Bar Structure Values

     Make_Progress_Bar_Structures();

     // Set Progress Bar Structure Values

     Make_Slider_Structures();

     // Reset the Command FIFO Buffer

     LCD_Cmd_Buf_Reset();

     // Delay to Set Up Command Buffer

     delay(100);

     Write_Bitmaps();

     Make_File_List_Bitmap();

     // Turn On LCD Backlight

     LCD_Set_Backlight (0x00FA, 128);

     // Write and Display Blue Logo

     Make_Logo_Bitmap(0x03);

     // Initial Display Commands

     LCD_Cmd_DL_Start();

     LCD_Cmd_Clear_Color_RGB(0x49,0x49,0x50);

     LCD_Cmd_Clear(1,1,1);

     // Set Drawing Color to White for Full-Brightness Bitmaps

     LCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);

     LCD_Cmd_Bitmap_Source(LOGO_RAM_G_ADDR);

     LCD_Cmd_Bitmap_Layout(RGB332, 200, 200);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 200, 200);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(140, 36, 0, 0);

     LCD_Cmd_End();

     LCD_Cmd_Display();

     LCD_Cmd_Swap();
     

     delay(500);


     // Write and Display Yellow Logo

     Make_Logo_Bitmap(0xFC);
     
     // Initial Display Commands

     LCD_Cmd_DL_Start();

     LCD_Cmd_Clear_Color_RGB(0x49,0x49,0x50);

     LCD_Cmd_Clear(1,1,1);

     // Set Drawing Color to White for Full-Brightness Bitmaps

     LCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);

     LCD_Cmd_Bitmap_Source(LOGO_RAM_G_ADDR);

     LCD_Cmd_Bitmap_Layout(RGB332, 200, 200);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 200, 200);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(140, 36, 0, 0);

     LCD_Cmd_End();

     LCD_Cmd_Display();

     LCD_Cmd_Swap();
     

     delay(500);

     // Write and Display Red Logo
     
     Make_Logo_Bitmap(0xE0);
  
     // Initial Display Commands

     LCD_Cmd_DL_Start();

     LCD_Cmd_Clear_Color_RGB(0x49,0x49,0x50);

     LCD_Cmd_Clear(1,1,1);

     // Set Drawing Color to White for Full-Brightness Bitmaps

     LCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);

     LCD_Cmd_Bitmap_Source(LOGO_RAM_G_ADDR);

     LCD_Cmd_Bitmap_Layout(RGB332, 200, 200);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 200, 200);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(140, 36, 0, 0);

     LCD_Cmd_End();

     LCD_Cmd_Display();

     LCD_Cmd_Swap();
     

     delay(500);

     // Write and Display Purple Logo

     Make_Logo_Bitmap(0xC3);
 
     // Initial Display Commands

     LCD_Cmd_DL_Start();

     LCD_Cmd_Clear_Color_RGB(0x49,0x49,0x50);

     LCD_Cmd_Clear(1,1,1);

     // Set Drawing Color to White for Full-Brightness Bitmaps

     LCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);

     LCD_Cmd_Bitmap_Source(LOGO_RAM_G_ADDR);

     LCD_Cmd_Bitmap_Layout(RGB332, 200, 200);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 200, 200);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(140, 36, 0, 0);

     LCD_Cmd_End();

     LCD_Cmd_Display();

     LCD_Cmd_Swap();


     delay(500);

     // Write and Display Standard Green Logo
  
     Make_Logo_Bitmap(0xD8);
 
     // Initial Display Commands

     LCD_Cmd_DL_Start();

     LCD_Cmd_Clear_Color_RGB(0x49,0x49,0x50);

     LCD_Cmd_Clear(1,1,1);

     // Set Drawing Color to White for Full-Brightness Bitmaps

     LCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);

     LCD_Cmd_Bitmap_Source(LOGO_RAM_G_ADDR);

     LCD_Cmd_Bitmap_Layout(RGB332, 200, 200);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 200, 200);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(140, 36, 0, 0);

     LCD_Cmd_End();

     LCD_Cmd_Display();

     LCD_Cmd_Swap();


     delay(1000);


     // Display Main Screen

     Main_Screen();
     


     delay(10000);

     

     // Initial Display Commands

     LCD_Cmd_DL_Start();

     LCD_Cmd_Clear_Color_RGB(0x80,0x80,0x80);

     LCD_Cmd_Clear(1,1,1);

     // Button Commands

     LCD_Cmd_Color_RGB(0x00, 0xFF, 0xFF);

     LCD_Cmd_Draw_Text(&Text1);

     LCD_Cmd_Draw_Text(&Text2);

     LCD_Cmd_Color_RGB(0xFF, 0x80, 0x00);

     LCD_Cmd_Draw_Clock(&Clock1);

     LCD_Cmd_Color_RGB(0xFF, 0xFF, 0x20);

     LCD_Cmd_Draw_Progress_Bar(&PrgBar1);

     LCD_Cmd_Set_Foreground_Color(0x20, 0xD0, 0x20);

     LCD_Cmd_Set_Background_Color(0x10, 0xB0, 0x10);

     LCD_Cmd_Color_RGB(0x80, 0xFF, 0x80);

     LCD_Cmd_Draw_Slider(&Sldr1);


     delay(1000);

     
     // Set Drawing Color to White for Full-Brightness Bitmaps

     LCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);

     // Bitmap 1 - RGB565 - 8x8 - RAM_G

     LCD_Cmd_Bitmap_Source(RAM_G);

     LCD_Cmd_Bitmap_Layout(RGB565, 16, 8);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 8, 8);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(220, 100, 0, 0);

    
     // Bitmap 2 - RGB565 - 8x8 - RAM_G + 128

     LCD_Cmd_Bitmap_Source(RAM_G + 128);

     LCD_Cmd_Bitmap_Layout(RGB565, 16, 8);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 8, 8);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(220, 120, 0, 0);


     // Bitmap 3 - RGB565 - 8x8 - RAM_G + 256    

     LCD_Cmd_Bitmap_Source(RAM_G + 256);

     LCD_Cmd_Bitmap_Layout(RGB565, 16, 8);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 8, 8);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(220, 140, 0, 0);


     // Bitmap 4 - RGB565 - 8x8 - RAM_G + 384    

     LCD_Cmd_Bitmap_Source(RAM_G + 384);

     LCD_Cmd_Bitmap_Layout(RGB565, 16, 8);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 8, 8);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(220, 160, 0, 0);


     // Bitmap 8_1 - RGB332 - 8x8 - RAM_G + 512    

     LCD_Cmd_Bitmap_Source(RAM_G + 512);

     LCD_Cmd_Bitmap_Layout(RGB332, 8, 8);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 8, 8);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(220, 180, 0, 0);


     // Bitmap 8_2 - RGB332 - 8x8 - RAM_G + 576    

     LCD_Cmd_Bitmap_Source(RAM_G + 576);

     LCD_Cmd_Bitmap_Layout(RGB332, 8, 8);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 8, 8);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(220, 200, 0, 0);


     // Bitmap 8_3 - RGB332 - 16x16 - RAM_G + 640

     LCD_Cmd_Bitmap_Source(RAM_G + 640);

     LCD_Cmd_Bitmap_Layout(RGB332, 16, 16);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 16, 16);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(220, 220, 0, 0);

     LCD_Cmd_End();

     LCD_Cmd_Display();

     LCD_Cmd_Swap();


     delay(3000);

     // Initial Display Commands

     LCD_Cmd_DL_Start();

     LCD_Cmd_Clear_Color_RGB(0x80,0x80,0x80);

     LCD_Cmd_Clear(1,1,1);

     delay(2000);

     // Set Drawing Color to White for Full-Brightness Bitmaps

     LCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);


     // Draw File List Bitmap

     LCD_Cmd_Bitmap_Source(FILE_LIST_RAM_G_ADDR);

     LCD_Cmd_Bitmap_Layout(RGB332, 400, 200);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 400, 200);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(20, 20, 0, 0);

     LCD_Cmd_End();

     LCD_Cmd_Display();

     LCD_Cmd_Swap();

}


void loop()
{
     Scroll_Position = (float) File_Select_Sldr.value / 180.0 * 200.0;

     Int_Scroll_Position = (unsigned int) Scroll_Position;

     
     LCD_Cmd_DL_Start();

     LCD_Cmd_Clear_Color_RGB(0x80,0x80,0x80);

     LCD_Cmd_Clear(1,1,1);

     
     // Draw File List Bitmap

     // Set Drawing Color to White for Full-Brightness Bitmaps

     LCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);

     LCD_Cmd_Bitmap_Source(FILE_LIST_RAM_G_ADDR + ((unsigned long) Int_Scroll_Position * 400));

     LCD_Cmd_Bitmap_Layout(RGB332, 400, 200);

     LCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 400, 200);

     LCD_Cmd_Begin(BEGIN_BITMAPS);

     LCD_Cmd_Vertex_2II(20, 20, 0, 0);


     // Draw Slider

     LCD_Cmd_Set_Tag(1);

     LCD_Cmd_Set_Foreground_Color(0x10, 0xB0, 0x10);

     LCD_Cmd_Set_Background_Color(0x20, 0xD0, 0x20);

     LCD_Cmd_Color_RGB(0x20, 0xD0, 0x20);

//     File_Select_Sldr.value = Scroll_Counter / 2;

     LCD_Cmd_Draw_Slider(&File_Select_Sldr);

     LCD_Cmd_Display();

     LCD_Cmd_Swap();


     Tag_Value = (int) LCD_Mem_Read32(REG_TOUCH_TAG);

     Touch_X_Value = (int) LCD_Mem_Read32(REG_TOUCH_SCREEN_XY) >> 16;

     Touch_Y_Value = (int) LCD_Mem_Read32(REG_TOUCH_SCREEN_XY) & 0x0000FFFF;



     if(Tag_Value)
     {
          Relative_Touch_Y_Value = Touch_Y_Value - 30;

          if(Relative_Touch_Y_Value < 0)
          {
               Relative_Touch_Y_Value = 0;
          }

          if(Relative_Touch_Y_Value > 180)
          {
               Relative_Touch_Y_Value = 180;
          }
          
          if(!bIsGrabbed && ((Touch_Y_Value - File_Select_Sldr.value) > 10) && ((Touch_Y_Value - File_Select_Sldr.value) < 50))
          {
               bIsGrabbed = 1;
          }
          else
          {
               bIsGrabbed = 0;
          }

          if(bIsGrabbed)
          {
               File_Select_Sldr.value = Relative_Touch_Y_Value;
          }

          

     Serial.println("");
     Serial.print("Tag Value = 0x");
     Serial.println(Tag_Value, HEX);
     Serial.print("X Value = ");
     Serial.println(Touch_X_Value, DEC);
     Serial.print("Y Value = ");
     Serial.println(Touch_Y_Value, DEC);
     Serial.print("Delta Y Value = ");
     Serial.println(Delta_Y_Value, DEC);
     Serial.print("Slider Value = ");
     Serial.println(File_Select_Sldr.value, DEC);

     }
     else
     {
          bIsGrabbed = 0;
     }


}




























