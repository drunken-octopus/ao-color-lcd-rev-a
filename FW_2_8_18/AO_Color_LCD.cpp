/**
 *
 * Functions added to Marlin to drive Aleph Objects Color LCD Touchscreen.
 *
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


 /****************************************************************************
  *                       FUNCTION MAP                                       *
  *                                                                          *
  * SPI and FT800/810 Commands                                               *
  *                                                                          *
  * CLCD_spiSelect()                  Set CS line to 0                       *
  * CLCD_spiDeselect()                Set CS Line to 1                       *
  * CLCD_Reset()                      Toggle FT800/810 Power Down Line 50 ms *
  * CLCD_spiInit()                    Configure I/O Lines for SPI            *
  * CLCD_spiTransfer()                Send/Receive 1 SPI Byte                *
  * CLCD_Init()                       Set FT800/810 Registers                *
  * CLCD_Enable()                     Turn On FT800/810 PCLK                 *
  * CLCD_Disable()                    Turn Off FT8880/810 PCLK               *
  * CLCD_Set_Backlight()              Set LCD Backlight Level                *
  *                                                                          *
  * MEMORY READ FUNCTIONS                                                    *
  *                                                                          *
  * CLCD_Mem_ReadAddr()               Send 32-Bit Address                    *
  * CLCD_Mem_Read8()                  Read 1 Byte                            *
  * CLCD_Mem_Read16()                 Read 2 Bytes                           *
  * CLCD_Mem_Read32()                 Read 4 Bytes                           *
  *                                                                          *
  * MEMORY WRITE FUNCTIONS                                                   *
  *                                                                          *
  * CLCD_Mem_WriteAddr()              Send 24-Bit Address                    *
  * CLCD_Mem_Write8()                 Write 1 Byte                           *
  * CLCD_Mem_Write16()                Write 2 Bytes                          *
  * CLCD_Mem_Write32()                Write 4 Bytes                          *
  *                                                                          *
  * HOST COMMAND FUNCTION                                                    *
  *                                                                          *
  * CLCD_Host_Cmd()                   Send 24-Bit Host Command               *
  *                                                                          *
  * COMMAND BUFFER FUNCTIONS                                                 *
  *                                                                          *
  * CLCD_Cmd_Buf_Reset()              Set Cmd Buffer Pointers to 0           *
  * CLCD_Cmd_FIFO_Start()             Wait for CP finish - Set FIFO Ptr      *
  * CLCD_Cmd()                        Send 32-Bit Value(4 Bytes)CMD Buffer   *
  * CLCD_Cmd_Bulk()                   Send Data Structure with 32-Bit Cmd    *
  * CLCD_Str_Bulk()                   Send Text String in 32-Bit Multiples   *
  * CLCD_Cmd_FIFO_Execute()           Set REG_CMD_WRITE and start CP         *
  *                                                                          *
  * FT800/810 GRAPHIC COMMANDS                                               *
  *                                                                          *
  * CLCD_Cmd_Clear_Color_RGB()        Set Clear Screen Color                 *
  * CLCD_Cmd_Clear()                  Clear Color-Stencil-Tag Buffer(s)      *
  * CLCD_Cmd_Color_RGB()              Set Current Color                      *
  * CLCD_Cmd_Set_Foreground_Color     Set Graphic Item Foreground Color      *
  * CLCD_Cmd_Set_Background_Color     Set Graphic Item Background Color      *
  * CLCD_Cmd_Bitmap_Source()          Set RAM Address of Bitmap Data         *
  * CLCD_Cmd_Bitmap_Layout()          Configure Bitmap Format and Layout     *
  * CLCD_Cmd_Bitmap_Size()            Set Bitmap Dimensions                  *
  * CLCD_Cmd_Bitmap_Handle()          Assign Handle ID to a Bitmap           *
  * CLCD_Cmd_Begin()                  Begin Drawing a Primative              *
  * CLCD_Cmd_Mem_Copy()               Copy a Block of Memory                 *
  * CLCD_Cmd_Append()                 Append Commands to Current DL          *
  * CLCD_Cmd_Gradient_Color()         Set 3D Button Highlight Color          *
  * CLCD_Cmd_Vertex_2II()             Set Coordinates of Graphics Primative  *
  * CLCD_Cmd_Draw_Button()            Draw Button using Button_Struct        *
  * CLCD_Cmd_Draw_Button_Bulk()       Draw Button with Bulk Write            *
  * CLCD_Cmd_Draw_Text()              Draw Text using Text_Struct            *
  * CLCD_Cmd_Draw_Text_Bulk()         Draw Text with Bulk Write              *
  * 
  * 
  * 
  * 
  * 
  * 
  * Class CommandFifo{}               Class to control Cmd FIFO              *
  * 


  */
 
 
 /**************************************************
  * RAM_G Graphics RAM Allocation                  *
  *                                                *
  * Address    Use                                 *
  *                                                *
  *    d900    Thumb Drive Symbol Bitmap           *
  *   d1040    Extruder Bitmap                     *
  *   d1109    Bed Heat Bitmap                     *
  *   d1201    Fan Bitmap                          *
  *  d20000    Static DL Space                     *
  *  d40000    Lulzbot Logo - Green                *
  *  d80000    Lulzbot Logo - Gray - For Wallpaper *
  * d120000    Lulzbot Logo - Overlay Bitmap       *
  * d160000    Lulzbot - Quiver                    *
  * d200000    Splash Sceeen Photo                 *
  * d340000                                        *
  **************************************************/


 #include "MarlinConfig.h"


 
 #include "AO_Color_LCD.h"
 #include "AO_FT810.h"
 #include "AO_Text_Fonts.h"
 
 #include "temperature.h"

 #include "watchdog.h"
 

// #include "AO_Lulzbot_Logos.h"

#ifdef IS_ARDUINO

  #include <SPI.h>

  SPISettings LCDsettings(4000000, MSBFIRST, SPI_MODE0);          // SPI Settings for LCD

#endif



// GRAPHIC ELEMENT STRUCTURES

struct Button_Struct Main_Button, Menu_Button, Stop_Button, USB_Button, Combo_Button;

struct Button_Struct TAZ_Main_Button, TAZ_Menu_Button, TAZ_Stop_Button, TAZ_USB_Button, TAZ_Combo_Button;

struct Button_Struct Extruder_Temp, Bed_Heat, Fan_Percent, Elapsed_Time, Print_Percent;

struct Button_Struct TAZ_Extruder_Temp, TAZ_Bed_Heat, TAZ_Fan_Percent, TAZ_Elapsed_Time, TAZ_Print_Percent;

struct Button_Struct X_Position, Y_Position, Z_Position, Status;

struct Button_Struct TAZ_X_Position, TAZ_Y_Position, TAZ_Z_Position, TAZ_Status;

struct Button_Struct Movement, Temperature, Configuration, SD_Card, About_Firmware;

struct Text_Struct X_Pos_Title, Y_Pos_Title, Z_Pos_Title, Text1, Text2;

struct Text_Struct TAZ_X_Pos_Title, TAZ_Y_Pos_Title, TAZ_Z_Pos_Title;

struct Text_Struct TAZ_Bed_Temp, TAZ_Ext0_Temp, TAZ_Fan_Pct;

struct Clock_Struct Clock1;

struct Progress_Struct PrgBar1;

struct Slider_Struct Sldr1, File_Select_Sldr;
 
 
unsigned char Combo_Button_State = 0;
unsigned char Stop_Button_State = 0;
unsigned char Menu_Button_State = 0;

static bool bOn_Off = 0;
static uint16_t DL_Offset;

uint8_t Device_ID;

uint16_t FIFO_Read_Ptr, FIFO_Write_Ptr;

volatile uint16_t Cmd_FIFO_Ptr = 0;                    // Keeps track of Write location in CMD FIFO - Rolls over to 0 after 4095

volatile uint16_t Cmd_FIFO_Bytes_Written = 0;

static float increment = 1;

// CLASS CommandFifo

class CommandFifo {
  private:
    uint32_t command_write_ptr;

    /* Writes bytes to the fifo. This function will
    /* write the exact numbers of bytes specified,
    /* even if it is not a multiple of four. */
    void _write_unaligned(void *data, uint16_t len) {
      uint16_t bytes_tail, bytes_head;
      uint32_t command_read_ptr;

      /* Wait until there is enough space in the circular buffer for the transfer */
      do {
        command_read_ptr  = CLCD_Mem_Read32(REG_CMD_READ);
        if (command_read_ptr <= command_write_ptr) {
          bytes_tail = 4096U - command_write_ptr;
          bytes_head = command_read_ptr;
        } else {
          bytes_tail = command_read_ptr - command_write_ptr;
          bytes_head = 0;
        }
      } while((bytes_tail + bytes_head) < len);

      /* Write as many bytes as possible following REG_CMD_WRITE */
      uint16_t bytes_to_write = min(len, bytes_tail);
      CLCD_Mem_Write_Bulk (RAM_CMD + command_write_ptr, data, bytes_to_write);
      command_write_ptr += bytes_to_write;
      data += bytes_to_write;
      len  -= bytes_to_write;

      if(len > 0) {
        /* Write remaining bytes at start of circular buffer */
        CLCD_Mem_Write_Bulk (RAM_CMD, data, len);
        command_write_ptr = len;
      }

      if(command_write_ptr == 4096U) {
        command_write_ptr = 0;
      }
    }

  public:
    void reset() {
      CLCD_Mem_Write32(REG_CMD_WRITE, 0x00000000);
      CLCD_Mem_Write32(REG_CMD_READ,  0x00000000);
      command_write_ptr = 0;
    };

    void start() {
      command_write_ptr = CLCD_Mem_Read32(REG_CMD_WRITE);
    }

    // Writes len bytes into the FIFO, if len is not
    // divisible by four, zero bytes will be written
    // to align to the boundary.

    void write(void *data, uint16_t len) {
      const uint8_t padding = MULTIPLE_OF_4(len) - len;

      #if defined(REG_CMDB_SPACE)
        uint16_t Command_Space = CLCD_Mem_Read32(REG_CMDB_SPACE);
        while(Command_Space < len + padding) {
          Command_Space = CLCD_Mem_Read32(REG_CMDB_SPACE);
        }
        CLCD_Mem_Write_Bulk(REG_CMDB_WRITE, data, len, padding);
      #else
        uint8_t pad_bytes[] = {0, 0, 0, 0};
        _write_unaligned(data,      len);
        _write_unaligned(pad_bytes, padding);
      #endif
    }

    void finish() {
      CLCD_Mem_Write32(REG_CMD_WRITE, command_write_ptr);
    }

    bool isIdle() {
      return CLCD_Mem_Read32(REG_CMD_READ) == CLCD_Mem_Read32(REG_CMD_WRITE);
    }

    void waitUntilIdle() {
      do {
      } while(!isIdle());
    }
} cmdFifo;


// SPI and FT800/810 Commands

void CLCD_spiInit (void)
{

#ifdef IS_ARDUINO

  SPI.begin();
  SPI.beginTransaction(LCDsettings);

#else

  SET_OUTPUT(CLCD_MOD_RESET);                                           // CLCD_MOD_RST - Module Reset, not SPI
  WRITE(CLCD_MOD_RESET, 1);
  
  SET_OUTPUT(CLCD_BB_SPI_MOSI);                                         // CLCD_MOSI
  WRITE(CLCD_BB_SPI_MOSI, 1);

  SET_OUTPUT(CLCD_BB_SPI_SCLK);                                         // CLCD_SCLK
  WRITE(CLCD_BB_SPI_SCLK, 0);
    
  SET_OUTPUT(CLCD_BB_SPI_CS);                                           // CLCD_CS
  WRITE(CLCD_BB_SPI_CS, 1);
    
  SET_INPUT_PULLUP(CLCD_BB_SPI_MISO);                                   // CLCD_MISO

  SET_OUTPUT(CLCD_AUX_0);                                               // Test Pulse to Trigger Scope
  WRITE(CLCD_AUX_0, 0);

  delay(50);

#endif
}


void CLCD_spiDeInit (void)
{

#ifdef IS_ARDUINO

  SPI.end();

#else

  SET_INPUT_PULLUP(CLCD_MOD_RESET);                                     // Keep LCD Module Reset High
  
  SET_INPUT(CLCD_BB_SPI_MOSI);                                         // CLCD_MOSI

  SET_INPUT(CLCD_BB_SPI_SCLK);                                         // CLCD_SCLK
    
  SET_INPUT(CLCD_BB_SPI_CS);                                           // CLCD_CS
    
  SET_INPUT_PULLUP(CLCD_BB_SPI_MISO);                                   // CLCD_MISO

  SET_OUTPUT(CLCD_AUX_0);                                               // Test Pulse to Trigger Scope
  WRITE(CLCD_AUX_0, 0);

  delay(50);

#endif
}


void CLCD_spiSelect (void)                                              // CLCD Bitbanged SPI - Chip Select
{
  WRITE(CLCD_BB_SPI_CS, 0);
}
  
  
void CLCD_spiDeselect (void)                                            // CLCD Bitbanged SPI - Chip Deselect
{
  WRITE(CLCD_BB_SPI_CS, 1);
}


void CLCD_Reset (void)
{
  WRITE(CLCD_MOD_RESET, 0);
  delay(50);
  WRITE(CLCD_MOD_RESET, 1);
} 


