/**
 * New Functions - Color_Init_LCD()
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

#ifndef _AO_COLOR_LCD_H
#define _AO_COLOR_LCD_H



// Colors RGB332

#define COLOR0     0x00          // Black
#define COLOR1     0x49          // Gray
#define COLOR2     0x7C          // Yellow Green
#define COLOR3     0xFF          // White
 
// Addresses and Dimensions

#define FILE_LIST_X_DIM                 400
#define FILE_LIST_Y_DIM                 400
#define FILE_LIST_RAM_G_ADDR            RAM_G + 240000

#define LOGO_RAM_G_ADDR                 RAM_G + 280000


//
// Define bitbanged CLCD pins based on type of printer and controller board
// Quiver_TAZ7_CLCD (Archim Controller Board), Hibiscus_Mini2_CLCD (Einsy Controller Board)
// See fastio_1280.h for a table of pins for the Einsy (AVR 1280/2560)
// See Archim2_Pinmap.txt for a table of pins for the Archim (ARM SAM3X8)
//

#if defined(LULZBOT_Quiver_TAZ7_CLCD)                                   // ARCHIM       J20 HEADER   +3.3V Pin-24   GND Pin-23
    #define CLCD_BB_SPI_CS                 99                           // PB6 J20 Pin-9
    #define CLCD_BB_SPI_SCLK               95                           // PB0 J20 Pin-6
    #define CLCD_BB_SPI_MOSI               94                           // PB1 J20 Pin-5
    #define CLCD_BB_SPI_MISO              101                           // PB5 J20 Pin-10
    #define CLCD_MOD_RESET                 93                           // PB2 J20 Pin-8
#endif

#if defined(LULZBOT_Hibiscus_Mini2_CLCD)                                // MINI RAMBO   P1 HEADER  +5V Pin-10  GND Pin-9
    #define CLCD_BB_SPI_SCLK               71                           // PG3 P1 Pin-8
    #define CLCD_BB_SPI_MOSI               85                           // PH7 P1 Pin-7
    #define CLCD_BB_SPI_CS                 70                           // PG4 P1 Pin-6
    #define CLCD_MOD_RESET                 19                           // PD2 P1 Pin-5
    #define CLCD_BB_SPI_MISO               82                           // PD5 P1 Pin-4
    #define CLCD_AUX_0                     18                           // PD3 P1 Pin-3
    #define CLCD_AUX_1                      9                           // PH6 P1 Pin-2
    #define CLCD_AUX_2                     84                           // PH2 P1 Pin-1
#endif

#if defined(LULZBOT_Hibiscus_EinsyMini2_CLCD)                           // EINSY        P1 HEADER  +5V Pin-1  GND Pin-2
    #define CLCD_BB_SPI_SCLK               71                           // PG3 P1 Pin-3
    #define CLCD_BB_SPI_MOSI               85                           // PH7 P1 Pin-4
    #define CLCD_BB_SPI_CS                 70                           // PG4 P1 Pin-5
    #define CLCD_MOD_RESET                 19                           // PD2 P1 Pin-6
    #define CLCD_BB_SPI_MISO               82                           // PD5 P1 Pin-7
    #define CLCD_AUX_0                     18                           // PD3 P1 Pin-8
    #define CLCD_AUX_1                      9                           // PH6 P1 Pin-9
    #define CLCD_AUX_2                     84                           // PH2 P1 Pin-10
#endif


/*************************** CACHED CONTENT MACROS ************************/

#define DL_CACHED_CONTENT_START \
    static uint32_t cached_dl_addr = 0; \
    static uint32_t cached_dl_size = 0; \
    if(cached_dl_addr == 0) {
      
#define DL_CACHED_CONTENT_END \
      Store_DL_To_Cache(cached_dl_addr,cached_dl_size); \
    } else { \
      CLCD_Cmd_Append(cached_dl_addr,cached_dl_size); \
    }





// GRAPHIC ITEM STRUCTURES

struct Button_Struct {
     int x_pos;
     int y_pos;
     int x_size;
     int y_size;
     unsigned char color_r;
     unsigned char color_g;
     unsigned char color_b;
     int font;
     unsigned char widget;
     unsigned int options;
     char text[40];
};

struct Text_Struct {
     int x_pos;
     int y_pos;
     int font;
     unsigned int options;
     char text[20];
};

struct Clock_Struct {
     int x_pos;
     int y_pos;
     int radius;
     unsigned int options;
     unsigned int hours;
     unsigned int minutes;
     unsigned int seconds;
     unsigned int milliseconds;
};

