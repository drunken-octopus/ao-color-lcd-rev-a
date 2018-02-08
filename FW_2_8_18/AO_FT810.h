/***********
 * FT810.h *
 ***********/


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

/****************************************************************************
 * This header defines registers and commands for the FTDI FT810 LCD Driver *
 * chip.                                                                    *
 ****************************************************************************/


/************************************************************************************************************
 * FT810                                                                                                    *
 *                                                                                                          *
 * START ADDRESS   END ADDRESS   SIZE      NAME            DESCRIPTION                                      *
 *                                                                                                          *
 * 0x000000        0x0FFFFF    1024 kB     RAM_G            Main Graphics RAM    0 to 1048572               *
 *                                                                                                          *
 * 0x0C0000        0x0C0003       4  B     ROM_CHIPID       [0:1] 0x800 [1:2] 0x0100  Chip Id, Version ID   *
 *                                                                                                          *
 * 0x1E0000        0x2FFFFB    1152 kB     ROM_FONT         Font table and bitmap                           *
 *                                                                                                          *
 * 0x2FFFFC        0x2FFFFF       4  B     ROM_FONT_ADDR    Font table pointer address                      *
 *                                                                                                          *
 * 0x300000        0x301FFF       8 kB     RAM_DL           Display List RAM                                *
 *                                                                                                          *
 * 0x302000        0x302FFF       4 kB     REG_*            Registers                                       *
 *                                                                                                          *
 * 0x308000        0x308FFF       4 kB     RAM_CMD          Command Buffer                                  *
 *                                                                                                          *
 ************************************************************************************************************/

#ifndef _AO_FT810_H
#define _AO_FT810_H

// Uncomment this line if using 4D Systems 4DLCD-FT843 Display with FT800 Driver.

//#define IS_FT800


// Uncomment this line if using an Arduino board instead of a Printer Controller board.
// This allows the use of the Arduino SPI Library and SPI Peripheral instead of the bit-banged SPI
// of the Printer Controllers (RAMBo, Mini RAMBo, Einsy, Archim).
// This is used by the CLCD_spiTransfer(uint8_t) function and CLCD_spiInit() function.

// #define IS_ARDUINO

#ifdef IS_FT800
// MEMORY LOCATIONS           FT800
#define RAM_G                 0x000000   // Main Graphics RAM     256KB 0x000000 to 0x03FFFF    DL_CACHE at 0x03E000
#define ROM_FONT              0x0BB23C   // Font ROM
#define ROM_FONT_ADDR         0x0FFFFC   // Font Table Pointer
#define RAM_DL                0x100000   // Display List RAM        8KB 0x100000 to 0x101FFF
#define RAM_PAL               0x102000   // Palette RAM
#define RAM_REG               0x102400   // Registers
#define RAM_CMD               0x108000   // Command Buffer

#else

// MEMORY LOCATIONS           FT810 
#define RAM_G                 0x000000   // Main Graphics RAM    1024KB 0x000000 to 0x0FFFFF    DL_CACHE at 0x0FE000
#define ROM_CHIPID            0x0C0000   // Chip ID ROM
#define ROM_FONT              0x1E0000   // Font ROM
#define ROM_FONT_ADDR         0x2FFFFC   // Font Table Pointer
#define RAM_DL                0x300000   // Display List RAM        8KB 0x300000 to 0x301FFF
#define RAM_REG               0x302000   // Registers
#define RAM_CMD               0x308000   // Command Buffer
#endif


#ifdef IS_FT800
// REGISTERS AND ADDRESSES    FT800

//      REGISTER              ADDRESS       SIZE    RESET VALUE     TYPE     DESCRIPTION