uint8_t CLCD_spiTransfer (uint8_t spiOutByte)
{
#ifdef IS_ARDUINO

  SPI.transfer(spiOutByte);

#else

  uint8_t spiIndex = 0x80;
    
  uint8_t spiInByte = 0;
    
  uint8_t k;

    
  for(k = 0; k <8; k++)                                                 // Output and Read each bit of spiOutByte and spiInByte
  {
    if(spiOutByte & spiIndex)                                           // Output MOSI Bit
  {
    WRITE(CLCD_BB_SPI_MOSI, 1);
  }
  else
  {
    WRITE(CLCD_BB_SPI_MOSI, 0);
  }
      
  WRITE(CLCD_BB_SPI_SCLK, 1);                                                      // Pulse Clock
      
  WRITE(CLCD_BB_SPI_SCLK, 0);
      
  if(READ(CLCD_BB_SPI_MISO))
  {
    spiInByte |= spiIndex;
  }
      
  spiIndex >>= 1;
  }
  
  return(spiInByte);

#endif
}


void CLCD_Init (void)
{
  CLCD_spiInit();                                  // Set Up I/O Lines for SPI and FT800/810 Control

  delay(50);
  
  CLCD_Reset();                                    // Power Down the FT800/810 for 50 ms

  delay(50);

/*
 *  If driving the 4D Systems 4DLCD-FT843 Board, the following Init sequence is needed for its FT800 Driver
 */

#ifdef IS_FT800                                    // Use External Crystal and 48 MHz System Clock

  CLCD_Host_Cmd(CLKEXT, 0);

  delay(20);
  
  CLCD_Host_Cmd(CLK48M, 0);

#else                                              // Use Internal RC Oscillator and 48 MHz System Clock

  CLCD_Host_Cmd(CLKINT, 0);

  delay(20);

  CLCD_Host_Cmd(0x62, 0x44);

#endif
  
  delay(20);


  CLCD_Host_Cmd(ACTIVE, 0);                        // Activate the System Clock

  delay(50);

  CLCD_Test_Pulse();

  
  Device_ID = CLCD_Mem_Read8(REG_ID);                                   // Read Device ID, Should Be 0x7C;

  delay(400);
  
  CLCD_Mem_Write8(REG_GPIO, 0x00);                                   // Turn OFF Display Enable (GPIO Bit 7);

  CLCD_Mem_Write8(REG_PCLK, 0x00);                                   // Turn OFF LCD PCLK

  CLCD_Set_Backlight(0x00FA, 0);

  /*
   *  Configure the FT800/810 Registers
   */

  CLCD_Mem_Write16(REG_HCYCLE, Hcycle);
  
  CLCD_Mem_Write16(REG_HOFFSET, Hoffset);

  CLCD_Mem_Write16(REG_HSYNC0, Hsync0);

  CLCD_Mem_Write16(REG_HSYNC1, Hsync1);

  CLCD_Mem_Write16(REG_VCYCLE, Vcycle);

  CLCD_Mem_Write16(REG_VOFFSET, Voffset);

  CLCD_Mem_Write16(REG_VSYNC0, Vsync0);

  CLCD_Mem_Write16(REG_VSYNC1, Vsync1);

  CLCD_Mem_Write16(REG_HSIZE, Hsize);

  CLCD_Mem_Write16(REG_VSIZE, Vsize);

  CLCD_Mem_Write8(REG_SWIZZLE, Swizzle);

  CLCD_Mem_Write8(REG_PCLK_POL, Pclkpol);

  CLCD_Mem_Write8(REG_CSPREAD, 1);
  
  #if ENABLED(TAZ)                                                      // If printer is a TAZ, set display to Portrait Mode
  CLCD_Mem_Write8(REG_ROTATE, 2);
  #else
  CLCD_Mem_Write8(REG_ROTATE, 0);
  #endif
 
  CLCD_Mem_Write8(REG_TOUCH_MODE, 0x03);                             // Configure the Touch Screen

  CLCD_Mem_Write8(REG_TOUCH_ADC_MODE, 0x01);

  CLCD_Mem_Write8(REG_TOUCH_OVERSAMPLE, 0x0F);

  CLCD_Mem_Write16(REG_TOUCH_RZTHRESH, 5000);

  CLCD_Mem_Write8(REG_VOL_SOUND, 0x00);                              // Turn Synthesizer Volume Off

  CLCD_Mem_Write8(REG_DLSWAP, 0x02);                                 // Swap on New Frame

  /*
   *  Turn On the Display
   */

  CLCD_Mem_Write8(REG_GPIO_DIR, 0x80);                               // Turn ON Display Enable

  CLCD_Mem_Write8(REG_GPIO, 0x80);


  /*
   *  Set Up the Widget Structures
   */
    
  Make_Button_Structures();
    
  Make_Text_Structures();
    
  Make_Clock_Structures();


  /*
   *  Load Icons into Graphics RAM
   */

  CLCD_Load_Icons();


#if defined(LULZBOT_Quiver_TAZ7_CLCD)

  Make_Logo_Bitmap(1, 0xDC, 40000);                                     // Make Logo in Lulzbot Green at Address d40000
  
  Make_Logo_Bitmap(0, 0x4D, 80000);                                     // Make Gray Logo for Wallpaper
  
  Make_Logo_Overlay_Bitmap(0xCC, 120000);
  
  Make_TAZ7_Overlay_Bitmap(0xCC, 160000);
  
  Make_Splash_Bitmap(200000);
  
  TAZ_Splash_Screen(200000);

#endif


  CLCD_Enable();                                                        // Turns on Clock by setting PCLK Register to 5
  
  delay(50);

  CLCD_Set_Backlight(0x00FA, 128);


// Set Initial Values for Touch Transform Registers

  CLCD_Mem_Write32(REG_TOUCH_TRANSFORM_A, 0x8100);
  CLCD_Mem_Write32(REG_TOUCH_TRANSFORM_B, 0);
  CLCD_Mem_Write32(REG_TOUCH_TRANSFORM_C, 0xFFF18000);
  CLCD_Mem_Write32(REG_TOUCH_TRANSFORM_D, 0);
  CLCD_Mem_Write32(REG_TOUCH_TRANSFORM_E, 0xFFFFB100);
  CLCD_Mem_Write32(REG_TOUCH_TRANSFORM_F, 0x0120D000);
  
  // Calibrate the Touchscreen

  CLCD_Cmd_Calibrate();

  // Turn off Backlight - Play Jingle - Set Main Screen - Delay 50 ms - Turn On Backlight

  CLCD_Set_Backlight(0x00FA, 0);

  delay(500);

  // Test Audio

  CLCD_Mem_Write8(REG_VOL_SOUND, 0xFF);

  CLCD_Mem_Write16(REG_SOUND, (0x37<< 8) | 0x47); // C8 MIDI note on xylophone 37

  CLCD_Mem_Write8(REG_PLAY, 1);

  delay(800);

  CLCD_Mem_Write16(REG_SOUND, (0x40<< 8) | 0x47); // C8 MIDI note on xylophone 40

  CLCD_Mem_Write8(REG_PLAY, 1);

  delay(800);

  CLCD_Mem_Write16(REG_SOUND, (0x3C<< 8) | 0x47); // C8 MIDI note on xylophone 3C

  CLCD_Mem_Write8(REG_PLAY, 1);

  delay(1200);

  CLCD_Mem_Write16(REG_SOUND, 0);

  CLCD_Mem_Write8(REG_PLAY, 1);



  Main_Screen();

  delay(50);

  CLCD_Set_Backlight(0x00FA, 128);

}


void CLCD_Enable (void)
{
  CLCD_Mem_Write8(REG_PCLK, 0x05);
}


void CLCD_Disable (void)
{
  CLCD_Mem_Write8(REG_PCLK, 0x00);
}


void CLCD_Set_Backlight (uint16_t Freq, uint8_t Duty)
{
  CLCD_Mem_Write16(REG_PWM_HZ, Freq);
  CLCD_Mem_Write8(REG_PWM_DUTY, Duty);
}


// MEMORY READ FUNCTIONS

void CLCD_Mem_ReadAddr (uint32_t Reg_Address)                           // Write 4-Byte Address
{
  CLCD_spiTransfer((uint8_t) ((Reg_Address >> 16) & 0x3F));             // Address [21:16]
  CLCD_spiTransfer((uint8_t) ((Reg_Address >> 8 ) & 0xFF));             // Address [15:8]
  CLCD_spiTransfer((uint8_t) ((Reg_Address >> 0)  & 0xFF));             // Address [7:0]
  CLCD_spiTransfer(0x00);                                               // Dummy Byte
}

uint8_t CLCD_Mem_Read8 (uint32_t Reg_Address)                           // Write 4-Byte Address, Read 1-Byte Data
{
  CLCD_spiSelect();
  
  CLCD_Mem_ReadAddr(Reg_Address);
 
  uint8_t R_Data = CLCD_spiTransfer(0x00);                              // Read 1 Byte

  CLCD_spiDeselect();

  return(R_Data);
}


uint16_t CLCD_Mem_Read16 (uint32_t Reg_Address)                         // Write 4-Byte Address, Read 2-Bytes Data
{
  CLCD_spiSelect();
  
  CLCD_Mem_ReadAddr(Reg_Address);
 
  uint16_t R_Data =  (((uint16_t) CLCD_spiTransfer(0x00)) << 0) |          // Read Byte 1
                     (((uint16_t) CLCD_spiTransfer(0x00)) << 8);           // Read Byte 2

  CLCD_spiDeselect();

  return(R_Data);
}


uint32_t CLCD_Mem_Read32 (uint32_t Reg_Address)                         // Write 4-Byte Address, Read 4-Bytes Data
{
  CLCD_spiSelect();
  
  CLCD_Mem_ReadAddr(Reg_Address);
 
  uint32_t R_Data =  (((uint32_t) CLCD_spiTransfer(0x00)) <<  0) |         // Read Byte 1
                     (((uint32_t) CLCD_spiTransfer(0x00)) <<  8) |         // Read Byte 2
                     (((uint32_t) CLCD_spiTransfer(0x00)) << 16) |         // Read Byte 3
                     (((uint32_t) CLCD_spiTransfer(0x00)) << 24);          // Read Byte 4

  CLCD_spiDeselect();

  return(R_Data);
}


// MEMORY WRITE FUNCTIONS

void CLCD_Mem_WriteAddr (uint32_t Reg_Address)                          // Write 3-Byte Address
{
  CLCD_spiTransfer((uint8_t) ((Reg_Address >> 16) | 0x80));          // Address [21:16]
  CLCD_spiTransfer((uint8_t) ((Reg_Address >> 8 ) & 0xFF));          // Address [15:8]
  CLCD_spiTransfer((uint8_t) ((Reg_Address >> 0)  & 0xFF));          // Address [7:0]
}


void CLCD_Mem_Write_Bulk (uint32_t Reg_Address, uint8_t *Data, uint16_t Len, uint8_t Padding = 0) { // Write 3-Byte Address, Multiple Bytes
  CLCD_spiSelect();
  CLCD_Mem_WriteAddr(Reg_Address);

  while(Len--) {
    CLCD_spiTransfer(*Data++);                                     // Write 1 Byte
  }

  while(Padding--) {
    CLCD_spiTransfer(0);                                           // Padding Bytes
  }

  CLCD_spiDeselect();
}


void CLCD_Mem_Write8 (uint32_t Reg_Address, uint8_t W_Data)             // Write 3-Byte Address, Write 1-Byte Data
{
  CLCD_spiSelect();

  CLCD_Mem_WriteAddr(Reg_Address);

  CLCD_spiTransfer(W_Data);                                             // Write 1 Byte

  CLCD_spiDeselect();
}


void CLCD_Mem_Write16 (uint32_t Reg_Address, uint16_t W_Data)           // Write 3-Byte Address, Write 2-Bytes Data
{
  CLCD_spiSelect();

  CLCD_Mem_WriteAddr(Reg_Address);

  CLCD_spiTransfer((uint8_t) (W_Data & 0x00FF));                        // Write Byte 0

  CLCD_spiTransfer((uint8_t) ((W_Data >> 8) & 0x00FF));                 // Write Byte 1

  CLCD_spiDeselect();
}


void CLCD_Mem_Write32 (uint32_t Reg_Address, uint32_t W_Data)           // Write 3-Byte Address, Write 4-Bytes Data
{
  CLCD_spiSelect();

  CLCD_Mem_WriteAddr(Reg_Address);

  CLCD_spiTransfer((uint8_t) (W_Data & 0x000000FF));                    // Write Byte 0

  CLCD_spiTransfer((uint8_t) ((W_Data >> 8) & 0x000000FF));             // Write Byte 1

  CLCD_spiTransfer((uint8_t) ((W_Data >> 16) & 0x000000FF));            // Write Byte 2

  CLCD_spiTransfer((uint8_t) ((W_Data >> 24) & 0x000000FF));            // Write Byte 3

  CLCD_spiDeselect();
}


// LOAD ICONS

void CLCD_Load_Icons (void)
{
  // Load Extruder Bitmap into RAM_G + 8000

  Flash_Write_RGB332_Bitmap((RAM_G + 8000), CLCD_Extruder_Widget, 69);


  // Load Bed Heat Bitmap into RAM_G + 8100

  Flash_Write_RGB332_Bitmap((RAM_G + 8100), CLCD_Bed_Heat_Widget, 92);


  // Load Fan Percent Bitmap into RAM_G + 8300

  Flash_Write_RGB332_Bitmap((RAM_G + 8300), CLCD_Fan_Widget, 128);


  // Load USB Thumb Drive Bitmap into RAM_G + 9000

  Flash_Write_RGB332_Bitmap((RAM_G + 9000), CLCD_TD_Widget, 140);

  
}