struct Progress_Struct {
     int x_pos;
     int y_pos;
     int width;
     int height;
     unsigned int options;
     unsigned int value;
     unsigned int range;
};

struct Slider_Struct {
     int x_pos;
     int y_pos;
     int width;
     int height;
     unsigned int options;
     unsigned int value;
     unsigned int range;
};



// FUNCTIONS

void CLCD_Test_Pulse (void);

void CLCD_Cmd_Buf_Reset (void);

void CLCD_spiSelect (void);
  
void CLCD_spiDeselect (void);

void CLCD_spiInit (void);

void CLCD_spiDeInit (void);

uint8_t CLCD_spiTransfer(uint8_t);
  
void CLCD_Reset (void);
  
void CLCD_Set_Backlight (uint16_t, uint8_t);
  
void CLCD_Enable (void);
  
void CLCD_Disable (void);
  
void CLCD_Jump (uint16_t);
  
void Make_Button_Structures (void);
  
void Make_Text_Structures (void);
  
void Make_Clock_Structures (void);
  
void Write_RGB332_Bitmap(uint32_t, const unsigned char*, uint16_t);

void Flash_Write_RGB332_Bitmap(uint32_t, const unsigned char*, uint16_t);
  
void CLCD_Init (void);

void CLCD_Mem_ReadAddr (uint32_t);                                    // Write 4-Byte Address
  
uint8_t CLCD_Mem_Read8 (uint32_t);
  
uint16_t CLCD_Mem_Read16 (uint32_t);
  
uint32_t CLCD_Mem_Read32 (uint32_t);

void CLCD_Mem_WriteAddr (uint32_t);

void CLCD_Mem_Write_Bulk (uint32_t, uint8_t *, uint16_t, uint8_t Padding = 0);
  
void CLCD_Mem_Write8 (uint32_t, uint8_t);
  
void CLCD_Mem_Write16 (uint32_t, uint16_t);
  
void CLCD_Mem_Write32 (uint32_t, uint32_t);

void CLCD_Load_Icons (void);
  
void CLCD_Host_Cmd (unsigned char, unsigned char);

void CLCD_Cmd_FIFO_Start (void);

void CLCD_Cmd (uint32_t);

void CLCD_Cmd_Bulk (void*, uint16_t);

void CLCD_Str_Bulk (char*);             // (void*, uint16_t);

void CLCD_Cmd_FIFO_Execute (void);
  
void CLCD_Command_Bulk(void *, uint16_t);

bool CLCD_Cmd_Is_Idle();

void CLCD_Cmd_Calibrate (void);
  
void CLCD_Cmd_Clear_Color_RGB (uint8_t, uint8_t, uint8_t);
  
void CLCD_Cmd_Clear (uint8_t, uint8_t, uint8_t);
  
void CLCD_Cmd_Color_RGB (uint8_t, uint8_t, uint8_t);
  
void CLCD_Cmd_Set_Foreground_Color (uint8_t, uint8_t, uint8_t);
  
void CLCD_Cmd_Set_Background_Color (uint8_t, uint8_t, uint8_t);

void CLCD_Cmd_Set_Tag (uint8_t);
  
void CLCD_Cmd_Gradient_Color (uint8_t, uint8_t, uint8_t);
  
void CLCD_Cmd_Bitmap_Source (uint32_t);
  
void CLCD_Cmd_Bitmap_Layout (uint8_t, uint16_t, uint16_t);
  
void CLCD_Cmd_Bitmap_Size(uint8_t, uint8_t, uint8_t, uint16_t, uint16_t);
  
void CLCD_Cmd_Begin (uint32_t);
  
void CLCD_Cmd_Mem_Copy (uint32_t, uint32_t, uint32_t);
  
void CLCD_Cmd_Append (uint32_t, uint32_t);
  
void CLCD_Cmd_Vertex_2II (uint16_t, uint16_t, uint8_t, uint8_t);

void CLCD_Cmd_Draw_Button(struct Button_Struct*);

void CLCD_Cmd_Draw_Button_Bulk(int16_t, int16_t, int16_t, int16_t, uint8_t *, int16_t,  int16_t);

void CLCD_Cmd_Draw_Text(struct Text_Struct*);

void CLCD_Cmd_Draw_Text_Bulk(int16_t, int16_t, uint8_t *, int16_t,  int16_t);
  
void CLCD_Cmd_Draw_Button_Text_Bulk(int16_t, int16_t, int16_t, int16_t, uint8_t *, int16_t);