#define REG_ID                0x102400   //    8    0x7C               r     Identification Register, Always 0x7C
#define REG_FRAMES            0x102404   //   32    0x00000000         r     Frame Counter, Since Reset
#define REG_CLOCK             0x102408   //   32    0x00000000         r     Clock cycles, Since Reset
#define REG_FREQUENCY         0x10240C   //   28    0x03938700       r/w     Main Clock Frequency
#define REG_RENDERMODE        0x102410   //    1    0x00             r/w     Rendering Mode: 0 = normal, 1 = single-line
#define REG_SNAPY             0x102414   //   11    0x0000           r/w     Scan Line Select for RENDERMODE 1
#define REG_SNAPSHOT          0x102418   //    1    -                  r     Trigger for RENDERMODE 1
#define REG_CPURESET          0x10241C   //    3    0x02             r/w     RESET Bit2 Audio - Bit1 Touch - Bit0 Graphics
#define REG_TAP_CRC           0x102420   //   32    -                  r     Live Video Tap
#define REG_TAP_MASK          0x102424   //   32    0xFFFFFFFF       r/w     Live Video Tap Mask
#define REG_HCYCLE            0x102428   //   12    0x224            r/w     Horizontal Total Cycle Count
#define REG_HOFFSET           0x10242C   //   12    0x02B            r/w     Horizontal Display Start Offset
#define REG_HSIZE             0x102430   //   12    0x1E0            r/w     Horizontal Display Pixel Count
#define REG_HSYNC0            0x102434   //   12    0x000            r/w     Horizontal Sync Fall Offset
#define REG_HSYNC1            0x102438   //   12    0x029            r/w     Horizontal Sync Rise Offset
#define REG_VCYCLE            0x10243C   //   12    0x124            r/w     Vertical Total Cycle Count
#define REG_VOFFSET           0x102440   //   12    0x00C            r/w     Vertical Display Start Offset
#define REG_VSIZE             0x102444   //   12    0x110            r/w     Vertical Display Line Count
#define REG_VSYNC0            0x102448   //   10    0x000            r/w     Vertical Sync Fall Offset
#define REG_VSYNC1            0x10244C   //   10    0x00A            r/w     Vertical Sync Rise Offset
#define REG_DLSWAP            0x102450   //    2    0x00             r/w     Display List Swap Control
#define REG_ROTATE            0x102454   //    3    0x00             r/w     Screen 90,180, 270 degree rotate
#define REG_OUTBITS           0x102458   //    9    0x1B6            r/w     Output Resolution, 3x3x3 Bits
#define REG_DITHER            0x10245C   //    1    0x01             r/w     Output Dither Enable
#define REG_SWIZZLE           0x102460   //    4    0x00             r/w     Output RGB Swizzle, Pin Change for PCB Routing 
#define REG_CSPREAD           0x102464   //    1    0x01             r/w     Output Clock Spreading Enable
#define REG_PCLK_POL          0x102468   //    1    0x00             r/w     PCLK Polarity: 0 = Rising Edge, 1 = Falling Edge
#define REG_PCLK              0x10246C   //    8    0x00             r/w     PCLK Frequency Divider, 0 = Disable Clock
#define REG_TAG_X             0x102470   //   11    0x000            r/w     Tag Query X Coordinate
#define REG_TAG_Y             0x102474   //   11    0x000            r/w     Tag Query Y Coordinate
#define REG_TAG               0x102478   //    8    0x00               r     Tag Query Result
#define REG_VOL_PB            0x10247C   //    8    0xFF             r/w     Audio Playback Volume
#define REG_VOL_SOUND         0x102480   //    8    0xFF             r/w     Audio Synthesizer Volume
#define REG_SOUND             0x102484   //   16    0x0000           r/w     Audio Sound Effect Select 
#define REG_PLAY              0x102488   //    1    0x00             r/w     Audio Start Effect Playback 
#define REG_GPIO_DIR          0x10248C   //    8    0x80             r/w     GPIO Pin Direction: 0 = Input , 1 = Output
#define REG_GPIO              0x102490   //    8    0x00             r/w     GPIO Pin Values for 0, 1, 7 Drive Strength 2, 3, 4, 5, 6
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
#define REG_TOUCH_TAG_XY      0x102514   //   32    -                  r     Touch-Screen Tag 0 Lookup (x-MSB16; y-LSB16)
#define REG_TOUCH_TAG         0x102518   //    8    -                  r     Touch-Screen Tag 0 Result
#define REG_TOUCH_TRANSFORM_A 0x10251C   //   32    0x00010000       r/w     Touch-Screen Transform Coefficient A (s15.16)
#define REG_TOUCH_TRANSFORM_B 0x102520   //   32    0x00000000       r/w     Touch-Screen Transform Coefficient B (s15.16)
#define REG_TOUCH_TRANSFORM_C 0x102524   //   32    0x00000000       r/w     Touch-Screen Transform Coefficient C (s15.16)
#define REG_TOUCH_TRANSFORM_D 0x102528   //   32    0x00000000       r/w     Touch-Screen Transform Coefficient D (s15.16)
#define REG_TOUCH_TRANSFORM_E 0x10252C   //   32    0x00010000       r/w     Touch-Screen Transform Coefficient E (s15.16)
#define REG_TOUCH_TRANSFORM_F 0x102530   //   32    0x00000000       r/w     Touch-Screen Transform Coefficient F (s15.16)
//      Reserved Addresses    0x102434 - 0x102470
#define REG_TOUCH_DIRECT_XY   0x102574   //   32    -                  r     Touch-Screen Direct Conversions XY (x-MSB16; y-LSB16)
#define REG_TOUCH_DIRECT_Z1Z2 0x102578   //   32    -                  r     Touch-Screen Direct Conversions Z (z1-MSB16; z2-LSB16)
#define REG_TRACKER           0x109000   //   32    0x00000000       r/w     Track Register (Track Value – MSB16; Tag Value - LSB8)