// HOST COMMAND FUNCTION

void CLCD_Host_Cmd (unsigned char Host_Command, unsigned char Byte2)    // Sends 24-Bit Host Command to LCD
{
  if(Host_Command != ACTIVE)
  {
    Host_Command |= 0x40;
  }

  CLCD_spiSelect();

  CLCD_spiTransfer(Host_Command);                                       // Write Byte 1

  CLCD_spiTransfer(Byte2);                                              // Write Byte 2

  CLCD_spiTransfer(0x00);                                               // Write Byte 3

  CLCD_spiDeselect();
}


// CO_PROCESSOR COMMANDS

void CLCD_Cmd_Buf_Reset (void) {                                        // Reset the REG_CMD_WRITE and REG_CMD_READ Pointer Registers
  cmdFifo.reset();
}


void CLCD_Cmd_FIFO_Start (void) {                                       // Wait until all commands are sent to DL - Set initial value of Cmd_FIFO_Ptr
  cmdFifo.waitUntilIdle();
  cmdFifo.start();
}


void CLCD_Cmd (uint32_t cmd32) {                                        // Writes a 32-bit (4 Bytes) Value to the Co-processor Command Buffer FIFO
  cmdFifo.write(&cmd32, sizeof(uint32_t));
}


void CLCD_Cmd_Bulk (void* data, uint16_t len) {                      // Writes a data structure - always a multiple of 32 bits - to the Co_Processor FIFO.
                                                                     // Data structure includes the 32-bit Co_Processor command.
  cmdFifo.write(data, len);
}


void CLCD_Str_Bulk (char* data) {
  cmdFifo.write(data, strlen(data)+1);
}


void CLCD_Cmd_FIFO_Execute (void)                                     // Cause Co-Processor to start processing commands by setting REG_CMD_WRITE value.
{
  cmdFifo.finish();
}


bool CLCD_Cmd_Is_Idle() // Check whether command processor is idle
{
  return cmdFifo.isIdle();
}


#if defined(REG_CMDB_SPACE)

void CLCD_Command_Bulk(void *data, uint16_t len)         // Writes bulk data to the co-processor
{
    uint16_t Command_Space = CLCD_Mem_Read32(REG_CMDB_SPACE);
    while(Command_Space < len) {
      Command_Space = CLCD_Mem_Read32(REG_CMDB_SPACE);
    }
    CLCD_Mem_Write_Bulk(REG_CMDB_WRITE, data, len, 0);
}

#else // !defined(REG_CMDB_SPACE)

void CLCD_Command_Bulk(void *data, uint16_t len)                  // Writes bulk data to the co-processor
{
    uint8_t bytes_tail; // Num bytes that can be written after REG_CMD_WRITE
    uint8_t bytes_head; // Num bytes that can be written up to REG_CMD_READ
    uint8_t command_read_ptr, command_write_ptr;
    
    /* Wait until there is enough space in the circular buffer for the transfer */
    do {
        command_read_ptr  = CLCD_Mem_Read32(REG_CMD_READ);
        command_write_ptr = CLCD_Mem_Read32(REG_CMD_WRITE);
        if (command_read_ptr <= command_write_ptr) {
          bytes_tail = 4096U - command_write_ptr;
          bytes_head = command_read_ptr;
        } else {
          bytes_tail = command_read_ptr - command_write_ptr;
          bytes_head = 0;
        }
    } while((bytes_tail + bytes_head) < len);

    /* Write as many bytes as possible following REG_CMD_WRITE */
    uint8_t bytes_to_write = min(len, bytes_tail);
    CLCD_Mem_Write_Bulk (RAM_CMD + command_write_ptr, data, bytes_to_write);
    command_write_ptr += bytes_to_write;
    data += bytes_to_write;
    len -= bytes_to_write;
    
    if(len > 0) {
        /* Write remaing bytes at start of circular buffer */
        CLCD_Mem_Write_Bulk (RAM_CMD, data, len);
        command_write_ptr = len;
    }

    CLCD_Mem_Write32(REG_CMD_WRITE, (command_write_ptr == 4096U) ? 0 : command_write_ptr);
}

#endif // !defined(REG_CMDB_SPACE) 

/*
bool CLCD_Cmd_Is_Idle() // Check whether command processor is idle
{
    return CLCD_Mem_Read32(REG_CMD_READ) == CLCD_Mem_Read32(REG_CMD_WRITE);
}
*/




// FT800/810 GRAPHIC COMMANDS

void CLCD_Cmd_Calibrate (void)
{
  uint32_t Initial_Transform_A = CLCD_Mem_Read32(REG_TOUCH_TRANSFORM_A);
  
  CLCD_Cmd_Buf_Reset();

  delay(200);

  CLCD_Set_Backlight(0x00FA, 128);

  delay(200);

  
  
  CLCD_Cmd_FIFO_Start();
  
  CLCD_Cmd(CMD_DLSTART);

  CLCD_Cmd_Clear_Color_RGB(0x80,0x80,0x80);

  CLCD_Cmd_Clear(1,1,1);
  
  CLCD_Cmd(CMD_CALIBRATE);
  CLCD_Cmd(1);
  CLCD_Cmd(END);
  CLCD_Cmd(DISPLAY);
  CLCD_Cmd(CMD_SWAP);

  CLCD_Cmd_FIFO_Execute();

  cli();

  while(!cmdFifo.isIdle())
  {
//    watchdog_reset();
  }

  sei();
}

void CLCD_Cmd_Clear_Color_RGB (uint8_t Red, uint8_t Grn, uint8_t Blu)   // DL Command - Set Clear Screen Color
{
  uint32_t Value32;

  Value32 = (uint32_t) Red;                                             // Add Color Info to Command
  Value32 <<= 8;
  Value32 |= (uint32_t) Grn;
  Value32 <<= 8;
  Value32 |= ((uint32_t) Blu | CLEAR_COLOR_RGB);

  CLCD_Cmd(Value32);
}


void CLCD_Cmd_Clear (uint8_t Clr, uint8_t Stl, uint8_t Tag)             // DL Command - Clear Color-Stencil-Tag Buffer(s)
{
  uint32_t Value32;

  Value32 = (uint32_t) (Clr & 0x01);                                    // Select Buffers to Clear
  Value32 <<= 1;
  Value32 |= (uint32_t) (Stl & 0x01);
  Value32 <<= 1;
  Value32 |= (((uint32_t) (Tag & 0x01)) | CLEAR);

  CLCD_Cmd(Value32);
}


void CLCD_Cmd_Color_RGB (uint8_t Red, uint8_t Grn, uint8_t Blu)         // DL Command - Set Current Color
{
  uint32_t Value32;

  Value32 = (uint32_t) Red;                                             // Add Color Info to Command
  Value32 <<= 8;
  Value32 |= (uint32_t) Grn;
  Value32 <<= 8;
  Value32 |= ((uint32_t) Blu | COLOR_RGB);

  CLCD_Cmd(Value32);
}


void CLCD_Cmd_Set_Foreground_Color (uint8_t Red, uint8_t Grn, uint8_t Blu)   // Co-Processor Command - Set Foreground Color for Widgets
{
  uint32_t Value32;

  Value32 = (uint32_t) Red;                                             // Add Color Info to Command
  Value32 <<= 8;
  Value32 |= (uint32_t) Grn;
  Value32 <<= 8;
  Value32 |= (uint32_t) Blu;

  CLCD_Cmd(CMD_FGCOLOR);

  CLCD_Cmd(Value32);
}


void CLCD_Cmd_Set_Background_Color (uint8_t Red, uint8_t Grn, uint8_t Blu)   // Co-Processor Command - Set Background Color for Widgets
{
  uint32_t Value32;

  Value32 = (uint32_t) Red;                                          // Add Color Info to Command
  Value32 <<= 8;
  Value32 |= (uint32_t) Grn;
  Value32 <<= 8;
  Value32 |= (uint32_t) Blu;

  CLCD_Cmd(CMD_BGCOLOR);

  CLCD_Cmd(Value32);
}


void CLCD_Cmd_Set_Tag (uint8_t Tag)
{
  unsigned long Value32;

  Value32 = TAG | (unsigned long) Tag;

  CLCD_Cmd(Value32);
}


void CLCD_Cmd_Bitmap_Source (uint32_t RAM_G_Addr)
{
     uint32_t Value32;

     Value32 = RAM_G_Addr & 0x000FFFFF;
     Value32 |= BITMAP_SOURCE;

     CLCD_Cmd(Value32);
}


void CLCD_Cmd_Bitmap_Layout (uint8_t format, uint16_t linestride, uint16_t height)
{
     uint32_t Value32;

     Value32 = (uint32_t) format;
     Value32 <<= 10;
     Value32 |= (uint32_t) linestride;
     Value32 <<= 9;
     Value32 |= (uint32_t) height;
     Value32 |= BITMAP_LAYOUT;

     CLCD_Cmd(Value32);
}


void CLCD_Cmd_Bitmap_Size(uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height)
{
     uint32_t Value32;

     Value32 = (uint32_t) filter;
     Value32 <<= 1;
     Value32 |= (uint32_t) wrapx;
     Value32 <<= 1;
     Value32 |= (uint32_t) wrapy;
     Value32 <<= 1;
     Value32 |= (uint32_t) width;
     Value32 <<= 9;
     Value32 |= (uint32_t) height;
     Value32 |= BITMAP_SIZE;

     CLCD_Cmd(Value32);
}


void CLCD_Cmd_Bitmap_Handle (uint16_t Handle)
{
     uint32_t Value32;

     Value32 = BITMAP_HANDLE | (uint32_t) Handle;

     CLCD_Cmd(Value32);
}


void CLCD_Cmd_Begin (uint32_t Primative)
{
     uint32_t Value32;

     Value32 = BEGIN + Primative;

     CLCD_Cmd(Value32);
}


void CLCD_Cmd_Mem_Copy (uint32_t Dest, uint32_t Src, uint32_t Num_Bytes)
{
  CLCD_Cmd(CMD_MEMCPY);
  
  CLCD_Cmd(Dest);
  
  CLCD_Cmd(Src);
  
  CLCD_Cmd(Num_Bytes);
}


void CLCD_Cmd_Append (uint32_t RAM_G_Start_Address, uint32_t Num_Bytes)
{
  CLCD_Cmd(CMD_APPEND);
  
  CLCD_Cmd(RAM_G_Start_Address);
  
  CLCD_Cmd(Num_Bytes);
}


void CLCD_Cmd_Gradient_Color (uint8_t Red, uint8_t Grn, uint8_t Blu)    // Co-Processor Command - Set Foreground Color for Widgets
{
  uint32_t Value32;

  Value32 = (uint32_t) Red;                                             // Add Color Info to Command
  Value32 <<= 8;
  Value32 |= (uint32_t) Grn;
  Value32 <<= 8;
  Value32 |= (uint32_t) Blu;
 
  CLCD_Cmd(CMD_GRADCOLOR);

  CLCD_Cmd(Value32);
}


void CLCD_Cmd_Vertex_2II (uint16_t X_Coord, uint16_t Y_Coord, uint8_t B_Handle, uint8_t Cell)
{
  uint32_t Value32;

  Value32 |= (uint32_t) X_Coord;
  Value32 <<= 9;
  Value32 |= (uint32_t) Y_Coord;
  Value32 <<= 5;
  Value32 |= (uint32_t) B_Handle;
  Value32 <<= 7;
  Value32 |= Cell;
  Value32 |= VERTEX2II;

  CLCD_Cmd(Value32);
}


void CLCD_Cmd_Draw_Button(struct Button_Struct *Btn)
{
  uint32_t Value32;

  uint16_t Str_Length, Str_Remain;

  uint8_t i;

  char C_Buf[4];

  char* Str_Ptr;
  

  // Set Button Color using CLCD_Cmd_Set_Foreground_Color() Function

  CLCD_Cmd_Set_Foreground_Color(Btn->color_r, Btn->color_g, Btn->color_b);
  

  // Send BUTTON Command, Position, Size, Font and Options

  CLCD_Cmd(CMD_BUTTON);

  Value32 = (uint32_t) Btn->y_pos;
  Value32 <<= 16;
  Value32 |= (uint32_t) Btn->x_pos;
  CLCD_Cmd(Value32);

  Value32 = (uint32_t) Btn->y_size;
  Value32 <<= 16;
  Value32 |= (uint32_t) Btn->x_size;
  CLCD_Cmd(Value32);

  Value32 = (uint32_t) Btn->options;
  Value32 <<= 16;
  Value32 |= (uint32_t) Btn->font;
  CLCD_Cmd(Value32);

  // 4 Character Text String as a 32-Bit Value
    
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

    Value32 = (uint32_t) C_Buf[0];
    Value32 <<= 8;
    Value32 |= (uint32_t) C_Buf[1];
    Value32 <<= 8;
    Value32 |= (uint32_t) C_Buf[2];
    Value32 <<= 8;
    Value32 |= (uint32_t) C_Buf[3];

    CLCD_Cmd(Value32);
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

  Value32 = (uint32_t) C_Buf[3];
  Value32 <<= 8;
  Value32 |= (uint32_t) C_Buf[2];
  Value32 <<= 8;
  Value32 |= (uint32_t) C_Buf[1];
  Value32 <<= 8;
  Value32 |= (uint32_t) C_Buf[0];

  CLCD_Cmd(Value32);
}

  
void CLCD_Cmd_Draw_Button_Bulk(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t *text, int16_t font,  int16_t option)
{
  struct {
    int32_t type = CMD_BUTTON;
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    int16_t font;
    int16_t option;
    } cmd_data;

  cmd_data.x      = x;
  cmd_data.y      = y;
  cmd_data.w      = w;
  cmd_data.h      = h;
  cmd_data.font   = font;
  cmd_data.option = option;

  CLCD_Cmd_Bulk( &cmd_data, sizeof(cmd_data) );
  CLCD_Str_Bulk(text);                                        //CLCD_Str_Bulk(text,strlen(text));
}  


