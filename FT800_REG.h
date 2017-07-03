/***************
 * FT800_REG.h *
 ***************/


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


/************************************************************************************************************
 * START ADDRESS   END ADDRESS   SIZE      NAME            DESCRIPTION                                      *
 *                                                                                                          *
 * 0x000000        0x03FFFF     256 kB     RAM_G            Main Graphics RAM                               *
 *                                                                                                          *
 * 0x0C0000        0x0C0003       4  B     ROM_CHIPID       [0:1] 0x800 [1:2] 0x0100  Chip Id, Version ID   *
 *                                                                                                          *
 * 0x0BB23C        0x0FFFFB     275 kB     ROM_FONT         Font table and bitmap                           *
 *                                                                                                          *
 * 0x0FFFFC        0x0FFFFF       4  B     ROM_FONT_ADDR    Font table pointer address                      *
 *                                                                                                          *
 * 0x100000        0x101FFF       8 kB     RAM_DL           Display List RAM                                *
 *                                                                                                          *
 * 0x102000        0x1023FF       1 kB     RAM_PAL          Palette RAM                                     *
 *                                                                                                          *
 * 0x102400        0x10257F     380  B     REG_*            Registers                                       *
 *                                                                                                          *
 * 0x108000        0x108FFF       4 kB     RAM_CMD          Command Buffer                                  *
 *                                                                                                          *
 ************************************************************************************************************/


// MEMORY LOCATIONS

#define RAM_G                 0x000000   // Main Graphics RAM
#define ROM_CHIPID            0x0C0000   // Chip ID ROM
#define ROM_Font              0x0BB23C   // Font ROM
#define ROM_FONT_ADDR         0x0FFFFC   // Font Table Pointer
#define RAM_DL                0x100000   // Display List RAM
#define RAM_PAL               0x102000   // Palette RAM
#define RAM_CMD               0x108000   // Command Buffer

// FT800 REGISTERS AND ADDRESSES

//      REGISTER              ADDRESS       SIZE    RESET VALUE     TYPE     DESCRIPTION