#else

// REGISTERS AND ADDRESSES    FT810

//      REGISTER              ADDRESS       SIZE    RESET VALUE     TYPE     DESCRIPTION

#define REG_ID                0x302000   //    8    0x7C               r     Identification Register, Always 0x7C
#define REG_FRAMES            0x302004   //   32    0x00000000         r     Frame Counter, Since Reset
#define REG_CLOCK             0x302008   //   32    0x00000000         r     Clock cycles, Since Reset
#define REG_FREQUENCY         0x30200C   //   28    0x03938700       r/w     Main Clock Frequency
#define REG_RENDERMODE        0x302010   //    1    0x00             r/w     Rendering Mode: 0 = normal, 1 = single-line
#define REG_SNAPY             0x302014   //   11    0x0000           r/w     Scan Line Select for RENDERMODE 1
#define REG_SNAPSHOT          0x302018   //    1    -                  r     Trigger for RENDERMODE 1
#define REG_SNAPFORMAT        0x30201C   //    6    0x20             r/w     Pixel Format for Scanline Readout
#define REG_CPURESET          0x302020   //    3    0x02             r/w     RESET Bit2 Audio - Bit1 Touch - Bit0 Graphics
#define REG_TAP_CRC           0x302024   //   32    -                  r     Live Video Tap
#define REG_TAP_MASK          0x302028   //   32    0xFFFFFFFF       r/w     Live Video Tap Mask
#define REG_HCYCLE            0x30202C   //   12    0x224            r/w     Horizontal Total Cycle Count
#define REG_HOFFSET           0x302030   //   12    0x02B            r/w     Horizontal Display Start Offset
#define REG_HSIZE             0x302034   //   12    0x1E0            r/w     Horizontal Display Pixel Count
#define REG_HSYNC0            0x302038   //   12    0x000            r/w     Horizontal Sync Fall Offset
#define REG_HSYNC1            0x30203C   //   12    0x029            r/w     Horizontal Sync Rise Offset
#define REG_VCYCLE            0x302040   //   12    0x124            r/w     Vertical Total Cycle Count
#define REG_VOFFSET           0x302044   //   12    0x00C            r/w     Vertical Display Start Offset
#define REG_VSIZE             0x302048   //   12    0x110            r/w     Vertical Display Line Count
#define REG_VSYNC0            0x30204C   //   10    0x000            r/w     Vertical Sync Fall Offset
#define REG_VSYNC1            0x302050   //   10    0x00A            r/w     Vertical Sync Rise Offset
#define REG_DLSWAP            0x302054   //    2    0x00             r/w     Display List Swap Control
#define REG_ROTATE            0x302058   //    3    0x00             r/w     Screen 90,180, 270 degree rotate
#define REG_OUTBITS           0x30205C   //    9    0x1B6            r/w     Output Resolution, 3x3x3 Bits
#define REG_DITHER            0x302060   //    1    0x01             r/w     Output Dither Enable
#define REG_SWIZZLE           0x302064   //    4    0x00             r/w     Output RGB Swizzle, Pin Change for PCB Routing 
#define REG_CSPREAD           0x302068   //    1    0x01             r/w     Output Clock Spreading Enable
#define REG_PCLK_POL          0x30206C   //    1    0x00             r/w     PCLK Polarity: 0 = Rising Edge, 1 = Falling Edge
#define REG_PCLK              0x302070   //    8    0x00             r/w     PCLK Frequency Divider, 0 = Disable Clock
#define REG_TAG_X             0x302074   //   11    0x000            r/w     Tag Query X Coordinate
#define REG_TAG_Y             0x302078   //   11    0x000            r/w     Tag Query Y Coordinate
#define REG_TAG               0x30207C   //    8    0x00               r     Tag Query Result
#define REG_VOL_PB            0x302080   //    8    0xFF             r/w     Audio Playback Volume
#define REG_VOL_SOUND         0x302084   //    8    0xFF             r/w     Audio Synthesizer Volume
#define REG_SOUND             0x302088   //   16    0x0000           r/w     Audio Sound Effect Select 
#define REG_PLAY              0x30208C   //    1    0x00             r/w     Audio Start Effect Playback 
#define REG_GPIO_DIR          0x302090   //    8    0x80             r/w     GPIO Pin Direction: 0 = Input , 1 = Output
#define REG_GPIO              0x302094   //    8    0x00             r/w     GPIO Pin Values for 0, 1, 7 Drive Strength 2, 3, 4, 5, 6
#define REG_GPIOX_DIR         0x302098   //   16    0x8000           r/w     Extended GPIO Pin Direction
#define REG_GPIOX             0x30209C   //   16    0x0080           r/w     Extended GPIO Pin Values
//      Reserved Addr         0x3020A0
//      Reserved Addr         0x3020A4
#define REG_INT_FLAGS         0x3020A8   //    8    0x00               r     Interrupt Flags, Clear by Reading 
#define REG_INT_EN            0x3020AC   //    1    0x00             r/w     Global Interrupt Enable
#define REG_INT_MASK          0x3020B0   //    8    0xFF             r/w     Interrupt Enable Mask
#define REG_PLAYBACK_START    0x3020B4   //   20    0x00000          r/w     Audio Playback RAM Start Address
#define REG_PLAYBACK_LENGTH   0x3020B8   //   20    0x00000          r/w     Audio Playback Sample Length (Bytes)
#define REG_PLAYBACK_READPTR  0x3020BC   //   20    -                  r     Audio Playback Read Pointer
#define REG_PLAYBACK_FREQ     0x3020C0   //   16    0x1F40           r/w     Audio Playback Frequency (Hz)
#define REG_PLAYBACK_FORMAT   0x3020C4   //    2    0x00             r/w     Audio Playback Format
#define REG_PLAYBACK_LOOP     0x3020C8   //    1    0x00             r/w     Audio Playback Loop Enable
#define REG_PLAYBACK_PLAY     0x3020CC   //    1    0x00               r     Audio Start Playback 
#define REG_PWM_HZ            0x3020D0   //   14    0x00FA           r/w     Backlight PWM Frequency (Hz)
#define REG_PWM_DUTY          0x3020D4   //    8    0x80             r/w     Backlight PWM Duty Cycle: 0 = 0%, 128 = 100% 
#define REG_MACRO_0           0x3020D8   //   32    0x00000000       r/w     Display List Macro Command 0
#define REG_MACRO_1           0x3020DC   //   32    0x00000000       r/w     Display List Macro Command 1
//      Reserved Addr         0x3020E0
//      Reserved Addr         0x3020E4
//      Reserved Addr         0x3020E8
//      Reserved Addr         0x3020EC
//      Reserved Addr         0x3020F0
//      Reserved Addr         0x3020F4
#define REG_CMD_READ          0x3020F8   //   12    0x000            r/w     Command Buffer Read Pointer
#define REG_CMD_WRITE         0x3020FC   //   12    0x000            r/w     Command Buffer Write Pointer
#define REG_CMD_DL            0x302100   //   13    0x0000           r/w     Command Display List Offset 
#define REG_TOUCH_MODE        0x302104   //    2    0x03             r/w     Touch-Screen Sampling Mode
#define REG_TOUCH_ADC_MODE    0x302108   //    1    0x01             r/w     Select Single Ended or Differential Sampling
#define REG_TOUCH_CHARGE      0x30210C   //   16    0x1770           r/w     Touch Screen Charge Time, n x 6 Clocks
#define REG_TOUCH_SETTLE      0x302110   //    4    0x03             r/w     Touch-Screen Settle Time, n x 6 Clocks
#define REG_TOUCH_OVERSAMPLE  0x302114   //    4    0x07             r/w     Touch-Screen Oversample Factor
#define REG_TOUCH_RZTHRESH    0x302118   //   16    0xFFFF           r/w     Touch-Screen Resistance Threshold
#define REG_TOUCH_RAW_XY      0x30211C   //   32    -                  r     Touch-Screen Raw (x-MSB16; y-LSB16)
#define REG_TOUCH_RZ          0x302120   //   16    -                  r     Touch-Screen Resistance
#define REG_TOUCH_SCREEN_XY   0x302124   //   32    -                  r     Touch-Screen Screen (x-MSB16; y-LSB16)
#define REG_TOUCH_TAG_XY      0x302128   //   32    -                  r     Touch-Screen Tag 0 Lookup (x-MSB16; y-LSB16)
#define REG_TOUCH_TAG         0x30212C   //    8    -                  r     Touch-Screen Tag 0 Result
#define REG_TOUCH_TAG1_XY     0x302130   //   32    -                  r     Touch-Screen Tag 1 Lookup
#define REG_TOUCH_TAG1        0x302134   //    8    -                  r     Touch-Screen Tag 1 Result
#define REG_TOUCH_TAG2_XY     0x302138   //   32    -                  r     Touch-Screen Tag 2 Lookup
#define REG_TOUCH_TAG2        0x30213C   //    8    -                  r     Touch-Screen Tag 2 Result
#define REG_TOUCH_TAG3_XY     0x302140   //   32    -                  r     Touch-Screen Tag 3 Lookup
#define REG_TOUCH_TAG3        0x302144   //    8    -                  r     Touch-Screen Tag 3 Result
#define REG_TOUCH_TAG4_XY     0x302148   //   32    -                  r     Touch-Screen Tag 4 Lookup
#define REG_TOUCH_TAG4        0x30214C   //    8    -                  r     Touch-Screen Tag 4 Result
#define REG_TOUCH_TRANSFORM_A 0x302150   //   32    0x00010000       r/w     Touch-Screen Transform Coefficient A (s15.16)
#define REG_TOUCH_TRANSFORM_B 0x302154   //   32    0x00000000       r/w     Touch-Screen Transform Coefficient B (s15.16)
#define REG_TOUCH_TRANSFORM_C 0x302158   //   32    0x00000000       r/w     Touch-Screen Transform Coefficient C (s15.16)
#define REG_TOUCH_TRANSFORM_D 0x30215C   //   32    0x00000000       r/w     Touch-Screen Transform Coefficient D (s15.16)
#define REG_TOUCH_TRANSFORM_E 0x302160   //   32    0x00010000       r/w     Touch-Screen Transform Coefficient E (s15.16)
#define REG_TOUCH_TRANSFORM_F 0x302164   //   32    0x00000000       r/w     Touch-Screen Transform Coefficient F (s15.16)
#define REG_TOUCH_CONFIG      0x302168   //   16    0x8381           r/w     Touch Configuration
#define REG_CTOUCH_TOUCH4_X   0x30216C   //   16    -                  r     Extended Mode Touch Screen
//      Reserved Addresses    0x302170
#define REG_BIST_EN           0x302174   //    1    0                r/w     BIST Memory Mapping Enable
//      Reserved Addr         0x302178
//      Reserved Addr         0x30217C
#define REG_TRIM              0x302180   //    8    0                r/w     Internal Clock Trimming
#define REG_ANA_COMP          0x302184   //    8    0                r/w     Analog Control Register
#define REG_SPI_WIDTH         0x302188   //    3    0                r/w     QSPI Bus Width Setting
#define REG_TOUCH_DIRECT_XY   0x30218C   //   32    -                  r     Touch-Screen Direct Conversions XY (x-MSB16; y-LSB16)
#define REG_TOUCH_DIRECT_Z1Z2 0x302190   //   32    -                  r     Touch-Screen Direct Conversions Z (z1-MSB16; z2-LSB16)
//      Reserved Addresses    0x302194 - 0x302560
#define REG_DATESTAMP         0x320564   //  128    -                  r     Stamp Date Code
#define REG_CMDB_SPACE        0x302574   //   12    0xFFC            r/w     Command DL Space Available
#define REG_CMDB_WRITE        0x302578   //   32    0                  w     Command DL Write
#endif

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