void CLCD_Cmd_Draw_Text(struct Text_Struct *Txt)
{
  uint32_t Value32;

  uint16_t Str_Length, Str_Remain;

  uint8_t i;

  char C_Buf[4];

  char* Str_Ptr;

  // Send TEXT Command, Position, Font and Options

  CLCD_Cmd(CMD_TEXT);

  Value32 = (uint32_t) Txt->y_pos;
  Value32 <<= 16;
  Value32 |= (uint32_t) Txt->x_pos;

  CLCD_Cmd(Value32);

  Value32 = (uint32_t) Txt->options;
  Value32 <<= 16;
  Value32 |= (uint32_t) Txt->font;
     
  CLCD_Cmd(Value32);

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

    Value32 = (uint32_t) C_Buf[0];
    Value32 <<= 8;
    Value32 |= (uint32_t) C_Buf[1];
    Value32 <<= 8;
    Value32 |= (uint32_t) C_Buf[2];
    Value32 <<= 8;
    Value32 |= (uint32_t) C_Buf[3];

    CLCD_Cmd(Value32);
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

  CLCD_Cmd(Value32);
}


void CLCD_Cmd_Draw_Text_Bulk(int16_t x, int16_t y, unsigned char *text, int16_t font,  int16_t options)
{
  struct {
    int32_t type = CMD_TEXT;
    int16_t x;
    int16_t y;
    int16_t font;
    int16_t options;
    } cmd_data;

  cmd_data.x       = x;
  cmd_data.y       = y;
  cmd_data.font    = font;
  cmd_data.options = options;

  CLCD_Cmd_Bulk( &cmd_data, sizeof(cmd_data) );
  CLCD_Str_Bulk(text);                      //CLCD_Str_Bulk(text,strlen(text));
}


// Draws text centered in a box, as if inside a button.
void CLCD_Cmd_Draw_Button_Text_Bulk(int16_t x, int16_t y, int16_t w, int16_t h, unsigned char *text, int16_t font)
{
    CLCD_Cmd_Draw_Text_Bulk(x + w/2, y + h/2, text, font, OPT_CENTER);
}


void CLCD_Jump (uint16_t Dest)
{
  CLCD_Cmd(JUMP | (uint32_t) Dest);
}
  
  

  
  
void TAZ_Info_Screen (uint8_t All)
{
  char S_Buffer[20];
  
  float fVal;
  
  
  fVal = current_position[X_AXIS];
  
  if((fVal > -0.01) && (fVal < 0.01))
  {
    fVal = 0.0;
  }
  
  sprintf(S_Buffer, "%3.1f mm", fVal);
  
  strcpy(TAZ_X_Position.text, S_Buffer);
  
  
  fVal = current_position[Y_AXIS];
  
  if((fVal > -0.01) && (fVal < 0.01))
  {
    fVal = 0.0;
  }
  
  sprintf(S_Buffer, "%3.1f mm", fVal);
  
  strcpy(TAZ_Y_Position.text, S_Buffer);
  
  
  fVal = current_position[Z_AXIS];
  
  if((fVal > -0.01) && (fVal < 0.01))
  {
    fVal = 0.0;
  }
  
  sprintf(S_Buffer, "%3.1f mm", fVal);
  
  strcpy(TAZ_Z_Position.text, S_Buffer);
  
 
  fVal = thermalManager.current_temperature_bed;
  
  if((fVal > -0.01) && (fVal < 0.01))
  {
    fVal = 0.0;
  }
  
  sprintf(S_Buffer, "%3.1f C", fVal);
  
  strcpy(TAZ_Bed_Temp.text, S_Buffer);
  
/*  
  fVal = thermalManager.current_temperature[0];
  
  if((fVal > -0.01) && (fVal < 0.01))
  {
    fVal = 0.0;
  }
  
  sprintf(S_Buffer, "%3.1f C", fVal);
  
  strcpy(TAZ_Ext0_Temp.text, S_Buffer);
*/  
  
	
  // Set Combo Button

  switch(Combo_Button_State)
  {
    case 0:                                                             // PRINT Button - Not Pressed
      Combo_Button.color_r = 0x20;
      Combo_Button.color_g = 0xD0;
      Combo_Button.color_b = 0x20;
      Combo_Button.options = OPT_3D;
      strcpy(Combo_Button.text, "PRINT");
      break;
    case 1:                                                             // PAUSE Button - Not Pressed
      Combo_Button.color_r = 0xD0;
      Combo_Button.color_g = 0x80;
      Combo_Button.color_b = 0x20;
      Combo_Button.options = OPT_3D;
      strcpy(Combo_Button.text, "PAUSE");
      break;
    case 2:                                                             // CONTINUE Button - Not Pressed
      Combo_Button.color_r = 0x20;
      Combo_Button.color_g = 0xD0;
      Combo_Button.color_b = 0x80;
      Combo_Button.options = OPT_3D;
      strcpy(Combo_Button.text, "CONTINUE");
      break;
    case 3:                                                             // PRINT Button - Pressed
      Combo_Button.color_r = 0x20;
      Combo_Button.color_g = 0xD0;
      Combo_Button.color_b = 0x20;
      Combo_Button.options = OPT_FLAT;
      strcpy(Combo_Button.text, "PRINT");
      break;
    case 4:                                                             // PAUSE Button - Pressed
      Combo_Button.color_r = 0xD0;
      Combo_Button.color_g = 0x80;
      Combo_Button.color_b = 0x20;
      Combo_Button.options = OPT_FLAT;
      strcpy(Combo_Button.text, "PAUSE");
      break;
    case 5:                                                             // CONTINUE Button - Pressed
      Combo_Button.color_r = 0x20;
      Combo_Button.color_g = 0xD0;
      Combo_Button.color_b = 0x80;
      Combo_Button.options = OPT_FLAT;
      strcpy(Combo_Button.text, "CONTINUE");
      break;
    default:                                                            // PRINT Button - Not Pressed
      Combo_Button.color_r = 0x20;
      Combo_Button.color_g = 0xD0;
      Combo_Button.color_b = 0x20;
      Combo_Button.options = OPT_3D;
      strcpy(Combo_Button.text, "PRINT");
  }

  switch(Stop_Button_State)
  {
    case 0:                                                             // Stop Button - Not Pressed
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
      Menu_Button.color_b = 0x80;
      Menu_Button.options = OPT_3D;
      strcpy(Menu_Button.text, "MENU");
      break;
    case 1:                                         // MENU Button - Pressed
      Menu_Button.color_r = 0xF0;
      Menu_Button.color_g = 0x20;
      Menu_Button.color_b = 0x80;
      Menu_Button.options = OPT_FLAT;
      strcpy(Menu_Button.text, "MENU");
      break;
    default:                                        // MENU Button - Not Pressed
      Menu_Button.color_r = 0xF0;
      Menu_Button.color_g = 0x20;
      Menu_Button.color_b = 0x80;
      Menu_Button.options = OPT_3D;
      strcpy(Menu_Button.text, "MENU");
  }
  
  
//
// Initialize DL and Display
//

  CLCD_Cmd(CMD_DLSTART);
	  
  CLCD_Cmd_Clear_Color_RGB(0x49, 0x49, 0x50);

  CLCD_Cmd_Clear(1,1,1);


//
// STATIC - Static Section - These are graphic elements that do not change with screen updates.
//


  if(All)
  {

    // Draw Wallpaper
  
    TAZ_Wallpaper(80000);
    
    delayMicroseconds(50);
  
    // Draw Screen Text
  
    CLCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);
  
    CLCD_Cmd_Draw_Text(&TAZ_X_Pos_Title);

    CLCD_Cmd_Draw_Text(&TAZ_Y_Pos_Title);

    CLCD_Cmd_Draw_Text(&TAZ_Z_Pos_Title);
  
  
    // Draw Blank Buttons
  
    // Draw MENU Button

    CLCD_Cmd_Set_Foreground_Color(TAZ_Menu_Button.color_r, TAZ_Menu_Button.color_g, TAZ_Menu_Button.color_b);

    CLCD_Cmd_Draw_Button(&TAZ_Menu_Button);


    // Draw USB Button (An SD Button for TAZ)

    CLCD_Cmd_Set_Foreground_Color(TAZ_USB_Button.color_r, TAZ_USB_Button.color_g, TAZ_USB_Button.color_b);

    CLCD_Cmd_Draw_Button(&TAZ_USB_Button);


    // Draw Extruder Temperature Button as Flat Button for Display Only

    CLCD_Cmd_Set_Foreground_Color(TAZ_Extruder_Temp.color_r, TAZ_Extruder_Temp.color_g, TAZ_Extruder_Temp.color_b);

    CLCD_Cmd_Draw_Button(&TAZ_Extruder_Temp);
    

    // Draw Bed Heat Button as Flat Button for Display Only

    CLCD_Cmd_Set_Foreground_Color(TAZ_Bed_Heat.color_r, TAZ_Bed_Heat.color_g, TAZ_Bed_Heat.color_b);

    CLCD_Cmd_Draw_Button(&TAZ_Bed_Heat);
    


    // Draw Fan Percent Button as Flat Button for Display Only

    CLCD_Cmd_Set_Foreground_Color(TAZ_Fan_Percent.color_r, TAZ_Fan_Percent.color_g, TAZ_Fan_Percent.color_b);

    CLCD_Cmd_Draw_Button(&TAZ_Fan_Percent);


    // Draw Print Percent Button as Flat Button for Display Only

    CLCD_Cmd_Set_Foreground_Color(TAZ_Print_Percent.color_r, TAZ_Print_Percent.color_g, TAZ_Print_Percent.color_b);

    CLCD_Cmd_Draw_Button(&TAZ_Print_Percent);


    // Draw Elapsed Time Button as Flat Button for Display Only

    CLCD_Cmd_Set_Foreground_Color(TAZ_Elapsed_Time.color_r, TAZ_Elapsed_Time.color_g, TAZ_Elapsed_Time.color_b);

    CLCD_Cmd_Draw_Button(&TAZ_Elapsed_Time);
    
    


    
  
  
    // Load USB Thumb Drive Bitmap into RAM_G + 900

    Write_RGB332_Bitmap((RAM_G + 900), CLCD_TD_Widget, 140);


    // Load Extruder Bitmap into RAM_G + 1040

    Write_RGB332_Bitmap((RAM_G + 1040), CLCD_Extruder_Widget, 69);


    // Load Bed Heat Bitmap into RAM_G + 1109

    Write_RGB332_Bitmap((RAM_G + 1109), CLCD_Bed_Heat_Widget,92);


    // Load Fan Percent Bitmap into RAM_G + 1201

    Write_RGB332_Bitmap((RAM_G + 1201), CLCD_Fan_Widget,128);
  
  
    // Draw Extruder Bitmap on Extruder Temperature Button

    CLCD_Cmd_Bitmap_Source(RAM_G + 1040);

    CLCD_Cmd_Bitmap_Layout(L1, 3, 23);

    CLCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 24, 23);

    CLCD_Cmd_Begin(BEGIN_BITMAPS);

    CLCD_Cmd_Vertex_2II(20, 20, 0, 0);


    // Draw Bed Heat Bitmap on Bed Heat Button

    CLCD_Cmd_Bitmap_Source(RAM_G + 1109);

    CLCD_Cmd_Bitmap_Layout(L1, 4, 23);

    CLCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 32, 23);

    CLCD_Cmd_Begin(BEGIN_BITMAPS);

    CLCD_Cmd_Vertex_2II(150, 20, 0, 0);


    // Draw Fan Percent Bitmap on Bed Heat Button

    CLCD_Cmd_Bitmap_Source(RAM_G + 1201);

    CLCD_Cmd_Bitmap_Layout(L1, 4, 32);

    CLCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 32, 32);

    CLCD_Cmd_Begin(BEGIN_BITMAPS);

    CLCD_Cmd_Vertex_2II(23, 64, 0, 0);
    
    
    // Draw Status Button as Flat Button for Display Only

    CLCD_Cmd_Set_Foreground_Color(TAZ_Status.color_r, TAZ_Status.color_g, TAZ_Status.color_b);

    CLCD_Cmd_Draw_Button(&TAZ_Status);
  
  
    // Get the end address of the DL (DL_Offset)
  
    DL_Offset = CLCD_Mem_Read16(REG_CMD_DL);
  
  
    // Copy the DL (RAM_DL) to (RAM_GL + d20000) DL_Offset is number of bytes
  
    CLCD_Cmd_Mem_Copy(20000, RAM_DL, (uint32_t) DL_Offset);
    
    delayMicroseconds(50);
  
  }
 
 
