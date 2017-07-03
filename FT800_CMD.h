/***************
 * FT800_CMD.h *
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

#define CELL                                 0x06000000

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