// HOST COMMANDS

#define ACTIVE    0x00
#define STANDBY   0x41
#define SLEEP     0x42
#define PWRDOWN   0x50
#define CLKEXT    0x44
#define CLKINT    0x48
#define CLK48M    0x62
#define CLK36M    0x61
#define CORESET   0x68


// DISPLAY LIST COMMANDS

#define ALPHA_FUNC                           0x09000000

#define ALPHA_FUNC_NEVER                     0x00000000
#define ALPHA_FUNC_LESS                      0x00010000
#define ALPHA_FUNC_LEQUAL                    0x00020000
#define ALPHA_FUNC_GREATER                   0x00030000
#define ALPHA_FUNC_GEQUAL                    0x00040000
#define ALPHA_FUNC_EQUAL                     0x00050000
#define ALPHA_FUNC_NOTEQUAL                  0x00060000
#define ALPHA_FUNC_ALWAYS                    0x00070000

#define BEGIN                                0x1F000000

#define BEGIN_BITMAPS                        0x00000001
#define BEGIN_POINTS                         0x00000002
#define BEGIN_LINES                          0x00000003
#define BEGIN_LINE_STRIP                     0x00000004
#define BEGIN_STRIP_R                        0x00000005
#define BEGIN_STRIP_L                        0x00000006
#define BEGIN_STRIP_A                        0x00000007
#define BEGIN_STRIP_B                        0x00000008
#define BEGIN_STRIP_RECTS                    0x00000009