//
// DYNAMIC - Dynamic Section - These elements can change with each screen update and need to be redrawn.
//

  
  // Set the DL with the STATIC graphic elements.
  
  CLCD_Cmd_Append(20000, (uint32_t) DL_Offset);
  
  
  // Draw X Position Button as Flat Button for Display Only

  CLCD_Cmd_Set_Foreground_Color(TAZ_X_Position.color_r, TAZ_X_Position.color_g, TAZ_X_Position.color_b);

  CLCD_Cmd_Draw_Button(&TAZ_X_Position);
  
  // Draw Y Position Button as Flat Button for Display Only

  CLCD_Cmd_Set_Foreground_Color(TAZ_Y_Position.color_r, TAZ_Y_Position.color_g, TAZ_Y_Position.color_b);

  CLCD_Cmd_Draw_Button(&TAZ_Y_Position);


  // Draw Z Position Button as Flat Button for Display Only

  CLCD_Cmd_Set_Foreground_Color(TAZ_Z_Position.color_r, TAZ_Z_Position.color_g, TAZ_Z_Position.color_b);

  CLCD_Cmd_Draw_Button(&TAZ_Z_Position);
  
  
  // Add Bed Temperature Text to Bed Heat Button
  
  CLCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);
  
  CLCD_Cmd_Draw_Text(&TAZ_Bed_Temp);
  
  
  // Add Ext0 Temperature Text to Extruder Temperature Button
  
  CLCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);
  
  CLCD_Cmd_Draw_Text(&TAZ_Ext0_Temp);
  
  
  // Draw Combo (PRINT - PAUSE - CONTINUE) Button

  CLCD_Cmd_Set_Foreground_Color(TAZ_Combo_Button.color_r, TAZ_Combo_Button.color_g, TAZ_Combo_Button.color_b);
  
  CLCD_Cmd_Draw_Button(&TAZ_Combo_Button);


  // Draw STOP Button

  CLCD_Cmd_Set_Foreground_Color(TAZ_Stop_Button.color_r, TAZ_Stop_Button.color_g, TAZ_Stop_Button.color_b);
  
  CLCD_Cmd_Draw_Button(&TAZ_Stop_Button);
      

  CLCD_Cmd(DISPLAY);

  CLCD_Cmd(CMD_SWAP);
}






















void CLCD_Cmd_Draw_Clock (struct Clock_Struct *Clk)
{
  uint32_t Value32;

  // Send CLOCK Command, Position, Radius, Options, Hours, Minutes, Seconds and Milliseconds

  CLCD_Cmd(CMD_CLOCK);

  Value32 = (uint32_t) Clk->y_pos;
  Value32 <<= 16;
  Value32 |= (uint32_t) Clk->x_pos;
  CLCD_Cmd(Value32);

  Value32 = (uint32_t) Clk->options;
  Value32 <<= 16;
  Value32 |= (uint32_t) Clk->radius;
  CLCD_Cmd(Value32);

  Value32 = (uint32_t) Clk->minutes;
  Value32 <<= 16;
  Value32 |= (uint32_t) Clk->hours;
  CLCD_Cmd(Value32);

  Value32 = (uint32_t) Clk->milliseconds;
  Value32 <<= 16;
  Value32 |= (uint32_t) Clk->seconds;

  CLCD_Cmd(Value32);
}


void Logo_Screen (uint32_t Bitmap_Address)                              // Display Logo Bitmap in Landscape Mode
{
  // Initial Display Commands
  
  CLCD_Cmd(CMD_DLSTART);
	  
  CLCD_Cmd_Clear_Color_RGB(0x49, 0x49, 0x50);

  CLCD_Cmd_Clear(1,1,1);

  // Set Drawing Color to White for Full-Brightness Bitmaps

  CLCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);

  CLCD_Cmd_Bitmap_Source(Bitmap_Address);

  CLCD_Cmd_Bitmap_Layout(RGB332, 200, 200);

  CLCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 200, 200);

  CLCD_Cmd_Begin(BEGIN_BITMAPS);

  CLCD_Cmd_Vertex_2II(140, 36, 0, 0);

  CLCD_Cmd(END);

  CLCD_Cmd(DISPLAY);

  CLCD_Cmd(CMD_SWAP);

}


void TAZ_Logo_Screen (uint32_t Bitmap_Address)                          // Display Logo Bitmap in Portrate Mode for TAZ
{
  // Initial Display Commands

  CLCD_Cmd(CMD_DLSTART);
	  
  CLCD_Cmd_Clear_Color_RGB(0x49, 0x49, 0x50);

  CLCD_Cmd_Clear(1,1,1);

  // Set Drawing Color to White for Full-Brightness Bitmaps

  CLCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);

  CLCD_Cmd_Bitmap_Source(Bitmap_Address);

  CLCD_Cmd_Bitmap_Layout(RGB332, 200, 200);

  CLCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 200, 200);

  CLCD_Cmd_Begin(BEGIN_BITMAPS);

  CLCD_Cmd_Vertex_2II(36, 140, 0, 0);

  CLCD_Cmd(END);

  CLCD_Cmd(DISPLAY);

  CLCD_Cmd(CMD_SWAP);

}


void TAZ_Wallpaper (uint32_t Bitmap_Address)                            // This function assumes that the DL and background color have
{                                                                       // been initialized - CMD_DLSTART, Clear Color, Clear(1,1,1)
  CLCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);

  CLCD_Cmd_Bitmap_Source(Bitmap_Address);

  CLCD_Cmd_Bitmap_Layout(RGB332, 200, 200);

  CLCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 200, 200);

  CLCD_Cmd_Begin(BEGIN_BITMAPS);

  CLCD_Cmd_Vertex_2II(36, 40, 0, 0);
  
  CLCD_Cmd_Begin(BEGIN_BITMAPS);

  CLCD_Cmd_Vertex_2II(36, 240, 0, 0);
}


void TAZ_Splash_Screen (uint32_t Bitmap_Address)
{
  // Initial Display Commands

  CLCD_Cmd(CMD_DLSTART);
	  
  CLCD_Cmd_Clear_Color_RGB(0x49, 0x49, 0x50);

  CLCD_Cmd_Clear(1,1,1);

  // Set Drawing Color to White for Full-Brightness Bitmaps

  CLCD_Cmd_Color_RGB(0xFF, 0xFF, 0xFF);

  CLCD_Cmd_Bitmap_Source(Bitmap_Address);

  CLCD_Cmd_Bitmap_Layout(RGB332, 272, 480);

  CLCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 272, 480);

  CLCD_Cmd_Begin(BEGIN_BITMAPS);

  CLCD_Cmd_Vertex_2II(0, 0, 0, 0);
  
  CLCD_Cmd_Bitmap_Source(120000);

  CLCD_Cmd_Bitmap_Layout(ARGB2, 200, 200);

  CLCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 200, 200);

  CLCD_Cmd_Begin(BEGIN_BITMAPS);

  CLCD_Cmd_Vertex_2II(36, 140, 0, 0);
  
  CLCD_Cmd_Bitmap_Source(160000);

  CLCD_Cmd_Bitmap_Layout(ARGB2, 200, 50);

  CLCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 200, 50);

  CLCD_Cmd_Begin(BEGIN_BITMAPS);

  CLCD_Cmd_Vertex_2II(36, 360, 0, 0);

  CLCD_Cmd(END);

  CLCD_Cmd(DISPLAY);

  CLCD_Cmd(CMD_SWAP);
	
}