#define REG_ID                0x102400   //    8    0x7C               r     Identification Register, Always 0x7C
#define REG_FRAMES            0x102404   //   32    0x00000000         r     Frame Counter, Since Reset
#define REG_CLOCK             0x102408   //   32    0x00000000         r     Clock cycles, Since Reset
#define REG_FREQUENCY         0x10240C   //   27    0x02DC6C00       r/w     Main Clock Frequency
#define REG_RENDERMODE        0x102410   //    1    0x00             r/w     Rendering Mode: 0 = normal, 1 = single-line
#define REG_SNAPY             0x102414   //    9    0x0000           r/w     Scan Line Select for RENDERMODE 1
#define REG_SNAPSHOT          0x102418   //    1    -                  r     Trigger for RENDERMODE 1
#define REG_CPURESET          0x10241C   //    1    0x00             r/w     RESET for All Engines
#define REG_TAP_CRC           0x102420   //   32    -                  r     Live Video Tap
#define REG_TAP_MASK          0x102424   //   32    0xFFFFFFFF       r/w     Live Video Tap Mask
#define REG_HCYCLE            0x102428   //   10    0x224            r/w     Horizontal Total Cycle Count
#define REG_HOFFSET           0x10242C   //   10    0x02B            r/w     Horizontal Display Start Offset
#define REG_HSIZE             0x102430   //   10    0x1E0            r/w     Horizontal Display Pixel Count
#define REG_HSYNC0            0x102434   //   10    0x000            r/w     Horizontal Sync Fall Offset
#define REG_HSYNC1            0x102438   //   10    0x029            r/w     Horizontal Sync Rise Offset
#define REG_VCYCLE            0x10243C   //   10    0x124            r/w     Vertical Total Cycle Count
#define REG_VOFFSET           0x102440   //   10    0x00C            r/w     Vertical Display Start Offset
#define REG_VSIZE             0x102444   //   10    0x110            r/w     Vertical Display Line Count
#define REG_VSYNC0            0x102448   //   10    0x000            r/w     Vertical Sync Fall Offset
#define REG_VSYNC1            0x10244C   //   10    0x00A            r/w     Vertical Sync Rise Offset
#define REG_DLSWAP            0x102450   //    2    0x00             r/w     Display List Swap Control
#define REG_ROTATE            0x102454   //    1    0x00             r/w     Screen 180 degree rotate
#define REG_OUTBITS           0x102458   //    9    0x1B6            r/w     Output Resolution, 3x3x3 Bits
#define REG_DITHER            0x10245C   //    1    0x01             r/w     Output Dither Enable
#define REG_SWIZZLE           0x102460   //    4    0x00             r/w     Output RGB Swizzle, Pin Change for PCB Routing 
#define REG_CSPREAD           0x102464   //    1    0x01             r/w     Output Clock Spreading Enable
#define REG_PCLK_POL          0x102468   //    1    0x00             r/w     PCLK Polarity: 0 = Rising Edge, 1 = Falling Edge
#define REG_PCLK              0x10246C   //    8    0x00             r/w     PCLK Frequency Divider, 0 = Disable Clock
#define REG_TAG_X             0x102470   //    9    0x000            r/w     Tag Query X Coordinate
#define REG_TAG_Y             0x102474   //    9    0x000            r/w     Tag Query Y Coordinate
#define REG_TAG               0x102478   //    8    0x00               r     Tag Query Result
#define REG_VOL_PB            0x10247C   //    8    0xFF             r/w     Audio Playback Volume
#define REG_VOL_SOUND         0x102480   //    8    0xFF             r/w     Audio Synthesizer Volume
#define REG_SOUND             0x102484   //   16    0x0000           r/w     Audio Sound Effect Select 
#define REG_PLAY              0x102488   //    1    0x00             r/w     Audio Start Effect Playback 
#define REG_GPIO_DIR          0x10248C   //    8    0x80             r/w     GPIO Pin Direction: 0 = Input , 1 = Output
#define REG_GPIO              0x102490   //    8    0x00             r/w     GPIO Pin Values for 0, 1, 7 Drive Strength 2, 3, 4, 5, 6
//      Reserved Addr         0x102494
#define REG_INT_FLAGS         0x102498   //    8    0x00               r     Interrupt Flags, Clear by Reading 
#define REG_INT_EN            0x10249C   //    1    0x00             r/w     Global Interrupt Enable
#define REG_INT_MASK          0x1024A0   //    8    0xFF             r/w     Interrupt Enable Mask
#define REG_PLAYBACK_START    0x1024A4   //   20    0x00000          r/w     Audio Playback RAM Start Address
#define REG_PLAYBACK_LENGTH   0x1024A8   //   20    0x00000          r/w     Audio Playback Sample Length (Bytes)
#define REG_PLAYBACK_READPTR  0x1024AC   //   20    -                  r     Audio Playback Read Pointer
#define REG_PLAYBACK_FREQ     0x1024B0   //   16    0x1F40           r/w     Audio Playback Frequency (Hz)
#define REG_PLAYBACK_FORMAT   0x1024B4   //    2    0x00             r/w     Audio Playback Format
#define REG_PLAYBACK_LOOP     0x1024B8   //    1    0x00             r/w     Audio Playback Loop Enable
#define REG_PLAYBACK_PLAY     0x1024BC   //    1    0x00               r     Audio Start Playback 
#define REG_PWM_HZ            0x1024C0   //   14    0x00FA           r/w     Backlight PWM Frequency (Hz)
#define REG_PWM_DUTY          0x1024C4   //    8    0x80             r/w     Backlight PWM Duty Cycle: 0 = 0%, 128 = 100% 
#define REG_MACRO_0           0x1024C8   //   32    0x00000000       r/w     Display List Macro Command 0
#define REG_MACRO_1           0x1024CC   //   32    0x00000000       r/w     Display List Macro Command 1
//      Reserved Addr         0x1024D0
//      Reserved Addr         0x1024D4
//      Reserved Addr         0x1024D8
//      Reserved Addr         0x1024DC
//      Reserved Addr         0x1024E0
#define REG_CMD_READ          0x1024E4   //   12    0x000            r/w     Command Buffer Read Pointer
#define REG_CMD_WRITE         0x1024E8   //   12    0x000            r/w     Command Buffer Write Pointer
#define REG_CMD_DL            0x1024EC   //   13    0x0000           r/w     Command Display List Offset 
#define REG_TOUCH_MODE        0x1024F0   //    2    0x03             r/w     Touch-Screen Sampling Mode
#define REG_TOUCH_ADC_MODE    0x1024F4   //    1    0x01             r/w     Select Single Ended or Differential Sampling
#define REG_TOUCH_CHARGE      0x1024F8   //   16    0x1770           r/w     Touch Screen Charge Time, n x 6 Clocks
#define REG_TOUCH_SETTLE      0x1024FC   //    4    0x03             r/w     Touch-Screen Settle Time, n x 6 Clocks
#define REG_TOUCH_OVERSAMPLE  0x102500   //    4    0x07             r/w     Touch-Screen Oversample Factor
#define REG_TOUCH_RZTHRESH    0x102504   //   16    0xFFFF           r/w     Touch-Screen Resistance Threshold
#define REG_TOUCH_RAW_XY      0x102508   //   32    -                  r     Touch-Screen Raw (x-MSB16; y-LSB16)
#define REG_TOUCH_RZ          0x10250C   //   16    -                  r     Touch-Screen Resistance
#define REG_TOUCH_SCREEN_XY   0x102510   //   32    -                  r     Touch-Screen Screen (x-MSB16; y-LSB16)
#define REG_TOUCH_TAG_XY      0x102514   //   32    -                  r     Touch-Screen Tag Lookup (x-MSB16; y-LSB16)
#define REG_TOUCH_TAG         0x102518   //    8    -                  r     Touch-Screen Tag Result
#define REG_TOUCH_TRANSFORM_A 0x10251C   //   32    0x00010000       r/w     Touch-Screen Transform Coefficient A (s15.16)
#define REG_TOUCH_TRANSFORM_B 0x102520   //   32    0x00000000       r/w     Touch-Screen Transform Coefficient B (s15.16)
#define REG_TOUCH_TRANSFORM_C 0x102524   //   32    0x00000000       r/w     Touch-Screen Transform Coefficient C (s15.16)
#define REG_TOUCH_TRANSFORM_D 0x102528   //   32    0x00000000       r/w     Touch-Screen Transform Coefficient D (s15.16)
#define REG_TOUCH_TRANSFORM_E 0x10252C   //   32    0x00010000       r/w     Touch-Screen Transform Coefficient E (s15.16)
#define REG_TOUCH_TRANSFORM_F 0x102530   //   32    0x00000000       r/w     Touch-Screen Transform Coefficient F (s15.16)
//      Reserved Addresses    0x102434 - 0x102470
#define REG_TOUCH_DIRECT_XY   0x102574   //   32    -                  r     Touch-Screen Direct Conversions XY (x-MSB16; y-LSB16)
#define REG_TOUCH_DIRECT_Z    0x102578   //   32    -                  r     Touch-Screen Direct Conversions Z (z1-MSB16; z2-LSB16)
#define REG_TRACKER           0x109000   //   32    0x00000000       r/w     Track Register (Track Value – MSB16; Tag Value - LSB8)


// OPTIONS

#define OPT_3D              0
#define OPT_RGB565          0
#define OPT_MONO            1
#define OPT_NODL            2
#define OPT_FLAT          256
#define OPT_SIGNED        256
#define OPT_CENTERX       512
#define OPT_CENTERY      1024
#define OPT_CENTER       1536
#define OPT_RIGHTX       2048
#define OPT_NOBACK       4096
#define OPT_NOTICKS      8192
#define OPT_NOHM        16384
#define OPT_NOPOINTER   16384
#define OPT_NOSECS      32768
#define OPT_NOHANDS     49152



// GLOBAl LCD REGISTER SET VALUES FOR WQVGA 480x272 DISPLAY

const int Vsync0  =    0;
const int Vsync1  =   10;
const int Voffset =   12;
const int Vcycle  =  292;
const int Hsync0  =    0;
const int Hsync1  =   41;
const int Hoffset =   43;
const int Hcycle  =  548;
const int Hsize   =  480;
const int Vsize   =  272;
const int Pclkpol =    1;
const int Swizzle =    0;
const int Pclk    =    5;