void CLCD_Cmd_Draw_Clock (struct Clock_Struct*);

void Main_Screen (void);

void Menu_Screen (void);

void Move_Axis (void);

void Cal_Screen (void);
  
void TAZ_Info_Screen (uint8_t);

void TAZ_Wallpaper (uint32_t);

void Color_LCD_Update (void);
  
uint8_t CLCD_spiTransfer (uint8_t, uint8_t);
  
#if defined(LULZBOT_Quiver_TAZ7_CLCD)
  void Make_Splash_Bitmap(uint32_t);
  void Make_Logo_Bitmap (uint8_t, uint8_t, uint32_t);
  void Make_Logo_Overlay_Bitmap (uint8_t, uint32_t);
  void Make_TAZ7_Overlay_Bitmap (uint8_t, uint32_t);
  void TAZ_Splash_Screen (uint32_t);
  void Logo_Screen (uint32_t);
  void TAZ_Logo_Screen (uint32_t);
#endif

void Store_DL_To_Cache(uint32_t &, uint32_t &);

void Read_Cmd_FIFOs (void);


   

/**************************** GRID LAYOUT MACROS **************************/

#define DISP_W 480       // Display width
#define DISP_H 272       // Display height

#define MARGIN_L         5
#define MARGIN_R         5
#define MARGIN_T         5 
#define MARGIN_B         5   

#define RGB(r,g,b)       CLCD_Cmd_Set_Foreground_Color(r, g, b);
#define GRID_X(x)        ((x)*DISP_W/GRID_COLS)
#define GRID_Y(y)        ((y)*DISP_H/GRID_ROWS)
#define CELL_X(x)        (GRID_X(x-1) + MARGIN_L)
#define CELL_Y(y)        (GRID_Y(y-1) + MARGIN_T)
#define CELL_W(w)        (GRID_X(w)   - MARGIN_L - MARGIN_R)
#define CELL_H(h)        (GRID_Y(h)   - MARGIN_T - MARGIN_B)
#define CELL(x,y)        CELL_X(x), CELL_Y(y)
#define SIZE(w,h)        CELL_W(w), CELL_H(h)
#define BTN              CLCD_Cmd_Draw_Button_Bulk
#define BTX              CLCD_Cmd_Draw_Button_Text_Bulk
#define T(t)             CLCD_Cmd_Set_Tag(t);
#define TOGGLE(val)      if(val) RGB(0xFF, 0x00, 0x00) else RGB(0x80, 0x80, 0x80);

// #define MULTIPLE_OF_4(val) (((val+4)>>2)<<2)
#define MULTIPLE_OF_4(val) ((((val)+3)>>2)<<2)

#define STR4_LEN(l)        ((l+4)>>2)

/***************************************************************************/

/******************************* COLOR SCHEME ******************************/

#define RGB_X_AXIS       RGB(0x80,0x00,0x00)
#define RGB_Y_AXIS       RGB(0x00,0x80,0x00)
#define RGB_Z_AXIS       RGB(0x00,0x00,0x80)

static int   marlin_temperature = 25;
static float marlin_x_axis      = 100;
static float marlin_y_axis      = 50;
static float marlin_z_axis      = 170;



 
// L1 Font Data for Widgets

const unsigned char PROGMEM CLCD_TD_Widget[140] =  {0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFC, 0x00,                    // Thumb Drive Widget
                                                    0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
                                                    0x00, 0x60, 0x00, 0x00, 0x00, 0x03, 0x80,
                                                    0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0xC0, 0x60, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0xC0, 0x60, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0xC0, 0x60, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0xC0, 0x60, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0xC0, 0x60, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0xC0, 0x60, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0xC0, 0x60, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0xC0, 0x60, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0xC0,
                                                    0x00, 0x60, 0x00, 0x00, 0x00, 0x03, 0x80,
                                                    0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
                                                    0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFC, 0x00};