void Make_Button_Structures (void)
{
  Combo_Button.x_pos = 10;
  Combo_Button.y_pos = 220;
  Combo_Button.x_size = 130;
  Combo_Button.y_size = 40;
  Combo_Button.color_r = 0x20;
  Combo_Button.color_g = 0xD0;
  Combo_Button.color_b = 0x20;
  Combo_Button.font = 28;
  Combo_Button.options = 0;
  strcpy(Combo_Button.text, "PRINT");

  TAZ_Combo_Button.x_pos = 40;
  TAZ_Combo_Button.y_pos = 320;
  TAZ_Combo_Button.x_size = 192;
  TAZ_Combo_Button.y_size = 40;
  TAZ_Combo_Button.color_r = 0x20;
  TAZ_Combo_Button.color_g = 0xD0;
  TAZ_Combo_Button.color_b = 0x20;
  TAZ_Combo_Button.font = 28;
  TAZ_Combo_Button.options = 0;
  strcpy(TAZ_Combo_Button.text, "PRINT");

  Main_Button.x_pos = 197;
  Main_Button.y_pos = 230;
  Main_Button.x_size = 86;
  Main_Button.y_size = 38;
  Main_Button.color_r = 0x60;
  Main_Button.color_g = 0x60;
  Main_Button.color_b = 0xF0;
  Main_Button.font = 28;
  Main_Button.options = 0;
  strcpy(Main_Button.text, "MAIN");

  TAZ_Main_Button.x_pos = 197;
  TAZ_Main_Button.y_pos = 230;
  TAZ_Main_Button.x_size = 86;
  TAZ_Main_Button.y_size = 38;
  TAZ_Main_Button.color_r = 0x60;
  TAZ_Main_Button.color_g = 0x60;
  TAZ_Main_Button.color_b = 0xF0;
  TAZ_Main_Button.font = 28;
  TAZ_Main_Button.options = 0;
  strcpy(TAZ_Main_Button.text, "MAIN");

  Menu_Button.x_pos = 385;
  Menu_Button.y_pos = 220;
  Menu_Button.x_size = 85;
  Menu_Button.y_size = 40;
  Menu_Button.color_r = 0xF0;
  Menu_Button.color_g = 0x60;
  Menu_Button.color_b = 0xB0;
  Menu_Button.font = 28;
  Menu_Button.options = 0;
  strcpy(Menu_Button.text, "MENU");

  TAZ_Menu_Button.x_pos = 24;
  TAZ_Menu_Button.y_pos = 425;
  TAZ_Menu_Button.x_size = 100;
  TAZ_Menu_Button.y_size = 40;
  TAZ_Menu_Button.color_r = 0xF0;
  TAZ_Menu_Button.color_g = 0xA0;
  TAZ_Menu_Button.color_b = 0x60;
  TAZ_Menu_Button.font = 28;
  TAZ_Menu_Button.options = 0;
  strcpy(TAZ_Menu_Button.text, "MENU");

  Stop_Button.x_pos = 150;
  Stop_Button.y_pos = 220;
  Stop_Button.x_size = 130;
  Stop_Button.y_size = 40;
  Stop_Button.color_r = 0xF0;
  Stop_Button.color_g = 0x20;
  Stop_Button.color_b = 0x20;
  Stop_Button.font = 28;
  Stop_Button.options = 0;
  strcpy(Stop_Button.text, "STOP");

  TAZ_Stop_Button.x_pos = 40;
  TAZ_Stop_Button.y_pos = 370;
  TAZ_Stop_Button.x_size = 192;
  TAZ_Stop_Button.y_size = 40;
  TAZ_Stop_Button.color_r = 0xF0;
  TAZ_Stop_Button.color_g = 0x20;
  TAZ_Stop_Button.color_b = 0x20;
  TAZ_Stop_Button.font = 28;
  TAZ_Stop_Button.options = 0;
  strcpy(TAZ_Stop_Button.text, "STOP");

  USB_Button.x_pos = 290;
  USB_Button.y_pos = 220;
  USB_Button.x_size = 85;
  USB_Button.y_size = 40;
  USB_Button.color_r = 0x20;
  USB_Button.color_g = 0x80;
  USB_Button.color_b = 0xD0;
  USB_Button.font = 28;
  USB_Button.options = 0;
  strcpy(USB_Button.text, "");

  TAZ_USB_Button.x_pos = 148;
  TAZ_USB_Button.y_pos = 425;
  TAZ_USB_Button.x_size = 100;
  TAZ_USB_Button.y_size = 40;
  TAZ_USB_Button.color_r = 0x20;
  TAZ_USB_Button.color_g = 0x80;
  TAZ_USB_Button.color_b = 0xD0;
  TAZ_USB_Button.font = 28;
  TAZ_USB_Button.options = 0;
  strcpy(TAZ_USB_Button.text, "SD Card");

  Extruder_Temp.x_pos = 10;
  Extruder_Temp.y_pos = 10;
  Extruder_Temp.x_size = 110;
  Extruder_Temp.y_size = 40;
  Extruder_Temp.color_r = 0xD0;
  Extruder_Temp.color_g = 0xA0;
  Extruder_Temp.color_b = 0x00;
  Extruder_Temp.font = 28;
  Extruder_Temp.options = OPT_FLAT;
  strcpy(Extruder_Temp.text, "");                                       // Text to be added later as a Dynamic operation.

  TAZ_Extruder_Temp.x_pos = 10;
  TAZ_Extruder_Temp.y_pos = 10;
  TAZ_Extruder_Temp.x_size = 115;
  TAZ_Extruder_Temp.y_size = 40;
  TAZ_Extruder_Temp.color_r = 0xD0;
  TAZ_Extruder_Temp.color_g = 0xA0;
  TAZ_Extruder_Temp.color_b = 0x00;
  TAZ_Extruder_Temp.font = 28;
  TAZ_Extruder_Temp.options = OPT_FLAT;
  strcpy(TAZ_Extruder_Temp.text, "");                                   // Text to be added later as a Dynamic operation.

  Bed_Heat.x_pos = 130;
  Bed_Heat.y_pos = 10;
  Bed_Heat.x_size = 110;
  Bed_Heat.y_size = 40;
  Bed_Heat.color_r = 0xD0;
  Bed_Heat.color_g = 0x40;
  Bed_Heat.color_b = 0x00;
  Bed_Heat.font = 28;
  Bed_Heat.options = OPT_FLAT;
  strcpy(Bed_Heat.text, "");                                            // Text to be added later as a Dynamic operation.

  TAZ_Bed_Heat.x_pos = 143;
  TAZ_Bed_Heat.y_pos = 10;
  TAZ_Bed_Heat.x_size = 115;
  TAZ_Bed_Heat.y_size = 40;
  TAZ_Bed_Heat.color_r = 0xD0;
  TAZ_Bed_Heat.color_g = 0x40;
  TAZ_Bed_Heat.color_b = 0x00;
  TAZ_Bed_Heat.font = 28;
  TAZ_Bed_Heat.options = OPT_FLAT;
  strcpy(TAZ_Bed_Heat.text, "");                                        // // Text to be added later as a Dynamic operation.

  Fan_Percent.x_pos = 250;
  Fan_Percent.y_pos = 10;
  Fan_Percent.x_size = 110;
  Fan_Percent.y_size = 40;
  Fan_Percent.color_r = 0x60;
  Fan_Percent.color_g = 0x60;
  Fan_Percent.color_b = 0xD0;
  Fan_Percent.font = 28;
  Fan_Percent.options = OPT_FLAT;
  strcpy(Fan_Percent.text, "");                                         // Text to be added later as a Dynamic operation.

  TAZ_Fan_Percent.x_pos = 10;
  TAZ_Fan_Percent.y_pos = 60;
  TAZ_Fan_Percent.x_size = 115;
  TAZ_Fan_Percent.y_size = 40;
  TAZ_Fan_Percent.color_r = 0x60;
  TAZ_Fan_Percent.color_g = 0x60;
  TAZ_Fan_Percent.color_b = 0xD0;
  TAZ_Fan_Percent.font = 28;
  TAZ_Fan_Percent.options = OPT_FLAT;
  strcpy(TAZ_Fan_Percent.text, "      80 %");                                     // Text to be added later as a Dynamic operation.

  Print_Percent.x_pos = 370;
  Print_Percent.y_pos = 10;
  Print_Percent.x_size = 100;
  Print_Percent.y_size = 25;
  Print_Percent.color_r = 0xE0;
  Print_Percent.color_g = 0xC0;
  Print_Percent.color_b = 0x00;
  Print_Percent.font = 28;
  Print_Percent.options = OPT_FLAT;
  strcpy(Print_Percent.text, "");                                       // Text to be added later as a Dynamic operation.

  TAZ_Print_Percent.x_pos = 143;
  TAZ_Print_Percent.y_pos = 60;
  TAZ_Print_Percent.x_size = 115;
  TAZ_Print_Percent.y_size = 20;
  TAZ_Print_Percent.color_r = 0xE0;
  TAZ_Print_Percent.color_g = 0x20;
  TAZ_Print_Percent.color_b = 0x60;
  TAZ_Print_Percent.font = 28;
  TAZ_Print_Percent.options = OPT_FLAT;
  strcpy(TAZ_Print_Percent.text, "   2%");                                   // Text to be added later as a Dynamic operation.

  Elapsed_Time.x_pos = 370;
  Elapsed_Time.y_pos = 45;
  Elapsed_Time.x_size = 100;
  Elapsed_Time.y_size = 25;
  Elapsed_Time.color_r = 0x40;
  Elapsed_Time.color_g = 0xE0;
  Elapsed_Time.color_b = 0x20;
  Elapsed_Time.font = 28;
  Elapsed_Time.options = OPT_FLAT;
  strcpy(Elapsed_Time.text, "");                                       // Text to be added later as a Dynamic operation.

  TAZ_Elapsed_Time.x_pos = 143;
  TAZ_Elapsed_Time.y_pos = 80;
  TAZ_Elapsed_Time.x_size = 115;
  TAZ_Elapsed_Time.y_size = 20;
  TAZ_Elapsed_Time.color_r = 0x40;
  TAZ_Elapsed_Time.color_g = 0xE0;
  TAZ_Elapsed_Time.color_b = 0x20;
  TAZ_Elapsed_Time.font = 28;
  TAZ_Elapsed_Time.options = OPT_FLAT;
  strcpy(TAZ_Elapsed_Time.text, "  0:00:27");                                    // Text to be added later as a Dynamic operation.

  X_Position.x_pos = 10;
  X_Position.y_pos = 80;
  X_Position.x_size = 146;
  X_Position.y_size = 40;
  X_Position.color_r = 0xA0;
  X_Position.color_g = 0x10;
  X_Position.color_b = 0xD0;
  X_Position.font = 29;
  X_Position.options = OPT_FLAT;
  strcpy(X_Position.text, "");                                          // Text to be added later as a Dynamic operation.

  TAZ_X_Position.x_pos = 100;
  TAZ_X_Position.y_pos = 110;
  TAZ_X_Position.x_size = 158;
  TAZ_X_Position.y_size = 40;
  TAZ_X_Position.color_r = 0xA0;
  TAZ_X_Position.color_g = 0x10;
  TAZ_X_Position.color_b = 0xD0;
  TAZ_X_Position.font = 29;
  TAZ_X_Position.options = OPT_FLAT;
  strcpy(TAZ_X_Position.text, "100.25 mm");                                      // Text to be added later as a Dynamic operation.

  Y_Position.x_pos = 167;
  Y_Position.y_pos = 80;
  Y_Position.x_size = 146;
  Y_Position.y_size = 40;
  Y_Position.color_r = 0x90;
  Y_Position.color_g = 0x10;
  Y_Position.color_b = 0xC0;
  Y_Position.font = 29;
  Y_Position.options = OPT_FLAT;
  strcpy(Y_Position.text, "");                                          // Text to be added later as a Dynamic operation.

  TAZ_Y_Position.x_pos = 100;
  TAZ_Y_Position.y_pos = 160;
  TAZ_Y_Position.x_size = 158;
  TAZ_Y_Position.y_size = 40;
  TAZ_Y_Position.color_r = 0x90;
  TAZ_Y_Position.color_g = 0x10;
  TAZ_Y_Position.color_b = 0xC0;
  TAZ_Y_Position.font = 29;
  TAZ_Y_Position.options = OPT_FLAT;
  strcpy(TAZ_Y_Position.text, " 76.50 mm");                                      // Text to be added later as a Dynamic operation.

  Z_Position.x_pos = 324;
  Z_Position.y_pos = 80;
  Z_Position.x_size = 145;
  Z_Position.y_size = 40;
  Z_Position.color_r = 0x80;
  Z_Position.color_g = 0x10;
  Z_Position.color_b = 0xB0;
  Z_Position.font = 29;
  Z_Position.options = OPT_FLAT;
  strcpy(Z_Position.text, "");                                          // Text to be added later as a Dynamic operation.

  TAZ_Z_Position.x_pos = 100;
  TAZ_Z_Position.y_pos = 210;
  TAZ_Z_Position.x_size = 158;
  TAZ_Z_Position.y_size = 40;
  TAZ_Z_Position.color_r = 0x80;
  TAZ_Z_Position.color_g = 0x10;
  TAZ_Z_Position.color_b = 0xB0;
  TAZ_Z_Position.font = 29;
  TAZ_Z_Position.options = OPT_FLAT;
  strcpy(TAZ_Z_Position.text, "30.05 mm");                                      // Text to be added later as a Dynamic operation.

  Status.x_pos = 10;
  Status.y_pos = 150;
  Status.x_size = 460;
  Status.y_size = 60;
  Status.color_r = 0xC0;
  Status.color_g = 0x40;
  Status.color_b = 0x80;
  Status.font = 28;
  Status.options = OPT_FLAT;
  strcpy(Status.text, "Going To Home Position");

  TAZ_Status.x_pos = 10;
  TAZ_Status.y_pos = 260;
  TAZ_Status.x_size = 252;
  TAZ_Status.y_size = 50;
  TAZ_Status.color_r = 0xC0;
  TAZ_Status.color_g = 0x40;
  TAZ_Status.color_b = 0x80;
  TAZ_Status.font = 29;
  TAZ_Status.options = OPT_FLAT;
  strcpy(TAZ_Status.text, "Going To Home Pos");

  // Menu Buttons

  Movement.x_pos = 140;
  Movement.y_pos = 5;
  Movement.x_size = 200;
  Movement.y_size = 40;
  Movement.color_r = 0xC0;
  Movement.color_g = 0x40;
  Movement.color_b = 0x80;
  Movement.font = 28;
  Movement.options = 0;
  strcpy(Movement.text, "Movement");

  Temperature.x_pos = 140;
  Temperature.y_pos = 50;
  Temperature.x_size = 200;
  Temperature.y_size = 40;
  Temperature.color_r = 0xC0;
  Temperature.color_g = 0x40;
  Temperature.color_b = 0x80;
  Temperature.font = 28;
  Temperature.options = 0;
  strcpy(Temperature.text, "Temperature");

  Configuration.x_pos = 140;
  Configuration.y_pos = 95;
  Configuration.x_size = 200;
  Configuration.y_size = 40;
  Configuration.color_r = 0xC0;
  Configuration.color_g = 0x40;
  Configuration.color_b = 0x80;
  Configuration.font = 28;
  Configuration.options = 0;
  strcpy(Configuration.text, "Configuration");

  SD_Card.x_pos = 140;
  SD_Card.y_pos = 140;
  SD_Card.x_size = 200;
  SD_Card.y_size = 40;
  SD_Card.color_r = 0xC0;
  SD_Card.color_g = 0x40;
  SD_Card.color_b = 0x80;
  SD_Card.font = 28;
  SD_Card.options = 0;
  strcpy(SD_Card.text, "SD Card");

  About_Firmware.x_pos = 140;
  About_Firmware.y_pos = 185;
  About_Firmware.x_size = 200;
  About_Firmware.y_size = 40;
  About_Firmware.color_r = 0xC0;
  About_Firmware.color_g = 0x40;
  About_Firmware.color_b = 0x80;
  About_Firmware.font = 28;
  About_Firmware.options = 0;
  strcpy(About_Firmware.text, "About Firmware");
}


void Make_Text_Structures (void)
{
  X_Pos_Title.x_pos = 70;
  X_Pos_Title.y_pos = 120;
  X_Pos_Title.font = 29;
  X_Pos_Title.options = 0;
  strcpy(X_Pos_Title.text, "X");

  TAZ_X_Pos_Title.x_pos = 20;
  TAZ_X_Pos_Title.y_pos = 115;
  TAZ_X_Pos_Title.font = 29;
  TAZ_X_Pos_Title.options = 0;
  strcpy(TAZ_X_Pos_Title.text, "X POS");

  Y_Pos_Title.x_pos = 240;
  Y_Pos_Title.y_pos = 120;
  Y_Pos_Title.font = 29;
  Y_Pos_Title.options = 0;
  strcpy(Y_Pos_Title.text, "Y");

  TAZ_Y_Pos_Title.x_pos = 20;
  TAZ_Y_Pos_Title.y_pos = 165;
  TAZ_Y_Pos_Title.font = 29;
  TAZ_Y_Pos_Title.options = 0;
  strcpy(TAZ_Y_Pos_Title.text, "Y POS");

  Z_Pos_Title.x_pos = 400;
  Z_Pos_Title.y_pos = 120;
  Z_Pos_Title.font = 29;
  Z_Pos_Title.options = 0;
  strcpy(Z_Pos_Title.text, "Z");

  TAZ_Z_Pos_Title.x_pos = 20;
  TAZ_Z_Pos_Title.y_pos = 215;
  TAZ_Z_Pos_Title.font = 29;
  TAZ_Z_Pos_Title.options = 0;
  strcpy(TAZ_Z_Pos_Title.text, "Z POS");
     
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
  
  TAZ_Bed_Temp.x_pos = 194;
  TAZ_Bed_Temp.y_pos = 18;
  TAZ_Bed_Temp.font = 28;
  TAZ_Bed_Temp.options = 0;
  strcpy(TAZ_Bed_Temp.text, "0.0 C");
  
  TAZ_Ext0_Temp.x_pos = 58;
  TAZ_Ext0_Temp.y_pos = 18;
  TAZ_Ext0_Temp.font = 28;
  TAZ_Ext0_Temp.options = 0;
  strcpy(TAZ_Ext0_Temp.text, "0.0 C");
}


void Make_Clock_Structures (void)
{
  Clock1.x_pos = 240;
  Clock1.y_pos = 136;
  Clock1.radius = 120;
  Clock1.options = 0;
  Clock1.hours = 10;
  Clock1.minutes = 10;
  Clock1.seconds = 28;
  Clock1.milliseconds = 0;
}

#if defined(LULZBOT_Quiver_TAZ7_CLCD)
void Make_Splash_Bitmap (uint32_t Base_Addr)
{
  uint32_t Array_Index;
  
  uint8_t Pixel_Byte;
	
  for(Array_Index = 0; Array_Index < 130560; Array_Index++)
  {
	if((Splash1[Array_Index] == 87) || (Splash1[Array_Index] == 119))   // This cleans up blue blotches in the sky of the photo.
	{
	  Pixel_Byte = 83;
	}
	else
	{
	  Pixel_Byte = Splash1[Array_Index];
	}
	
    CLCD_Mem_Write8(1, (Base_Addr + Array_Index), Pixel_Byte);
  }
}