#define BITMAP_HANDLE                        0x05000000

#define BITMAP_LAYOUT                        0x07000000

#define BITMAP_LAYOUT_ARGB1555               0x00000000
#define BITMAP_LAYOUT_L1                     0x00080000
#define BITMAP_LAYOUT_L4                     0x00100000
#define BITMAP_LAYOUT_L8                     0x00180000
#define BITMAP_LAYOUT_RGB332                 0x00200000
#define BITMAP_LAYOUT_ARGB2                  0x00280000
#define BITMAP_LAYOUT_ARGB4                  0x00300000
#define BITMAP_LAYOUT_RGB565                 0x00380000
#define BITMAP_LAYOUT_PALETTED               0x00400000
#define BITMAP_LAYOUT_TEXT8X8                0x00480000
#define BITMAP_LAYOUT_TEXTVGA                0x00500000
#define BITMAP_LAYOUT_BARGRAPH               0x00580000

#define ARGB1555                             0x00
#define L1                                   0x01
#define L4                                   0x02
#define L8                                   0x03
#define RGB332                               0x04
#define ARGB2                                0x05
#define ARGB4                                0x06
#define RGB565                               0x07
#define PALETTED                             0x08
#define TEXT8X8                              0x09
#define TEXTVGA                              0x0A
#define BARGRAPH                             0x0B