const unsigned char PROGMEM CLCD_Extruder_Widget[69] =     {0x3F, 0xFF, 0xFC,
                                                    0x7F, 0xFF, 0xFE,
                                                    0xC0, 0x00, 0x03,
                                                    0xC0, 0x00, 0x03,
                                                    0xC0, 0x00, 0x03,
                                                    0xC0, 0x00, 0x03,
                                                    0x7F, 0xFF, 0xFE,
                                                    0x3F, 0xFF, 0xFC,
                                                    0x3F, 0xFF, 0xFC,
                                                    0x7F, 0xFF, 0xFE,
                                                    0xC0, 0x00, 0x03,
                                                    0xC0, 0x00, 0x03,
                                                    0xC0, 0x00, 0x03,
                                                    0xC0, 0x00, 0x03,
                                                    0x7F, 0xFF, 0xFE,
                                                    0x7F, 0xFF, 0xFE,
                                                    0x07, 0xFF, 0xE0,
                                                    0x03, 0xFF, 0xC0,
                                                    0x01, 0x81, 0x80,
                                                    0x00, 0xC3, 0x00,
                                                    0x00, 0x66, 0x00,
                                                    0x00, 0x3C, 0x00,
                                                    0x00, 0x3C, 0x00};

const unsigned char PROGMEM CLCD_Bed_Heat_Widget[92] =     {0x01, 0x81, 0x81, 0x80,
                                                    0x01, 0x81, 0x81, 0x80,
                                                    0x00, 0xC0, 0xC0, 0xC0,
                                                    0x00, 0xC0, 0xC0, 0xC0,
                                                    0x00, 0x60, 0x60, 0x60,
                                                    0x00, 0x60, 0x60, 0x60,
                                                    0x00, 0xC0, 0xC0, 0xC0,
                                                    0x00, 0xC0, 0xC0, 0xC0,
                                                    0x01, 0x81, 0x81, 0x80,
                                                    0x01, 0x81, 0x81, 0x80,
                                                    0x03, 0x03, 0x03, 0x00,
                                                    0x03, 0x03, 0x03, 0x00,
                                                    0x06, 0x06, 0x06, 0x00,
                                                    0x06, 0x06, 0x06, 0x00,
                                                    0x03, 0x03, 0x03, 0x00,
                                                    0x03, 0x03, 0x03, 0x00,
                                                    0x01, 0x81, 0x81, 0x80,
                                                    0x01, 0x81, 0x81, 0x80,
                                                    0x00, 0x00, 0x00, 0x00,
                                                    0xFF, 0xFF, 0xFF, 0xFF,
                                                    0xFF, 0xFF, 0xFF, 0xFF,
                                                    0xC0, 0x00, 0x00, 0x03,
                                                    0xFF, 0xFF, 0xFF, 0xFF};

  const unsigned char PROGMEM CLCD_Fan_Widget[128] =     {0xFF, 0xFF, 0xFF, 0xFF,
                                                   0xFF, 0xFF, 0xFF, 0xFF,
                                                   0xF8, 0x00, 0x00, 0x1F,
                                                   0xF0, 0x03, 0xF8, 0x0F,
                                                   0xE0, 0x07, 0xF0, 0x07,
                                                   0xC0, 0x0F, 0xE0, 0x03,
                                                   0xC0, 0x1F, 0xE0, 0x03,
                                                   0xC0, 0x1F, 0xE0, 0x03,
                                                   0xC0, 0x0F, 0xE0, 0x03,
                                                   0xC0, 0x07, 0xE0, 0x03,
                                                   0xC0, 0x03, 0xC0, 0x03,
                                                   0xD0, 0x00, 0x00, 0xC3,
                                                   0xD8, 0x03, 0xC1, 0xE3,
                                                   0xDF, 0xC7, 0xE3, 0xF3,
                                                   0xDF, 0xEF, 0xF7, 0xFB,
                                                   0xDF, 0xEF, 0xF7, 0xFB,
                                                   0xDF, 0xEF, 0xF7, 0xFB,
                                                   0xDF, 0xEF, 0xF7, 0xFB,
                                                   0xCF, 0xC7, 0xE3, 0xFB,
                                                   0xC7, 0x83, 0xC0, 0x1B,
                                                   0xC3, 0x00, 0x00, 0x0B,
                                                   0xC0, 0x03, 0xC0, 0x03,
                                                   0xC0, 0x07, 0xE0, 0x03,
                                                   0xC0, 0x07, 0xF0, 0x03,
                                                   0xC0, 0x07, 0xF8, 0x03,
                                                   0xC0, 0x07, 0xF8, 0x03,
                                                   0xC0, 0x07, 0xF0, 0x03,
                                                   0xE0, 0x0F, 0xE0, 0x07,
                                                   0xF0, 0x1F, 0xC0, 0x0F,
                                                   0xF8, 0x00, 0x00, 0x1F,
                                                   0xFF, 0xFF, 0xFF, 0xFF,
                                                   0xFF, 0xFF, 0xFF, 0xFF};

#endif // _AO_COLOR_LCD_H