// Function - Make_Logo_Bitmap  Uses Pattern Array Lulzbot_Logo200_GL in the AO_Lulzbot_Logos.h file to Create a 200x200
//                              Bitmap of a selected color at the selected address in the FT810 Memory.

void Make_Logo_Bitmap (uint8_t Logo_Type, uint8_t Color, uint32_t Base_Addr)
{
  uint16_t Array_Index, Pix_Count;

  uint8_t RGB332_Byte, Pixel_Byte, Pix[4];

  uint32_t Test_Address;

  for(Array_Index = 0; Array_Index < 10000; Array_Index++)
  {
    Pixel_Byte = Lulzbot_Logo200_GL[Array_Index];

    Pix[0] = (Pixel_Byte >> 6) & 0x03;
    Pix[1] = (Pixel_Byte >> 4) & 0x03;
    Pix[2] = (Pixel_Byte >> 4) & 0x03;
    Pix[3] =  Pixel_Byte & 0x03;
    
    if(Logo_Type)
    {
	  for(Pix_Count = 0; Pix_Count < 4; Pix_Count++)   // Logo Color is selectable, other colors are predefined.
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
            RGB332_Byte = Color;
            break;
          case 3:
            RGB332_Byte = COLOR3;
            break;
          default:
            RGB332_Byte = COLOR1;
        }

        CLCD_Mem_Write8(1, (Base_Addr + ((uint32_t) Array_Index * 4) + (uint32_t) Pix_Count), RGB332_Byte);
      }
	}
	else
	{
	  for(Pix_Count = 0; Pix_Count < 4; Pix_Count++)   // Logo Color is selectable, other colors are predefined.
      {
        switch(Pix[Pix_Count])
        {
          case 0:
            RGB332_Byte = 0x49;
            break;
          case 1:
            RGB332_Byte = 0x49;
            break;
          case 2:
            RGB332_Byte = Color;
            break;
          case 3:
            RGB332_Byte = 0x49;
            break;
          default:
            RGB332_Byte = 0x49;
        }

        CLCD_Mem_Write8(1, (Base_Addr + ((uint32_t) Array_Index * 4) + (uint32_t) Pix_Count), RGB332_Byte);
      }		
	}

  }
}


// Function - Make_Logo_Overlay_Bitmap  Uses Pattern Array Lulzbot_Logo200_GL in the AO_Lulzbot_Logos.h file to Create a 200x200
//                                      ARGB2 Bitmap of a selected color and transparents at the selected address in the FT810 Memory.

void Make_Logo_Overlay_Bitmap (uint8_t Color, uint32_t Base_Addr)
{
  uint16_t Array_Index, Pix_Count;

  uint8_t ARGB332_Byte, Pixel_Byte, Pix[4];

  uint32_t Test_Address;

  for(Array_Index = 0; Array_Index < 10000; Array_Index++)
  {
    Pixel_Byte = Lulzbot_Logo200_GL[Array_Index];

    Pix[0] = (Pixel_Byte >> 6) & 0x03;
    Pix[1] = (Pixel_Byte >> 4) & 0x03;
    Pix[2] = (Pixel_Byte >> 4) & 0x03;
    Pix[3] =  Pixel_Byte & 0x03;
    
    for(Pix_Count = 0; Pix_Count < 4; Pix_Count++)                      // Logo Color is selectable, other colors are predefined.
    {
      switch(Pix[Pix_Count])
      {
        case 0:
          ARGB332_Byte = 0x00;                                          // Black (0xC0) or Transparent (0x00)
          break;
        case 1:
          ARGB332_Byte = 0x00;                                          // Transparent
          break;
        case 2:
          ARGB332_Byte = Color;                                         // Selected Color
          break;
        case 3:
          ARGB332_Byte = Color;                                         // Selected Color
          break;
        default:
          ARGB332_Byte = 0x00;                                          // Transparent
      }

        CLCD_Mem_Write8(1, (Base_Addr + ((uint32_t) Array_Index * 4) + (uint32_t) Pix_Count), ARGB332_Byte);
    }


  }
}


// Function - Make_TAZ7_Overlay_Bitmap  Uses Pattern Array Lulzbot_Logo200_GL in the AO_Lulzbot_Logos.h file to Create a 200x200
//                                      ARGB2 Bitmap of a selected color and transparents at the selected address in the FT810 Memory.

void Make_TAZ7_Overlay_Bitmap (uint8_t Color, uint32_t Base_Addr)
{
  uint32_t Array_Index;

  uint32_t Test_Address;

  for(Array_Index = 0; Array_Index < 10000; Array_Index++)
  {
    if(TAZ7_Logo[Array_Index] == 0)
    {
      CLCD_Mem_Write8(1, (Base_Addr + Array_Index), Color);
    }
	else
	{
	  CLCD_Mem_Write8(1, (Base_Addr + Array_Index), 0x00);
	}
  }
}

#endif

void Write_RGB332_Bitmap(uint32_t Mem_Address, const unsigned char* pRGB332_Array, uint16_t Num_Pixels)
{
  uint16_t i;

  for(i = 0; i < Num_Pixels; i++)
  {
    CLCD_Mem_Write8((Mem_Address + i), *(pRGB332_Array + i));
    delayMicroseconds(10);
  }
}


void Flash_Write_RGB332_Bitmap(uint32_t Mem_Address, const unsigned char* pRGB332_Array, uint16_t Num_Pixels)
{
  unsigned int i;

  unsigned char Flash_Byte;

  for(i = 0; i < Num_Pixels; i++)
  {
    Flash_Byte = pgm_read_byte_near(pRGB332_Array + i);

    CLCD_Mem_Write8((Mem_Address + i), Flash_Byte);
  }
}


void Main_Screen (void)
{
    CLCD_Cmd_FIFO_Start();
  
    CLCD_Cmd(CMD_DLSTART);
    
    DL_CACHED_CONTENT_START
    
    CLCD_Cmd_Clear_Color_RGB(0x80, 0x80, 0x80);
    CLCD_Cmd_Clear(1,1,1);

    #define GRID_ROWS 8
    #define GRID_COLS 4
    
    T(0)
    RGB(0xD0,0xA0,0x00) BTN( CELL(1,1), SIZE(1,2), "",  28, OPT_FLAT);
    RGB(0xD0,0x40,0x00) BTN( CELL(2,1), SIZE(1,2), "",  28, OPT_FLAT);
    RGB(0x60,0x60,0xD0) BTN( CELL(3,1), SIZE(1,2), "",  28, OPT_FLAT);
    RGB(0xF0,0x60,0xB0) BTN( CELL(4,1), SIZE(1,1), "",  28, OPT_FLAT);
    RGB(0x40,0xE0,0x20) BTN( CELL(4,2), SIZE(1,1), "",  28, OPT_FLAT);

    #define GRID_COLS 3
    #define MARGIN_B 0
    RGB_X_AXIS          BTN( CELL(1,3), SIZE(1,1), "",  29, OPT_FLAT);
    RGB_Y_AXIS          BTN( CELL(2,3), SIZE(1,1), "",  29, OPT_FLAT);
    RGB_Z_AXIS          BTN( CELL(3,3), SIZE(1,1), "",  29, OPT_FLAT);
   #define MARGIN_B 5
    
    #define MARGIN_T 0


    RGB_Z_AXIS            BTN( CELL(3,3), SIZE(1,1), "",  29, OPT_FLAT);
    RGB(0x80,0x80,0x80)   BTN( CELL(1,4), SIZE(1,1), "X", 29, OPT_FLAT);
    RGB(0x80,0x80,0x80)   BTN( CELL(2,4), SIZE(1,1), "Y", 29, OPT_FLAT);
    RGB(0x80,0x80,0x80)   BTN( CELL(3,4), SIZE(1,1), "Z", 29, OPT_FLAT);
    #define MARGIN_T 5

    #define GRID_COLS 1
    RGB(0xC0,0x40,0x80) BTN( CELL(1,5), SIZE(1,2), "Going To Home Position", 30, OPT_FLAT);

    #define MARGIN_T 15
    
    #define GRID_COLS 4
    T(1) RGB(0x20,0xD0,0x20) BTN( CELL(1,7), SIZE(1,2), "PRINT",  28, OPT_3D);
    T(2) RGB(0xF0,0x20,0x20) BTN( CELL(2,7), SIZE(1,2), "STOP",   28, OPT_3D);
    T(3) RGB(0x20,0x80,0xD0) BTN( CELL(3,7), SIZE(1,2), "",       28, OPT_3D);
    T(4) RGB(0xE0,0xC0,0x00) BTN( CELL(4,7), SIZE(1,2), "MENU",   28, OPT_3D);
    
    // Draw Thumb Drive Bitmap on USB Button

    #define ICON_HEIGHT 20 // Height of the icon (used for centering it vertically on a button)
    #define ICON_OFFSET 30 // How far over icon will be drawn from left size of button
    
    T(3)
    CLCD_Cmd_Bitmap_Source(RAM_G + 9000);
    CLCD_Cmd_Bitmap_Layout(L1, 7, ICON_HEIGHT);
    CLCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 50, ICON_HEIGHT);
    CLCD_Cmd_Begin(BEGIN_BITMAPS);
    CLCD_Cmd_Vertex_2II( CELL_X(3) + ICON_OFFSET, CELL_Y(7) + (CELL_H(2) - ICON_HEIGHT)/2, 0, 0);

    #define MARGIN_T 5
    
    // Draw Extruder Bitmap on Extruder Temperature Button

    #define ICON_HEIGHT 23 // Height of the icon (used for centering it vertically on a button)
    #define ICON_OFFSET 10 // How far over icon will be drawn from left size of button
    
    T(0)
    CLCD_Cmd_Bitmap_Source(RAM_G + 8000);
    CLCD_Cmd_Bitmap_Layout(L1, 3, ICON_HEIGHT);
    CLCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 24, ICON_HEIGHT);
    CLCD_Cmd_Begin(BEGIN_BITMAPS);
    CLCD_Cmd_Vertex_2II( CELL_X(1) + ICON_OFFSET, CELL_Y(1) + (CELL_H(2) - ICON_HEIGHT)/2, 0, 0);

    // Draw Bed Heat Bitmap on Bed Heat Button

    #define ICON_HEIGHT 23 // Height of the icon (used for centering it vertically on a button)
    #define ICON_OFFSET 10 // How far over icon will be drawn from left size of button
    
    T(0)
    CLCD_Cmd_Bitmap_Source(RAM_G + 8100);
    CLCD_Cmd_Bitmap_Layout(L1, 4, ICON_HEIGHT);
    CLCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 32, ICON_HEIGHT);
    CLCD_Cmd_Begin(BEGIN_BITMAPS);
    CLCD_Cmd_Vertex_2II( CELL_X(2) + ICON_OFFSET, CELL_Y(1) + (CELL_H(2) - ICON_HEIGHT)/2, 0, 0);

    // Draw Fan Percent Bitmap on Bed Heat Button

    #define ICON_HEIGHT 32 // Height of the icon (used for centering it vertically on a button)
    #define ICON_OFFSET 10 // How far over icon will be drawn from left size of button
    
    T(0)
    CLCD_Cmd_Bitmap_Source(RAM_G + 8300);
    CLCD_Cmd_Bitmap_Layout(L1, 4, ICON_HEIGHT);
    CLCD_Cmd_Bitmap_Size(NEAREST, BORDER, BORDER, 32, ICON_HEIGHT);
    CLCD_Cmd_Begin(BEGIN_BITMAPS);
    CLCD_Cmd_Vertex_2II( CELL_X(3) + ICON_OFFSET, CELL_Y(1) + (CELL_H(2) - ICON_HEIGHT)/2, 0, 0);

//    CLCD_Cmd(END);

    DL_CACHED_CONTENT_END

    /* Dynamic content, non-cached data follows */
    
    #define GRID_ROWS 8
    #define GRID_COLS 4
    T(0)

    unsigned char b[255];
    sprintf(b, "       %-3d C",    marlin_temperature); BTX( CELL(1,1), SIZE(1,2), b, 28);
    sprintf(b, "       %-3d C",    174               ); BTX( CELL(2,1), SIZE(1,2), b, 28);
    sprintf(b, "         %-3d %%", 86                ); BTX( CELL(3,1), SIZE(1,2), b, 28);
    dtostrf(97.5, 5, 1, b);                             BTX( CELL(4,1), SIZE(1,1), b, 28);
    sprintf(b, " %02d : %02d",     0, 05             ); BTX( CELL(4,2), SIZE(1,1), b, 28);

    #define GRID_COLS 3
    #define MARGIN_B 0
    dtostrf(marlin_x_axis, 5, 1, b); strcat(b, " mm"); BTX( CELL(1,3), SIZE(1,1), b, 29);
    dtostrf(marlin_y_axis, 5, 1, b); strcat(b, " mm"); BTX( CELL(2,3), SIZE(1,1), b, 29);
    dtostrf(marlin_z_axis, 5, 1, b); strcat(b, " mm"); BTX( CELL(3,3), SIZE(1,1), b, 29);
    #define MARGIN_B 5

    CLCD_Cmd(END);
    CLCD_Cmd(DISPLAY);
    CLCD_Cmd(CMD_SWAP);

    CLCD_Cmd_FIFO_Execute();
}