#define BITMAP_SIZE                          0x08000000

#define NEAREST                              0x00
#define BILINEAR                             0x01
#define BORDER                               0x00
#define REPEAT                               0x01

#define BITMAP_SOURCE                        0x01000000

#define BITMAP_TRANSFORM_A                   0x15000000

#define BITMAP_TRANSFORM_B                   0x16000000

#define BITMAP_TRANSFORM_C                   0x17000000

#define BITMAP_TRANSFORM_D                   0x18000000

#define BITMAP_TRANSFORM_E                   0x19000000

#define BITMAP_TRANSFORM_F                   0x1A000000

#define BLEND_FUNC                           0x0B000000

#define BLEND_FUNC_SRC_ZERO                  0x00000000
#define BLEND_FUNC_SRC_ONE                   0x00000008
#define BLEND_FUNC_SRC_SRC_ALPHA             0x00000010
#define BLEND_FUNC_SRC_DST_ALPHA             0x00000018
#define BLEND_FUNC_SRC_ONE_MINUS_SRC_ALPHA   0x00000020
#define BLEND_FUNC_SRC_ONE_MINUS_DST_ALPHA   0x00000028

#define BLEND_FUNC_DST_ZERO                  0x00000000
#define BLEND_FUNC_DST_ONE                   0x00000001
#define BLEND_FUNC_DST_SRC_ALPHA             0x00000002
#define BLEND_FUNC_DST_DST_ALPHA             0x00000003
#define BLEND_FUNC_DST_ONE_MINUS_SRC_ALPHA   0x00000004
#define BLEND_FUNC_DST_ONE_MINUS_DST_ALPHA   0x00000005

#define CALL                                 0x1D000000

//#define CELL                                 0x06000000

#define CLEAR                                0x26000000

#define CLEAR_COLOR_BUFFER                   0x00000004
#define CLEAR_STENCIL_BUFFER                 0x00000002
#define CLEAR_TAG_BUFFER                     0x00000001

#define CLEAR_COLOR_A                        0x0F000000

#define CLEAR_COLOR_RGB                      0x02000000

#define CLEAR_STENCIL                        0x11000000

#define CLEAR_TAG                            0x12000000

#define COLOR_A                              0x10000000

#define COLOR_MASK                           0x20000000

#define COLOR_MASK_RED                       0x00000008
#define COLOR_MASK_GRN                       0x00000004
#define COLOR_MASK_BLU                       0x00000002
#define COLOR_MASK_ALPHA                     0x00000001

#define COLOR_RGB                            0x04000000

#define DISPLAY                              0x00000000

#define END                                  0x21000000

#define JUMP                                 0x1E000000

#define LINE_WIDTH                           0x0E000000

#define MACRO                                0x25000000

#define POINT_SIZE                           0x0D000000

#define RESTORE_CONTEXT                      0x23000000

#define RETURN                               0x24000000

#define SAVE_CONTEXT                         0x22000000

#define SCISSOR_SIZE                         0x1C000000

#define SCISSOR_XY                           0x1B000000

#define STENCIL_FUNC                         0x0A000000

#define STENCIL_FUNC_NEVER                   0x00000000
#define STENCIL_FUNC_LESS                    0x00010000
#define STENCIL_FUNC_LEQUAL                  0x00020000
#define STENCIL_FUNC_GREATER                 0x00030000
#define STENCIL_FUNC_GEQUAL                  0x00040000
#define STENCIL_FUNC_EQUAL                   0x00050000
#define STENCIL_FUNC_NOTEQUAL                0x00060000
#define STENCIL_FUNC_ALWAYS                  0x00070000