void Menu_Screen (void)
{
    CLCD_Cmd_FIFO_Start();
  
    CLCD_Cmd(CMD_DLSTART);
    
    DL_CACHED_CONTENT_START

    CLCD_Cmd_Clear_Color_RGB(0x80, 0x80, 0x80);
    CLCD_Cmd_Clear(1,1,1);

    #define GRID_ROWS 4
    
    #define GRID_COLS 2
    T(0) RGB(0x20,0x80,0xD0) BTN( CELL(1,1), SIZE(1,1), "Auto Home",        29, OPT_3D);
    T(5) RGB(0x20,0x80,0xD0) BTN( CELL(1,2), SIZE(1,1), "Move Axis",        29, OPT_3D);
    T(0) RGB(0x20,0x80,0xD0) BTN( CELL(1,3), SIZE(1,1), "Disable Steppers", 29, OPT_3D);

    T(0) RGB(0x20,0x80,0xD0) BTN( CELL(2,1), SIZE(1,1), "Temperature",      29, OPT_3D);
    T(20) RGB(0x20,0x80,0xD0) BTN( CELL(2,2), SIZE(1,1), "Calibration",    29, OPT_3D);
    T(0) RGB(0x20,0x80,0xD0) BTN( CELL(2,3), SIZE(1,1), "About Firmware",   29, OPT_3D);

    #define MARGIN_T 15
    #define GRID_COLS 4
    T(1) RGB(0xE0,0xC0,0x00) BTN( CELL(2,4), SIZE(2,1), "Back",          29, OPT_3D);

    DL_CACHED_CONTENT_END
    
    CLCD_Cmd(END);
    CLCD_Cmd(DISPLAY);
    CLCD_Cmd(CMD_SWAP);

    CLCD_Cmd_FIFO_Execute();
}


void Cal_Screen (void)
{
  uint32_t T_Transform_A = CLCD_Mem_Read32(REG_TOUCH_TRANSFORM_A);
  uint32_t T_Transform_B = CLCD_Mem_Read32(REG_TOUCH_TRANSFORM_B);
  uint32_t T_Transform_C = CLCD_Mem_Read32(REG_TOUCH_TRANSFORM_C);
  uint32_t T_Transform_D = CLCD_Mem_Read32(REG_TOUCH_TRANSFORM_D);
  uint32_t T_Transform_E = CLCD_Mem_Read32(REG_TOUCH_TRANSFORM_E);
  uint32_t T_Transform_F = CLCD_Mem_Read32(REG_TOUCH_TRANSFORM_F);

  unsigned char b[20];

  
  
    CLCD_Cmd_FIFO_Start();
  
    CLCD_Cmd(CMD_DLSTART);
    
    DL_CACHED_CONTENT_START

    CLCD_Cmd_Clear_Color_RGB(0x40, 0x40, 0x40);
    CLCD_Cmd_Clear(1,1,1);

    #define MARGIN_T 5

    #define GRID_ROWS 7
    
    #define GRID_COLS 2
    
    T(0) RGB(0x30,0x10,0xD0) BTN( CELL(1,1), SIZE(1,1), "TOUCH TRANSFORM_A", 28, OPT_3D);
    T(0) RGB(0x40,0x10,0xD0) BTN( CELL(1,2), SIZE(1,1), "TOUCH TRANSFORM_B", 28, OPT_3D);
    T(0) RGB(0x50,0x10,0xD0) BTN( CELL(1,3), SIZE(1,1), "TOUCH TRANSFORM_C", 28, OPT_3D);
    T(0) RGB(0x60,0x10,0xD0) BTN( CELL(1,4), SIZE(1,1), "TOUCH TRANSFORM_D", 28, OPT_3D);
    T(0) RGB(0x70,0x10,0xD0) BTN( CELL(1,5), SIZE(1,1), "TOUCH TRANSFORM_E", 28, OPT_3D);
    T(0) RGB(0x80,0x10,0xD0) BTN( CELL(1,6), SIZE(1,1), "TOUCH TRANSFORM_F", 28, OPT_3D);

    T(0) RGB(0x10,0x40,0x10) BTN( CELL(2,1), SIZE(1,1), "", 28, OPT_FLAT);
    T(0) RGB(0x10,0x40,0x10) BTN( CELL(2,2), SIZE(1,1), "", 28, OPT_FLAT);
    T(0) RGB(0x10,0x40,0x10) BTN( CELL(2,3), SIZE(1,1), "", 28, OPT_FLAT);
    T(0) RGB(0x10,0x40,0x10) BTN( CELL(2,4), SIZE(1,1), "", 28, OPT_FLAT);
    T(0) RGB(0x10,0x40,0x10) BTN( CELL(2,5), SIZE(1,1), "", 28, OPT_FLAT);
    T(0) RGB(0x10,0x40,0x10) BTN( CELL(2,6), SIZE(1,1), "", 28, OPT_FLAT);

    sprintf(b, "0x%08lX", T_Transform_A); BTX( CELL(2,1), SIZE(1,1), b, 28);
    sprintf(b, "0x%08lX", T_Transform_B); BTX( CELL(2,2), SIZE(1,1), b, 28);
    sprintf(b, "0x%08lX", T_Transform_C); BTX( CELL(2,3), SIZE(1,1), b, 28);
    sprintf(b, "0x%08lX", T_Transform_D); BTX( CELL(2,4), SIZE(1,1), b, 28);
    sprintf(b, "0x%08lX", T_Transform_E); BTX( CELL(2,5), SIZE(1,1), b, 28);
    sprintf(b, "0x%08lX", T_Transform_F); BTX( CELL(2,6), SIZE(1,1), b, 28);

    #define GRID_COLS 3
    
    T(1) RGB(0xE0,0xC0,0x00) BTN( CELL(2,7), SIZE(1,1), "Back",          29, OPT_3D);


    DL_CACHED_CONTENT_END
    
    CLCD_Cmd(END);
    CLCD_Cmd(DISPLAY);
    CLCD_Cmd(CMD_SWAP);

    CLCD_Cmd_FIFO_Execute();
}

    
void Move_Axis (void)
{
    CLCD_Cmd_FIFO_Start();
  
    CLCD_Cmd(CMD_DLSTART);
    
    DL_CACHED_CONTENT_START
    
    CLCD_Cmd_Clear_Color_RGB(0x80, 0x80, 0x80);
    CLCD_Cmd_Clear(1,1,1);

    #define GRID_ROWS 5
    #define GRID_COLS 8

    T( 0) RGB(0x80,0x80,0x80) BTN( CELL(2,1), SIZE(4,1), "Move Axis",  29, OPT_FLAT);
    
    T( 0) RGB(0x80,0x80,0x80) BTN( CELL(1,2), SIZE(1,1), "X:",  29, OPT_FLAT);
    T( 0) RGB_X_AXIS          BTN( CELL(2,2), SIZE(2,1), "",    28, OPT_FLAT);
    T(14) RGB(0x33,0x33,0x33) BTN( CELL(4,2), SIZE(1,1), "-",   29, OPT_3D);
    T(15) RGB(0x33,0x33,0x33) BTN( CELL(5,2), SIZE(1,1), "+",   29, OPT_3D);
    
    T( 0) RGB(0x80,0x80,0x80) BTN( CELL(1,3), SIZE(1,1), "Y:",  29, OPT_FLAT);
    T( 0) RGB_Y_AXIS          BTN( CELL(2,3), SIZE(2,1), "",    28, OPT_FLAT);
    T(16) RGB(0x33,0x33,0x33) BTN( CELL(4,3), SIZE(1,1), "-",   29, OPT_3D);
    T(17) RGB(0x33,0x33,0x33) BTN( CELL(5,3), SIZE(1,1), "+",   29, OPT_3D);

    T( 0) RGB(0x80,0x80,0x80) BTN( CELL(1,4), SIZE(1,1), "Z:",  29, OPT_FLAT);
    T( 0) RGB_Z_AXIS          BTN( CELL(2,4), SIZE(2,1), "",    28, OPT_FLAT);
    T(18) RGB(0x33,0x33,0x33) BTN( CELL(4,4), SIZE(1,1), "-",   29, OPT_3D);
    T(19) RGB(0x33,0x33,0x33) BTN( CELL(5,4), SIZE(1,1), "+",   29, OPT_3D);

    T( 0) RGB(0x80,0x80,0x80) BTN( CELL(7,1), SIZE(2,1), "Amount", 29, OPT_FLAT);

    T(10) TOGGLE(increment == 0.1 ) BTN( CELL(7,2), SIZE(1,1), "0.1", 29, OPT_3D);
    T(11) TOGGLE(increment == 1.  ) BTN( CELL(8,2), SIZE(1,1), "1",   29, OPT_3D);
    T(12) TOGGLE(increment == 10. ) BTN( CELL(7,3), SIZE(2,1), "10",  29, OPT_3D);
    T(13) TOGGLE(increment == 100.) BTN( CELL(7,4), SIZE(2,1), "100", 29, OPT_3D);

    #define MARGIN_T 15
    #define GRID_COLS 4
    T(4) RGB(0xE0,0xC0,0x00) BTN( CELL(1,5), SIZE(4,1), "Back", 29, OPT_3D);

    DL_CACHED_CONTENT_END

    /* Dynamic content, non-cached data follows */

    #define MARGIN_T  5
    #define GRID_COLS 8
    
    unsigned char b[255];
    dtostrf(marlin_x_axis, 5, 1, b); strcat(b, " mm"); BTX( CELL(2,2), SIZE(2,1), b, 28);
    dtostrf(marlin_y_axis, 5, 1, b); strcat(b, " mm"); BTX( CELL(2,3), SIZE(2,1), b, 28);
    dtostrf(marlin_z_axis, 5, 1, b); strcat(b, " mm"); BTX( CELL(2,4), SIZE(2,1), b, 28);
    
    CLCD_Cmd(END);
    CLCD_Cmd(DISPLAY);
    CLCD_Cmd(CMD_SWAP);

    CLCD_Cmd_FIFO_Execute();
}


/*******************      CLCD UPDATE ROUTINE      ************************/

void Color_LCD_Update (void)
{
  static void (*screen)(void) = Main_Screen;
    static bool pressed = false;
    static uint32_t last_update = 0;

    static uint32_t Millisec;
    static uint32_t Last_Millisec = 0;
    
    const uint32_t tag = CLCD_Mem_Read32(REG_TOUCH_TAG);

//    watchdog_reset();

    Millisec = millis();

    if(Millisec > (Last_Millisec + 50))
    {
      Last_Millisec = Millisec;
    }
    else
    {
      return;
    }

//    CLCD_spiInit();

    CLCD_Test_Pulse();

    if(tag == 0) {
        pressed = false;
    }
    else if(!pressed) {
        pressed = true;
      
        switch(tag) {
          case 1:  screen    = Main_Screen;     break;
          case 4:  screen    = Menu_Screen;     break;
          case 5:  screen    = Move_Axis;       break;
          case 10: increment = 0.1;             break;
          case 11: increment = 1;               break;
          case 12: increment = 10;              break;
          case 13: increment = 100;             break;
          case 14: marlin_x_axis -=  increment; break;
          case 15: marlin_x_axis +=  increment; break;
          case 16: marlin_y_axis -=  increment; break;
          case 17: marlin_y_axis +=  increment; break;
          case 18: marlin_z_axis -=  increment; break;
          case 19: marlin_z_axis +=  increment; break;
          case 20: screen    = Cal_Screen;      break;
        }
        (*screen)();
    }

    if(screen == Main_Screen && (millis() - last_update) > 500) {
        last_update = millis();
        Main_Screen();

        marlin_temperature++;
    }

}


/******************* DISPLAY LIST CACHE MANAGEMENT ************************/

/* The Display List Cache mechanism stores the display list corresponding
 * to a menu into RAM_G so that on subsequent calls drawing the menu does
 * not require as much SPI traffic. The macros DL_CACHED_CONTENT_START
 * and DL_CACHED_CONTENT_END should bracket the portions that need to
 * be cached (dynamic content, such as indicators, should not be cached).
 * 
 */

#ifdef IS_FT800
 
  #define DL_CACHE_START 0x03E000

#else

  #define DL_CACHE_START 0x0FE000

#endif

static uint32_t dl_cache_end = DL_CACHE_START; // Pointer to free memory




/* Copies the DL to RAM_G, then returns the address and size of the
 * stored DL in addr and size */
void Store_DL_To_Cache(uint32_t &addr, uint32_t &size)
{
    while(!CLCD_Cmd_Is_Idle());
  
    size = CLCD_Mem_Read32(REG_CMD_DL);
    addr = dl_cache_end;

    if((addr + size) > (RAM_G + 1024*1024)) {
        // Not enough memory to cache the display list.
        addr = 0;
    } else {
        CLCD_Cmd_Mem_Copy(addr, RAM_DL, size);
        dl_cache_end += size;
    }
}


void Read_Cmd_FIFOs (void)
{
  FIFO_Read_Ptr  = CLCD_Mem_Read16(REG_CMD_READ) & 0x0FFF;
  FIFO_Write_Ptr = CLCD_Mem_Read16(REG_CMD_WRITE) & 0x0FFF;
}





void CLCD_Test_Pulse(void)
{
  WRITE(CLCD_AUX_0, 1);
  delayMicroseconds(10);
  WRITE(CLCD_AUX_0, 0);
}