#define STENCIL_MASK                         0x13000000

#define STENCIL_OP                           0x0C000000

#define STENCIL_OP_PASS_ZERO                 0x00000000
#define STENCIL_OP_PASS_KEEP                 0x00000001
#define STENCIL_OP_PASS_REPLACE              0x00000002
#define STENCIL_OP_PASS_INCR                 0x00000003
#define STENCIL_OP_PASS_DECR                 0x00000004
#define STENCIL_OP_PASS_INVERT               0x00000005

#define STENCIL_OP_FAIL_ZERO                 0x00000000
#define STENCIL_OP_FAIL_KEEP                 0x00000008
#define STENCIL_OP_FAIL_REPLACE              0x00000010
#define STENCIL_OP_FAIL_INCR                 0x00000018
#define STENCIL_OP_FAIL_DECR                 0x00000020
#define STENCIL_OP_FAIL_INVERT               0x00000028

#define TAG                                  0x03000000

#define TAG_MASK                             0x14000000

#define VERTEX2F                             0x40000000

#define VERTEX2II                            0x80000000

// CO-PROCESSOR ENGINE COMMANDS

#define CMD_DLSTART                          0xFFFFFF00
#define CMD_SWAP                             0xFFFFFF01
#define CMD_COLDSTART                        0xFFFFFF32
#define CMD_INTERRUPT                        0xFFFFFF02
#define CMD_APPEND                           0xFFFFFF1E
#define CMD_REGREAD                          0xFFFFFF19
#define CMD_MEMWRITE                         0xFFFFFF1A
#define CMD_INFLATE                          0xFFFFFF22
#define CMD_LOADIMAGE                        0xFFFFFF24
#define CMD_MEMCRC                           0xFFFFFF18
#define CMD_MEMZERO                          0xFFFFFF1C
#define CMD_MEMSET                           0xFFFFFF1B
#define CMD_MEMCPY                           0xFFFFFF1D
#define CMD_BUTTON                           0xFFFFFF0D
#define CMD_CLOCK                            0xFFFFFF14
#define CMD_FGCOLOR                          0xFFFFFF0A
#define CMD_BGCOLOR                          0xFFFFFF09
#define CMD_GRADCOLOR                        0xFFFFFF34
#define CMD_GAUGE                            0xFFFFFF13
#define CMD_GRADIENT                         0xFFFFFF0B
#define CMD_KEYS                             0xFFFFFF0E
#define CMD_PROGRESS                         0xFFFFFF0F
#define CMD_SCROLLBAR                        0xFFFFFF11
#define CMD_SLIDER                           0xFFFFFF10
#define CMD_DIAL                             0xFFFFFF2D
#define CMD_TOGGLE                           0xFFFFFF12
#define CMD_TEXT                             0xFFFFFF0C
#define CMD_NUMBER                           0xFFFFFF2E
#define CMD_LOADIDENTITY                     0xFFFFFF26
#define CMD_SETMATRIX                        0xFFFFFF2A
#define CMD_GETMATRIX                        0xFFFFFF33
#define CMD_GETPTR                           0xFFFFFF23
#define CMD_GETPROPS                         0xFFFFFF25
#define CMD_SCALE                            0xFFFFFF28
#define CMD_ROTATE                           0xFFFFFF29
#define CMD_TRANSLATE                        0xFFFFFF27
#define CMD_CALIBRATE                        0xFFFFFF15
#define CMD_SPINNER                          0xFFFFFF16
#define CMD_SCREENSAVER                      0xFFFFFF2F
#define CMD_SKETCH                           0xFFFFFF30
#define CMD_STOP                             0xFFFFFF17
#define CMD_SETFONT                          0xFFFFFF2B
#define CMD_TRACK                            0xFFFFFF2C
#define CMD_SNAPSHOT                         0xFFFFFF1F
#define CMD_LOGO                             0xFFFFFF31


#endif // _AO_FT810_H





