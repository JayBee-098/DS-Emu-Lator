/*
	Copyright (C) 2006 yopyop
	Copyright (C) 2006-2007 Theo Berkau
	Copyright (C) 2007 shash
	Copyright (C) 2009-2015 DeSmuME team

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the this software.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GPU_H
#define GPU_H

#include <stdio.h>
#include <iosfwd>

#include "types.h"

#ifdef ENABLE_SSE2
#include <emmintrin.h>
#endif

class GPUEngineBase;
class EMUFILE;
struct MMU_struct;

//#undef FORCEINLINE
//#define FORCEINLINE

#define GPU_FRAMEBUFFER_NATIVE_WIDTH	256
#define GPU_FRAMEBUFFER_NATIVE_HEIGHT	192

#define GPU_VRAM_BLOCK_LINES			256

void gpu_savestate(EMUFILE* os);
bool gpu_loadstate(EMUFILE* is, int size);

typedef void (*rot_fun)(GPUEngineBase *gpu, const s32 auxX, const s32 auxY, const int lg, const u32 map, const u32 tile, const u16 *pal, const size_t i);

union FragmentColor
{
	u32 color;
	struct
	{
		u8 r,g,b,a;
	};
};

typedef union
{
    u32 value;
	
	struct
	{
#ifdef LOCAL_LE
		u8 BG_Mode:3;						//  0- 2: A+B;
		u8 BG0_3D:1;						//     3: A  ; 0=2D,         1=3D
		u8 OBJ_Tile_mapping:1;				//     4: A+B; 0=2D (32KB),  1=1D (32..256KB)
		u8 OBJ_BMP_2D_dim:1;				//     5: A+B; 0=128x512,    1=256x256 pixels
		u8 OBJ_BMP_mapping:1;				//     6: A+B; 0=2D (128KB), 1=1D (128..256KB)
		u8 ForceBlank:1;					//     7: A+B;
		
		u8 BG0_Enable:1;					//     8: A+B; 0=Disable, 1=Enable
		u8 BG1_Enable:1;					//     9: A+B; 0=Disable, 1=Enable
		u8 BG2_Enable:1;					//    10: A+B; 0=Disable, 1=Enable
		u8 BG3_Enable:1;					//    11: A+B; 0=Disable, 1=Enable
		u8 OBJ_Enable:1;					//    12: A+B; 0=Disable, 1=Enable
		u8 Win0_Enable:1;					//    13: A+B; 0=Disable, 1=Enable
		u8 Win1_Enable:1;					//    14: A+B; 0=Disable, 1=Enable
		u8 WinOBJ_Enable:1;					//    15: A+B; 0=Disable, 1=Enable
		
		u8 DisplayMode:2;					// 16-17: A+B; coreA(0..3) coreB(0..1) GBA(Green Swap)
											//        0=off (white screen)
											//        1=on (normal BG & OBJ layers)
											//        2=VRAM display (coreA only)
											//        3=RAM display (coreA only, DMA transfers)
		u8 VRAM_Block:2;					// 18-19: A  ; VRAM block (0..3=A..D)
		u8 OBJ_Tile_1D_Bound:2;				//    20: A+B;
		u8 OBJ_BMP_1D_Bound:1;				// 21-22: A  ;
		u8 OBJ_HBlank_process:1;			//    23: A+B; OBJ processed during HBlank (GBA bit5)
		
		u8 CharacBase_Block:3;				// 24-26: A  ; Character Base (64K step)
		u8 ScreenBase_Block:3;				// 27-29: A  ; Screen Base (64K step)
		u8 ExBGxPalette_Enable:1;			//    30: A+B; 0=Disable, 1=Enable BG extended Palette
		u8 ExOBJPalette_Enable:1;			//    31: A+B; 0=Disable, 1=Enable OBJ extended Palette
#else
		u8 ForceBlank:1;					//     7: A+B;
		u8 OBJ_BMP_mapping:1;				//     6: A+B; 0=2D (128KB), 1=1D (128..256KB)
		u8 OBJ_BMP_2D_dim:1;				//     5: A+B; 0=128x512,    1=256x256 pixels
		u8 OBJ_Tile_mapping:1;				//     4: A+B; 0=2D (32KB),  1=1D (32..256KB)
		u8 BG0_3D:1;						//     3: A  ; 0=2D,         1=3D
		u8 BG_Mode:3;						//  0- 2: A+B;
		
		u8 WinOBJ_Enable:1;					//    15: A+B; 0=Disable, 1=Enable
		u8 Win1_Enable:1;					//    14: A+B; 0=Disable, 1=Enable
		u8 Win0_Enable:1;					//    13: A+B; 0=Disable, 1=Enable
		u8 OBJ_Enable:1;					//    12: A+B; 0=Disable, 1=Enable
		u8 BG3_Enable:1;					//    11: A+B; 0=Disable, 1=Enable
		u8 BG2_Enable:1;					//    10: A+B; 0=Disable, 1=Enable
		u8 BG1_Enable:1;					//     9: A+B; 0=Disable, 1=Enable
		u8 BG0_Enable:1;					//     8: A+B; 0=Disable, 1=Enable
		
		u8 OBJ_HBlank_process:1;			//    23: A+B; OBJ processed during HBlank (GBA bit5)
		u8 OBJ_BMP_1D_Bound:1;				//    22: A  ;
		u8 OBJ_Tile_1D_Bound:2;				// 20-21: A+B;
		u8 VRAM_Block:2;					// 18-19: A  ; VRAM block (0..3=A..D)
		u8 DisplayMode:2;					// 16-17: A+B; coreA(0..3) coreB(0..1) GBA(Green Swap)
											//        0=off (white screen)
											//        1=on (normal BG & OBJ layers)
											//        2=VRAM display (coreA only)
											//        3=RAM display (coreA only, DMA transfers)
		
		u8 ExOBJPalette_Enable:1;			//    31: A+B; 0=Disable, 1=Enable OBJ extended Palette
		u8 ExBGxPalette_Enable:1;			//    30: A+B; 0=Disable, 1=Enable BG extended Palette
		u8 ScreenBase_Block:3;				// 27-29: A  ; Screen Base (64K step)
		u8 CharacBase_Block:3;				// 24-26: A  ; Character Base (64K step)
#endif
	};
} IOREG_DISPCNT;							// 0x400x000: Display control (Engine A+B)

typedef union
{
	u16 value;
	
	struct
	{
		u16 VBlankFlag:1;					//     0: Set at V-Blank; 0=Not in V-Blank, 1=V-Blank occurred
		u16 HBlankFlag:1;					//     1: Set at H-Blank; 0=Not in H-Blank, 1=H-Blank occurred
		u16 VCounterFlag:1;					//     2: Set when this register's VCount matches the currently rendered scanline, interacts with VCOUNT (0x4000006); 0=Unmatched, 1=Matched
		u16 VBlankIRQ_Enable:1;				//     3: Send an IRQ when VBlankFlag is set; 0=Disable, 1=Enable
		u16 HBlankIRQ_Enable:1;				//     4: Send an IRQ when HBlankFlag is set; 0=Disable, 1=Enable
		u16 VCounterIRQ_Enable:1;			//     5: Send an IRQ when VCounterFlag is set; 0=Disable, 1=Enable
		u16 LCDInitReady:1;					//     6: Report that the LCD initialization is ready, DSi only; 0=Unready 1=Ready
		u16 VCount:9;						//  7-15: Current scanline, interacts with VCOUNT (0x4000006)
	};
} IOREG_DISPSTAT;							// 0x4000004: Display status (Engine A only)

typedef union
{
	u16 value;
	
	struct
	{
		u16 CurrentScanline:9;				//  0- 8: The scanline currently being rendered; 0...262
		u16 :7;								//  9-15: Unused bits
	};
} IOREG_VCOUNT;								// 0x4000006: Current display scanline (Engine A only)

/*******************************************************************************
    this structure is for display control of a specific layer,
    there are 4 background layers
    their priority indicate which one to draw on top of the other
    some flags indicate special drawing mode, size, FX
*******************************************************************************/
typedef union
{
    u16 value;
	
	struct
	{
#ifdef LOCAL_LE
		u8 Priority:2;						//  0- 1: 0..3=high..low
		u8 CharacBase_Block:4;				//  2- 5: individual character base offset (n*16KB)
		u8 Mosaic_Enable:1;					//     6: 0=disable, 1=Enable mosaic
		u8 Palette_256:1;					//     7: 0=16x16, 1=1*256 palette
		
		u8 ScreenBase_Block:5;				//  8-12: individual screen base offset (text n*2KB, BMP n*16KB)
		u8 PaletteSet_Wrap:1;				//    13: BG0 extended palette set 0=set0, 1=set2
											//        BG1 extended palette set 0=set1, 1=set3
											//        BG2 overflow area wraparound 0=off, 1=wrap
											//        BG3 overflow area wraparound 0=off, 1=wrap
		u8 ScreenSize:2;					// 14-15: text    : 256x256 512x256 256x512 512x512
											//        x/rot/s : 128x128 256x256 512x512 1024x1024
											//        bmp     : 128x128 256x256 512x256 512x512
											//        large   : 512x1024 1024x512 - -
#else
		u8 Palette_256:1;					//     7: 0=16x16, 1=1*256 palette
		u8 Mosaic_Enable:1;					//     6: 0=disable, 1=Enable mosaic
		u8 CharacBase_Block:4;				//  2- 5: individual character base offset (n*16KB)
		u8 Priority:2;						//  0- 1: 0..3=high..low
		
		u8 ScreenSize:2;					// 14-15: text    : 256x256 512x256 256x512 512x512
											//        x/rot/s : 128x128 256x256 512x512 1024x1024
											//        bmp     : 128x128 256x256 512x256 512x512
											//        large   : 512x1024 1024x512 - -
		u8 PaletteSet_Wrap:1;				//    13: BG0 extended palette set 0=set0, 1=set2
											//        BG1 extended palette set 0=set1, 1=set3
											//        BG2 overflow area wraparound 0=off, 1=wrap
											//        BG3 overflow area wraparound 0=off, 1=wrap
		u8 ScreenBase_Block:5;				//  8-12: individual screen base offset (text n*2KB, BMP n*16KB)
#endif
	};
} IOREG_BGnCNT;								// 0x400x008, 0x400x00A, 0x400x00C, 0x400x00E: BGn layer control (Engine A+B)

typedef IOREG_BGnCNT IOREG_BG0CNT;			// 0x400x008: BG0 layer control (Engine A+B)
typedef IOREG_BGnCNT IOREG_BG1CNT;			// 0x400x00A: BG1 layer control (Engine A+B)
typedef IOREG_BGnCNT IOREG_BG2CNT;			// 0x400x00C: BG2 layer control (Engine A+B)
typedef IOREG_BGnCNT IOREG_BG3CNT;			// 0x400x00E: BG3 layer control (Engine A+B)

typedef union
{
	u16 value;
	
	struct
	{
		u16 Offset:9;						//  0- 8: Offset value; 0...511
		u16 :7;								//  9-15: Unused bits
	};
} IOREG_BGnHOFS;							// 0x400x010, 0x400x014, 0x400x018, 0x400x01C: BGn horizontal offset (Engine A+B)

typedef IOREG_BGnHOFS IOREG_BGnVOFS;		// 0x400x012, 0x400x016, 0x400x01A, 0x400x01E: BGn vertical offset (Engine A+B)

typedef IOREG_BGnHOFS IOREG_BG0HOFS;		// 0x400x010: BG0 horizontal offset (Engine A+B)
typedef IOREG_BGnVOFS IOREG_BG0VOFS;		// 0x400x012: BG0 vertical offset (Engine A+B)
typedef IOREG_BGnHOFS IOREG_BG1HOFS;		// 0x400x014: BG1 horizontal offset (Engine A+B)
typedef IOREG_BGnVOFS IOREG_BG1VOFS;		// 0x400x016: BG1 vertical offset (Engine A+B)
typedef IOREG_BGnHOFS IOREG_BG2HOFS;		// 0x400x018: BG2 horizontal offset (Engine A+B)
typedef IOREG_BGnVOFS IOREG_BG2VOFS;		// 0x400x01A: BG2 vertical offset (Engine A+B)
typedef IOREG_BGnHOFS IOREG_BG3HOFS;		// 0x400x01C: BG3 horizontal offset (Engine A+B)
typedef IOREG_BGnVOFS IOREG_BG3VOFS;		// 0x400x01E: BG3 vertical offset (Engine A+B)

typedef struct
{
	IOREG_BGnHOFS BGnHOFS;
	IOREG_BGnVOFS BGnVOFS;
} IOREG_BGnOFS;								// 0x400x010, 0x400x014, 0x400x018, 0x400x01C: BGn horizontal offset (Engine A+B)

typedef union
{
	s16 value;
	
	struct
	{
		u16 Fraction:8;
		s16 Integer:8;
	};
} IOREG_BGnPA;								// 0x400x020, 0x400x030: BGn rotation/scaling parameter A (Engine A+B)
typedef IOREG_BGnPA IOREG_BGnPB;			// 0x400x022, 0x400x032: BGn rotation/scaling parameter B (Engine A+B)
typedef IOREG_BGnPA IOREG_BGnPC;			// 0x400x024, 0x400x034: BGn rotation/scaling parameter C (Engine A+B)
typedef IOREG_BGnPA IOREG_BGnPD;			// 0x400x026, 0x400x036: BGn rotation/scaling parameter D (Engine A+B)

typedef union
{
	u32 value;
	
	struct
	{
		u32 Fraction:8;
		s32 Integer:20;
		u32 :4;
	};
} IOREG_BGnX;								// 0x400x028, 0x400x038: BGn X-coordinate (Engine A+B)
typedef IOREG_BGnX IOREG_BGnY;				// 0x400x02C, 0x400x03C: BGn Y-coordinate (Engine A+B)

typedef IOREG_BGnPA IOREG_BG2PA;			// 0x400x020: BG2 rotation/scaling parameter A (Engine A+B)
typedef IOREG_BGnPB IOREG_BG2PB;			// 0x400x022: BG2 rotation/scaling parameter B (Engine A+B)
typedef IOREG_BGnPC IOREG_BG2PC;			// 0x400x024: BG2 rotation/scaling parameter C (Engine A+B)
typedef IOREG_BGnPD IOREG_BG2PD;			// 0x400x026: BG2 rotation/scaling parameter D (Engine A+B)
typedef IOREG_BGnX IOREG_BG2X;				// 0x400x028: BG2 X-coordinate (Engine A+B)
typedef IOREG_BGnY IOREG_BG2Y;				// 0x400x02C: BG2 Y-coordinate (Engine A+B)

typedef IOREG_BGnPA IOREG_BG3PA;			// 0x400x030: BG3 rotation/scaling parameter A (Engine A+B)
typedef IOREG_BGnPB IOREG_BG3PB;			// 0x400x032: BG3 rotation/scaling parameter B (Engine A+B)
typedef IOREG_BGnPC IOREG_BG3PC;			// 0x400x034: BG3 rotation/scaling parameter C (Engine A+B)
typedef IOREG_BGnPD IOREG_BG3PD;			// 0x400x036: BG3 rotation/scaling parameter D (Engine A+B)
typedef IOREG_BGnX IOREG_BG3X;				// 0x400x038: BG3 X-coordinate (Engine A+B)
typedef IOREG_BGnY IOREG_BG3Y;				// 0x400x03C: BG3 Y-coordinate (Engine A+B)

typedef struct
{
	IOREG_BGnPA BGnPA;						// 0x400x020, 0x400x030: BGn rotation/scaling parameter A (Engine A+B)
	IOREG_BGnPB BGnPB;						// 0x400x022, 0x400x032: BGn rotation/scaling parameter B (Engine A+B)
	IOREG_BGnPC BGnPC;						// 0x400x024, 0x400x034: BGn rotation/scaling parameter C (Engine A+B)
	IOREG_BGnPD BGnPD;						// 0x400x026, 0x400x036: BGn rotation/scaling parameter D (Engine A+B)
	IOREG_BGnX BGnX;						// 0x400x028, 0x400x038: BGn X-coordinate (Engine A+B)
	IOREG_BGnY BGnY;						// 0x400x02C, 0x400x03C: BGn Y-coordinate (Engine A+B)
} IOREG_BGnParameter;						// 0x400x020, 0x400x030: BGn parameters (Engine A+B)

typedef struct
{
	IOREG_BG2PA BG2PA;						// 0x400x020: BG2 rotation/scaling parameter A (Engine A+B)
	IOREG_BG2PB BG2PB;						// 0x400x022: BG2 rotation/scaling parameter B (Engine A+B)
	IOREG_BG2PC BG2PC;						// 0x400x024: BG2 rotation/scaling parameter C (Engine A+B)
	IOREG_BG2PD BG2PD;						// 0x400x026: BG2 rotation/scaling parameter D (Engine A+B)
	IOREG_BG2X BG2X;						// 0x400x028: BG2 X-coordinate (Engine A+B)
	IOREG_BG2Y BG2Y;						// 0x400x02C: BG2 Y-coordinate (Engine A+B)
} IOREG_BG2Parameter;						// 0x400x020: BG2 parameters (Engine A+B)

typedef struct
{
	IOREG_BG3PA BG3PA;						// 0x400x030: BG3 rotation/scaling parameter A (Engine A+B)
	IOREG_BG3PB BG3PB;						// 0x400x032: BG3 rotation/scaling parameter B (Engine A+B)
	IOREG_BG3PC BG3PC;						// 0x400x034: BG3 rotation/scaling parameter C (Engine A+B)
	IOREG_BG3PD BG3PD;						// 0x400x036: BG3 rotation/scaling parameter D (Engine A+B)
	IOREG_BG3X BG3X;						// 0x400x038: BG3 X-coordinate (Engine A+B)
	IOREG_BG3Y BG3Y;						// 0x400x03C: BG3 Y-coordinate (Engine A+B)
} IOREG_BG3Parameter;						// 0x400x030: BG3 parameters (Engine A+B)

/*******************************************************************************
	these structures are for window description,
	windows are square regions and can "subclass"
	background layers or object layers (i.e window controls the layers)

	screen
	|
	+-- Window0/Window1/OBJwindow/OutOfWindows
		|
		+-- BG0/BG1/BG2/BG3/OBJ
*******************************************************************************/

typedef union
{
	u16 value;
	
	struct
	{
		u16 Right:8;						//  0- 7: Right coordinate of window
		u16 Left:8;							//  8-15: Left coordinate of window
	};
} IOREG_WIN0H;								// 0x400x040: Horizontal coordinates of Window 0 (Engine A+B)
typedef IOREG_WIN0H IOREG_WIN1H;			// 0x400x042: Horizontal coordinates of Window 1 (Engine A+B)

typedef union
{
	u16 value;
	
	struct
	{
		u16 Bottom:8;						//  0- 7: Bottom coordinate of window
		u16 Top:8;							//  8-15: Top coordinate of window
	};
} IOREG_WIN0V;								// 0x400x044: Vertical coordinates of Window 0 (Engine A+B)
typedef IOREG_WIN0V IOREG_WIN1V;			// 0x400x046: Vertical coordinates of Window 1 (Engine A+B)

typedef union
{
	u8 value;
	
	struct
	{
#ifdef LOCAL_LE
		u8 BG0_Enable:1;					//     0: Layer BG0 display; 0=Disable, 1=Enable
		u8 BG1_Enable:1;					//     1: Layer BG1 display; 0=Disable, 1=Enable
		u8 BG2_Enable:1;					//     2: Layer BG2 display; 0=Disable, 1=Enable
		u8 BG3_Enable:1;					//     3: Layer BG3 display; 0=Disable, 1=Enable
		u8 OBJ_Enable:1;					//     4: Layer OBJ display; 0=Disable, 1=Enable
		u8 Effect_Enable:1;					//     5: Color special effect; 0=Disable, 1=Enable
		u8 :2;								//  6- 7: Unused bits
#else
		u8 :2;								//  6- 7: Unused bits
		u8 Effect_Enable:1;					//     5: Color special effect; 0=Disable, 1=Enable
		u8 OBJ_Enable:1;					//     4: Layer OBJ display; 0=Disable, 1=Enable
		u8 BG3_Enable:1;					//     3: Layer BG3 display; 0=Disable, 1=Enable
		u8 BG2_Enable:1;					//     2: Layer BG2 display; 0=Disable, 1=Enable
		u8 BG1_Enable:1;					//     1: Layer BG1 display; 0=Disable, 1=Enable
		u8 BG0_Enable:1;					//     0: Layer BG0 display; 0=Disable, 1=Enable
#endif
	};
} IOREG_WIN0IN;								// 0x400x048: Control of inside of Window 0 (highest priority)
typedef IOREG_WIN0IN IOREG_WIN1IN;			// 0x400x049: Control of inside of Window 1 (medium priority)
typedef IOREG_WIN0IN IOREG_WINOUT;			// 0x400x04A: Control of outside of all windows
typedef IOREG_WIN0IN IOREG_WINOBJ;			// 0x400x04B: Control of inside of Window OBJ (lowest priority)

typedef union
{
	u32 value;
	
	struct
	{
		u32 BG_MosaicH:4;					//  0- 3: Mosaic pixel width for BG layers; 0...15
		u32 BG_MosaicV:4;					//  4- 7: Mosaic pixel height for BG layers; 0...15
		
		u32 OBJ_MosaicH:4;					//  8-11: Mosaic pixel width for OBJ layer; 0...15
		u32 OBJ_MosaicV:4;					// 12-15: Mosaic pixel height for OBJ layer; 0...15
		
		u32 :16;							// 16-31: Unused bits
	};
} IOREG_MOSAIC;								// 0x400x04C: Mosaic size (Engine A+B)

typedef union
{
	u16 value;
	
	struct
	{
#ifdef LOCAL_LE
		u16 BG0_Target1:1;					//     0: Select layer BG0 for 1st target; 0=Disable, 1=Enable
		u16 BG1_Target1:1;					//     1: Select layer BG1 for 1st target; 0=Disable, 1=Enable
		u16 BG2_Target1:1;					//     2: Select layer BG2 for 1st target; 0=Disable, 1=Enable
		u16 BG3_Target1:1;					//     3: Select layer BG3 for 1st target; 0=Disable, 1=Enable
		u16 OBJ_Target1:1;					//     4: Select layer OBJ for 1st target; 0=Disable, 1=Enable
		u16 Backdrop_Target1:1;				//     5: Select backdrop for 1st target; 0=Disable, 1=Enable
		u16 ColorEffect:2;					//  6- 7: Color effect mode;
											//        0=Disable
											//        1=Alpha blend 1st and 2nd target, interacts with BLDALPHA (0x400x052)
											//        2=Increase brightness, interacts with BLDY (0x400x054)
											//        3=Decrease brightness, interacts with BLDY (0x400x054)
		
		u16 BG0_Target2:1;					//     8: Select layer BG0 for 2nd target; 0=Disable, 1=Enable
		u16 BG1_Target2:1;					//     9: Select layer BG1 for 2nd target; 0=Disable, 1=Enable
		u16 BG2_Target2:1;					//    10: Select layer BG2 for 2nd target; 0=Disable, 1=Enable
		u16 BG3_Target2:1;					//    11: Select layer BG3 for 2nd target; 0=Disable, 1=Enable
		u16 OBJ_Target2:1;					//    12: Select layer OBJ for 2nd target; 0=Disable, 1=Enable
		u16 Backdrop_Target2:1;				//    13: Select backdrop for 2nd target; 0=Disable, 1=Enable
		u16 :2;								// 14-15: Unused bits
#else
		u16 ColorEffect:2;					//  6- 7: Color effect mode;
											//        0=Disable
											//        1=Alpha blend 1st and 2nd target, interacts with BLDALPHA (0x400x052)
											//        2=Increase brightness, interacts with BLDY (0x400x054)
											//        3=Decrease brightness, interacts with BLDY (0x400x054)
		u16 Backdrop_Target1:1;				//     5: Select backdrop for 1st target; 0=Disable, 1=Enable
		u16 OBJ_Target1:1;					//     4: Select layer OBJ for 1st target; 0=Disable, 1=Enable
		u16 BG3_Target1:1;					//     3: Select layer BG3 for 1st target; 0=Disable, 1=Enable
		u16 BG2_Target1:1;					//     2: Select layer BG2 for 1st target; 0=Disable, 1=Enable
		u16 BG1_Target1:1;					//     1: Select layer BG1 for 1st target; 0=Disable, 1=Enable
		u16 BG0_Target1:1;					//     0: Select layer BG0 for 1st target; 0=Disable, 1=Enable
		
		u16 :2;								// 14-15: Unused bits
		u16 Backdrop_Target2:1;				//    13: Select backdrop for 2nd target; 0=Disable, 1=Enable
		u16 OBJ_Target2:1;					//    12: Select layer OBJ for 2nd target; 0=Disable, 1=Enable
		u16 BG3_Target2:1;					//    11: Select layer BG3 for 2nd target; 0=Disable, 1=Enable
		u16 BG2_Target2:1;					//    10: Select layer BG2 for 2nd target; 0=Disable, 1=Enable
		u16 BG1_Target2:1;					//     9: Select layer BG1 for 2nd target; 0=Disable, 1=Enable
		u16 BG0_Target2:1;					//     8: Select layer BG0 for 2nd target; 0=Disable, 1=Enable
#endif
	};
} IOREG_BLDCNT;								// 0x400x050: Color effects selection (Engine A+B)

typedef union
{
	u16 value;
	
	struct
	{
		u16 EVA:5;							//  0- 4: Blending coefficient for 1st target; 0...31 (clamped to 16)
		u16 :3;								//  5- 7: Unused bits
		
		u16 EVB:5;							//  8-12: Blending coefficient for 2nd target; 0...31 (clamped to 16)
		u16 :3;								// 13-15: Unused bits
	};
} IOREG_BLDALPHA;							// 0x400x052: Color effects selection, interacts with BLDCNT (0x400x050) (Engine A+B)

typedef union
{
	u16 value;
	
	struct
	{
#ifdef LOCAL_LE
		u16 EVY:5;							//  0- 4: Blending coefficient for increase/decrease brightness; 0...31 (clamped to 16)
		u16 :3;								//  5- 7: Unused bits
		
		u16 :8;								//  8-15: Unused bits
#else
		u16 :3;								//  5- 7: Unused bits
		u16 EVY:5;							//  0- 4: Blending coefficient for increase/decrease brightness; 0...31 (clamped to 16)
		
		u16 :8;								//  8-15: Unused bits
#endif
	};
} IOREG_BLDY;								// 0x400x054: Color effects selection, interacts with BLDCNT (0x400x050) (Engine A+B)

typedef union
{
    u32 value;
	
	struct
	{
#ifdef LOCAL_LE
		u8 EnableTexMapping:1;				//     0: Apply textures; 0=Disable, 1=Enable
		u8 PolygonShading:1;				//     1: Polygon shading mode, interacts with POLYGON_ATTR (0x40004A4); 0=Toon Shading, 1=Highlight Shading
		u8 EnableAlphaTest:1;				//     2: Perform alpha test, interacts with ALPHA_TEST_REF (0x4000340); 0=Disable, 1=Enable
		u8 EnableAlphaBlending:1;			//     3: Perform alpha blending, interacts with POLYGON_ATTR (0x40004A4); 0=Disable, 1=Enable
		u8 EnableAntiAliasing:1;			//     4: Render polygon edges with antialiasing; 0=Disable, 1=Enable
		u8 EnableEdgeMarking:1;				//     5: Perform polygon edge marking, interacts with EDGE_COLOR (0x4000330); 0=Disable, 1=Enable
		u8 FogOnlyAlpha:1;					//     6: Apply fog to the alpha channel only, interacts with FOG_COLOR (0x4000358) / FOG_TABLE (0x4000360); 0=Color+Alpha, 1=Alpha
		u8 EnableFog:1;						//     7: Perform fog rendering, interacts with FOG_COLOR (0x4000358) / FOG_OFFSET (0x400035C) / FOG_TABLE (0x4000360);
											//        0=Disable, 1=Enable
		
		u8 FogShiftSHR:4;					//  8-11: SHR-Divider, interacts with FOG_OFFSET (0x400035C); 0...10
		u8 AckColorBufferUnderflow:1;		//    12: Color Buffer RDLINES Underflow; 0=None, 1=Underflow/Acknowledge
		u8 AckVertexRAMOverflow:1;			//    13: Polygon/Vertex RAM Overflow; 0=None, 1=Overflow/Acknowledge
		u8 RearPlaneMode:1;					//    14: Use clear image, interacts with CLEAR_COLOR (0x4000350) / CLEAR_DEPTH (0x4000354) / CLRIMAGE_OFFSET (0x4000356);
											//        0=Blank, 1=Bitmap
		u8 :1;								//    15: Unused bits
		
		u16 :16;							// 16-31: Unused bits
#else
		u8 EnableFog:1;						//     7: Perform fog rendering, interacts with FOG_COLOR (0x4000358) / FOG_OFFSET (0x400035C) / FOG_TABLE (0x4000360);
											//        0=Disable, 1=Enable
		u8 FogOnlyAlpha:1;					//     6: Apply fog to the alpha channel only, interacts with FOG_COLOR (0x4000358) / FOG_TABLE (0x4000360); 0=Color+Alpha, 1=Alpha
		u8 EnableEdgeMarking:1;				//     5: Perform polygon edge marking, interacts with EDGE_COLOR (0x4000330); 0=Disable, 1=Enable
		u8 EnableAntiAliasing:1;			//     4: Render polygon edges with antialiasing; 0=Disable, 1=Enable
		u8 EnableAlphaBlending:1;			//     3: Perform alpha blending, interacts with POLYGON_ATTR (0x40004A4); 0=Disable, 1=Enable
		u8 EnableAlphaTest:1;				//     2: Perform alpha test, interacts with ALPHA_TEST_REF (0x4000340); 0=Disable, 1=Enable
		u8 PolygonShading:1;				//     1: Polygon shading mode, interacts with POLYGON_ATTR (0x40004A4); 0=Toon Shading, 1=Highlight Shading
		u8 EnableTexMapping:1;				//     0: Apply textures; 0=Disable, 1=Enable
		
		u8 :1;								//    15: Unused bits
		u8 RearPlaneMode:1;					//    14: Use clear image, interacts with CLEAR_COLOR (0x4000350) / CLEAR_DEPTH (0x4000354) / CLRIMAGE_OFFSET (0x4000356);
											//        0=Blank, 1=Bitmap
		u8 AckVertexRAMOverflow:1;			//    13: Polygon/Vertex RAM Overflow; 0=None, 1=Overflow/Acknowledge
		u8 AckColorBufferUnderflow:1;		//    12: Color Buffer RDLINES Underflow; 0=None, 1=Underflow/Acknowledge
		u8 FogShiftSHR:4;					//  8-11: SHR-Divider, interacts with FOG_OFFSET (0x400035C); 0...10
		
		u16 :16;							// 16-31: Unused bits
#endif
	};
} IOREG_DISP3DCNT;							// 0x4000060: 3D engine control (Engine A only)

typedef union
{
	u32 value;
	
	struct
	{
#ifdef LOCAL_LE
		unsigned EVA:5;						//  0- 4: Blending coefficient for SrcA; 0...31 (clamped to 16)
		unsigned :3;						//  5- 7: Unused bits
		
		unsigned EVB:5;						//  8-12: Blending coefficient for SrcB; 0...31 (clamped to 16)
		unsigned :3;						// 13-15: Unused bits
		
		unsigned VRAMWriteBlock:2;			// 16-17: VRAM write target block; 0=Block A, 1=Block B, 2=Block C, 3=Block D
		unsigned VRAMWriteOffset:2;			// 18-19: VRAM write target offset; 0=0KB, 1=32KB, 2=64KB, 3=96KB
		unsigned CaptureSize:2;				// 20-21: Display capture dimensions; 0=128x128, 1=256x64, 2=256x128, 3=256x192
		unsigned :2;						// 22-23: Unused bits
		
		unsigned SrcA:1;					//    24: SrcA target; 0=Current framebuffer, 1=3D render buffer
		unsigned SrcB:1;					//    25: SrcB target;
											//        0=VRAM block, interacts with DISPCNT (0x4000000)
											//        1=Main memory FIFO, interacts with DISP_MMEM_FIFO (0x4000068)
		unsigned VRAMReadOffset:2;			// 26-27: VRAM read target offset; 0=0KB, 1=32KB, 2=64KB, 3=96KB
		unsigned :1;						//    28: Unused bit
		unsigned CaptureSrc:2;				// 29-30: Select capture target; 0=SrcA, 1=SrcB, 2=SrcA+SrcB blend, 3=SrcA+SrcB blend
		unsigned CaptureEnable:1;			//    31: Display capture status; 0=Disable/Ready 1=Enable/Busy
#else
		unsigned :3;						//  5- 7: Unused bits
		unsigned EVA:5;						//  0- 4: Blending coefficient for SrcA; 0...31 (clamped to 16)
		
		unsigned :3;						// 13-15: Unused bits
		unsigned EVB:5;						//  8-12: Blending coefficient for SrcB; 0...31 (clamped to 16)
		
		unsigned :2;						// 22-23: Unused bits
		unsigned CaptureSize:2;				// 20-21: Display capture dimensions; 0=128x128, 1=256x64, 2=256x128, 3=256x192
		unsigned VRAMWriteOffset:2;			// 18-19: VRAM write target offset; 0=0KB, 1=32KB, 2=64KB, 3=96KB
		unsigned VRAMWriteBlock:2;			// 16-17: VRAM write target block; 0=Block A, 1=Block B, 2=Block C, 3=Block D
		
		unsigned CaptureEnable:1;			//    31: Display capture status; 0=Disable/Ready 1=Enable/Busy
		unsigned CaptureSrc:2;				// 29-30: Select capture target; 0=SrcA, 1=SrcB, 2=SrcA+SrcB blend, 3=SrcA+SrcB blend
		unsigned :1;						//    28: Unused bit
		unsigned VRAMReadOffset:2;			// 26-27: VRAM read target offset; 0=0KB, 1=32KB, 2=64KB, 3=96KB
		unsigned SrcB:1;					//    25: SrcB target;
											//        0=VRAM block, interacts with DISPCNT (0x4000000)
											//        1=Main memory FIFO, interacts with DISP_MMEM_FIFO (0x4000068)
		unsigned SrcA:1;					//    24: SrcA target; 0=Current framebuffer, 1=3D render buffer
#endif
	};
	
} IOREG_DISPCAPCNT;							// 0x4000064: Display capture control (Engine A only)

typedef u32 IOREG_DISP_MMEM_FIFO;			// 0x4000068: Main memory FIFO data (Engine A only)

typedef union
{
	u32 value;
	
	struct
	{
#ifdef LOCAL_LE
		u32 Intensity:5;					//  0- 4: Brightness coefficient for increase/decrease brightness; 0...31 (clamped to 16)
		u32 :3;								//  5- 7: Unused bits
		
		u32 :6;								//  8-13: Unused bits
		u32 Mode:2;							// 14-15: Brightness mode; 0=Disable, 1=Increase, 2=Decrease, 3=Reserved
		
		u32 :16;							// 16-31: Unused bits
#else
		u32 :3;								//  5- 7: Unused bits
		u32 Intensity:5;					//  0- 4: Brightness coefficient for increase/decrease brightness; 0...31 (clamped to 16)
		
		u32 Mode:2;							// 14-15: Brightness mode; 0=Disable, 1=Increase, 2=Decrease, 3=Reserved
		u32 :6;								//  8-13: Unused bits
		
		u32 :16;							// 16-31: Unused bits
#endif
	};
} IOREG_MASTER_BRIGHT;						// 0x400x06C: Master brightness effect (Engine A+B)

#include "PACKED.h"
typedef struct
{
	IOREG_DISPCNT			DISPCNT;				// 0x0400x000
	IOREG_DISPSTAT			DISPSTAT;				// 0x04000004
	IOREG_VCOUNT			VCOUNT;					// 0x04000006
	
	union
	{
		IOREG_BGnCNT		BGnCNT[4];				// 0x0400x008
		
		struct
		{
			IOREG_BG0CNT	BG0CNT;					// 0x0400x008
			IOREG_BG1CNT	BG1CNT;					// 0x0400x00A
			IOREG_BG2CNT	BG2CNT;					// 0x0400x00C
			IOREG_BG3CNT	BG3CNT;					// 0x0400x00E
		};
	};
	
	union
	{
		IOREG_BGnOFS		BGnOFS[4];				// 0x0400x010
		
		struct
		{
			IOREG_BG0HOFS	BG0HOFS;				// 0x0400x010
			IOREG_BG0VOFS	BG0VOFS;				// 0x0400x012
			IOREG_BG1HOFS	BG1HOFS;				// 0x0400x014
			IOREG_BG1VOFS	BG1VOFS;				// 0x0400x016
			IOREG_BG2HOFS	BG2HOFS;				// 0x0400x018
			IOREG_BG2VOFS	BG2VOFS;				// 0x0400x01A
			IOREG_BG3HOFS	BG3HOFS;				// 0x0400x01C
			IOREG_BG3VOFS	BG3VOFS;				// 0x0400x01E
		};
	};
	
	union
	{
		IOREG_BGnParameter			BGnParam[2];	// 0x0400x020
		
		struct
		{
			union
			{
				IOREG_BG2Parameter	BG2Param;		// 0x0400x020
				
				struct
				{
					IOREG_BG2PA		BG2PA;			// 0x0400x020
					IOREG_BG2PB		BG2PB;			// 0x0400x022
					IOREG_BG2PC		BG2PC;			// 0x0400x024
					IOREG_BG2PD		BG2PD;			// 0x0400x026
					IOREG_BG2X		BG2X;			// 0x0400x028
					IOREG_BG2Y		BG2Y;			// 0x0400x02C
				};
			};
			
			union
			{
				IOREG_BG3Parameter	BG3Param;		// 0x0400x030
				
				struct
				{
					IOREG_BG3PA		BG3PA;			// 0x0400x030
					IOREG_BG3PB		BG3PB;			// 0x0400x032
					IOREG_BG3PC		BG3PC;			// 0x0400x034
					IOREG_BG3PD		BG3PD;			// 0x0400x036
					IOREG_BG3X		BG3X;			// 0x0400x038
					IOREG_BG3Y		BG3Y;			// 0x0400x03C
				};
			};
		};
	};
	
	IOREG_WIN0H				WIN0H;					// 0x0400x040
	IOREG_WIN1H				WIN1H;					// 0x0400x042
	IOREG_WIN0V				WIN0V;					// 0x0400x044
	IOREG_WIN1V				WIN1V;					// 0x0400x046
	IOREG_WIN0IN			WIN0IN;					// 0x0400x048
	IOREG_WIN1IN			WIN1IN;					// 0x0400x049
	IOREG_WINOUT			WINOUT;					// 0x0400x04A
	IOREG_WINOBJ			WINOBJ;					// 0x0400x04B
	
	IOREG_MOSAIC			MOSAIC;					// 0x0400x04C
	
	IOREG_BLDCNT			BLDCNT;					// 0x0400x050
	IOREG_BLDALPHA			BLDALPHA;				// 0x0400x052
	IOREG_BLDY				BLDY;					// 0x0400x054
	
	u8						unused[10];				// 0x0400x056
	
	IOREG_DISP3DCNT			DISP3DCNT;				// 0x04000060
	IOREG_DISPCAPCNT		DISPCAPCNT;				// 0x04000064
	IOREG_DISP_MMEM_FIFO	DISP_MMEM_FIFO;			// 0x04000068
	
	IOREG_MASTER_BRIGHT		MASTER_BRIGHT;			// 0x0400x06C
} GPU_IOREG;										// 0x04000000, 0x04001000: GPU registers
#include "PACKED_END.h"

enum BlendFunc
{
	NoBlend, Blend, Increase, Decrease
};

enum GPUEngineID
{
	GPUEngineID_Main	= 0,
	GPUEngineID_Sub		= 1
};

/* human readable bitmask names */
#define ADDRESS_STEP_512B	   0x00200
#define ADDRESS_STEP_1KB		0x00400
#define ADDRESS_STEP_2KB		0x00800
#define ADDRESS_STEP_4KB		0x01000
#define ADDRESS_STEP_8KB		0x02000
#define ADDRESS_STEP_16KB	   0x04000
#define ADDRESS_STEP_32KB	   0x08000
#define ADDRESS_STEP_64KB	   0x10000
#define ADDRESS_STEP_128KB	   0x20000
#define ADDRESS_STEP_256KB	   0x40000
#define ADDRESS_STEP_512KB	   0x80000
#define ADDRESS_MASK_256KB	   (ADDRESS_STEP_256KB-1)

#ifdef LOCAL_BE
struct _TILEENTRY
{
/*14*/	unsigned Palette:4;
/*13*/	unsigned VFlip:1;	// VERTICAL FLIP (top<-->bottom)
/*12*/	unsigned HFlip:1;	// HORIZONTAL FLIP (left<-->right)
/* 0*/	unsigned TileNum:10;
};
#else
struct _TILEENTRY
{
/* 0*/	unsigned TileNum:10;
/*12*/	unsigned HFlip:1;	// HORIZONTAL FLIP (left<-->right)
/*13*/	unsigned VFlip:1;	// VERTICAL FLIP (top<-->bottom)
/*14*/	unsigned Palette:4;
};
#endif
typedef union
{
	struct _TILEENTRY bits;
	u16 val;
} TILEENTRY;

struct _ROTOCOORD
{
	u32 Fraction:8;
	s32 Integer:20;
	u32 pad:4;
};
typedef union
{
	struct _ROTOCOORD bits;
	s32 val;
} ROTOCOORD;


/*
	this structure is for color representation,
	it holds 5 meaningful bits per color channel (red,green,blue)
	and	  1 meaningful bit for alpha representation
	this bit can be unused or used for special FX
*/

struct _COLOR { // abgr x555
#ifdef LOCAL_BE
	unsigned alpha:1;    // sometimes it is unused (pad)
	unsigned blue:5;
	unsigned green:5;
	unsigned red:5;
#else
     unsigned red:5;
     unsigned green:5;
     unsigned blue:5;
     unsigned alpha:1;    // sometimes it is unused (pad)
#endif
};
struct _COLORx { // abgr x555
	unsigned bgr:15;
	unsigned alpha:1;	// sometimes it is unused (pad)
};

typedef union
{
	struct _COLOR bits;
	struct _COLORx bitx;
	u16 val;
} COLOR;

struct _COLOR32 { // ARGB
	unsigned :3;
	unsigned blue:5;
	unsigned :3;
	unsigned green:5;
	unsigned :3;
	unsigned red:5;
	unsigned :7;
	unsigned alpha:1;	// sometimes it is unused (pad)
};

typedef union
{
	struct _COLOR32 bits;
	u32 val;
} COLOR32;

#define COLOR_16_32(w,i)	\
	/* doesnt matter who's 16bit who's 32bit */ \
	i.bits.red   = w.bits.red; \
	i.bits.green = w.bits.green; \
	i.bits.blue  = w.bits.blue; \
	i.bits.alpha = w.bits.alpha;



 // (00: Normal, 01: Transparent, 10: Object window, 11: Bitmap)
enum GPU_OBJ_MODE
{
	GPU_OBJ_MODE_Normal = 0,
	GPU_OBJ_MODE_Transparent = 1,
	GPU_OBJ_MODE_Window = 2,
	GPU_OBJ_MODE_Bitmap = 3
};

typedef union
{
	u16 attr[4];
	
	struct
	{
#ifdef LOCAL_BE
		//attr0
		unsigned Shape:2;
		unsigned Depth:1;
		unsigned Mosaic:1;
		unsigned Mode:2;
		unsigned RotScale:2;
		unsigned Y:8;
		
		//attr1
		unsigned Size:2;
		unsigned VFlip:1;
		unsigned HFlip:1;
		unsigned RotScalIndex:3;
		signed X:9;
		
		//attr2
		unsigned PaletteIndex:4;
		unsigned Priority:2;
		unsigned TileIndex:10;
		
		//attr3
		unsigned attr3:16; // Whenever this is used, you will need to explicitly convert endianness.
#else
		//attr0
		unsigned Y:8;
		unsigned RotScale:2;
		unsigned Mode:2;
		unsigned Mosaic:1;
		unsigned Depth:1;
		unsigned Shape:2;
		
		//attr1
		signed X:9;
		unsigned RotScalIndex:3;
		unsigned HFlip:1;
		unsigned VFlip:1;
		unsigned Size:2;
		
		//attr2
		unsigned TileIndex:10;
		unsigned Priority:2;
		unsigned PaletteIndex:4;
		
		//attr3
		unsigned attr3:16; // Whenever this is used, you will need to explicitly convert endianness.
#endif
	};
	
} OAMAttributes;

enum SpriteRenderMode
{
	SpriteRenderMode_Sprite1D = 0,
	SpriteRenderMode_Sprite2D = 1
};

typedef struct
{
	 s16 x;
	 s16 y;
} SpriteSize;

typedef u8 TBlendTable[32][32];

#define NB_PRIORITIES	4
#define NB_BG		4
//this structure holds information for rendering.
typedef struct
{
	u8 PixelsX[256];
	u8 BGs[NB_BG], nbBGs;
	u8 pad[1];
	u16 nbPixelsX;
	//256+8:
	u8 pad2[248];

	//things were slower when i organized this struct this way. whatever.
	//u8 PixelsX[256];
	//int BGs[NB_BG], nbBGs;
	//int nbPixelsX;
	////<-- 256 + 24
	//u8 pad2[256-24];
} itemsForPriority_t;
#define MMU_ABG		0x06000000
#define MMU_BBG		0x06200000
#define MMU_AOBJ	0x06400000
#define MMU_BOBJ	0x06600000
#define MMU_LCDC	0x06800000

enum GPULayerID
{
	GPULayerID_BG0					= 0,
	GPULayerID_BG1					= 1,
	GPULayerID_BG2					= 2,
	GPULayerID_BG3					= 3,
	GPULayerID_OBJ					= 4,
	
	GPULayerID_None					= 5
};

enum BGType
{
	BGType_Invalid					= 0,
	BGType_Text						= 1,
	BGType_Affine					= 2,
	BGType_Large8bpp				= 3,
	
	BGType_AffineExt				= 4,
	BGType_AffineExt_256x16			= 5,
	BGType_AffineExt_256x1			= 6,
	BGType_AffineExt_Direct			= 7
};

enum GPUDisplayMode
{
	GPUDisplayMode_Off				= 0,
	GPUDisplayMode_Normal			= 1,
	GPUDisplayMode_VRAM				= 2,
	GPUDisplayMode_MainMemory		= 3
};

enum GPUMasterBrightMode
{
	GPUMasterBrightMode_Disable		= 0,
	GPUMasterBrightMode_Up			= 1,
	GPUMasterBrightMode_Down		= 2,
	GPUMasterBrightMode_Reserved	= 3
	
};

enum GPULayerType
{
	GPULayerType_3D					= 0,
	GPULayerType_BG					= 1,
	GPULayerType_OBJ				= 2
};

enum NDSDisplayID
{
	NDSDisplayID_Main				= 0,
	NDSDisplayID_Touch				= 1
};

struct DISPCAPCNT_parsed
{
	enum CAPX {
		_128, _256
	} capx;
	
	u8 EVA;
	u8 EVB;
	u8 readOffset;
	u16 capy;
};

typedef struct
{
	u8 blockIndexDisplayVRAM;
	bool isBlockUsed[4];
} VRAM3DUsageProperties;

typedef struct
{
	bool isCustomSizeRequested;			// true - The user requested a custom size; false - The user requested the native size
	size_t customWidth;					// The custom buffer width, measured in pixels
	size_t customHeight;				// The custom buffer height, measured in pixels
	u16 *masterCustomBuffer;			// Pointer to the head of the master custom buffer.
	u16 *masterNativeBuffer;			// Pointer to the head of the master native buffer.
	
	u16 *customBuffer[2];				// Pointer to a display's custom size framebuffer
	u16	*nativeBuffer[2];				// Pointer to a display's native size framebuffer
	
	bool didPerformCustomRender[2];		// true - The display performed a custom-sized render; false - The display performed a native-sized render
	size_t renderedWidth[2];			// The display rendered at this width, measured in pixels
	size_t renderedHeight[2];			// The display rendered at this height, measured in pixels
	u16 *renderedBuffer[2];				// The display rendered to this buffer
} NDSDisplayInfo;

#define VRAM_NO_3D_USAGE 0xFF

class GPUEngineBase
{
protected:
	static CACHE_ALIGN u16 _fadeInColors[17][0x8000];
	static CACHE_ALIGN u16 _fadeOutColors[17][0x8000];
	static CACHE_ALIGN u8 _blendTable555[17][17][32][32];
	
	static const CACHE_ALIGN SpriteSize _sprSizeTab[4][4];
	static const CACHE_ALIGN BGType _mode2type[8][4];
	static const CACHE_ALIGN u16 _sizeTab[8][4][2];
	static const CACHE_ALIGN u8 _winEmpty[GPU_FRAMEBUFFER_NATIVE_WIDTH];
	
	static struct MosaicLookup {
		
		struct TableEntry {
			u8 begin, trunc;
		} table[16][256];
		
		MosaicLookup() {
			for(int m=0;m<16;m++)
				for(int i=0;i<256;i++) {
					int mosaic = m+1;
					TableEntry &te = table[m][i];
					te.begin = (i%mosaic==0);
					te.trunc = i/mosaic*mosaic;
				}
		}
		
		TableEntry *width, *height;
		int widthValue, heightValue;
		
	} _mosaicLookup;
	
	CACHE_ALIGN u16 _sprColor[GPU_FRAMEBUFFER_NATIVE_WIDTH];
	CACHE_ALIGN u8 _sprAlpha[GPU_FRAMEBUFFER_NATIVE_WIDTH];
	CACHE_ALIGN u8 _sprType[GPU_FRAMEBUFFER_NATIVE_WIDTH];
	CACHE_ALIGN u8 _sprPrio[GPU_FRAMEBUFFER_NATIVE_WIDTH];
	CACHE_ALIGN u8 _sprWin[GPU_FRAMEBUFFER_NATIVE_WIDTH];
	
	bool _enableDebug;
	bool _enableLayer[5];
	itemsForPriority_t _itemsForPriority[NB_PRIORITIES];
	
	struct MosaicColor {
		u16 bg[4][256];
		struct Obj {
			u16 color;
			u8 alpha;
			u8 opaque;
		} obj[256];
	} _mosaicColors;
	
	GPUEngineID _engineID;
	GPU_IOREG *_IORegisterMap;
	u16 *_paletteBG;
	u16 *_paletteOBJ;
	OAMAttributes *_oamList;
	u32 _sprMem;
	
	u8 _bgPrio[5];
	bool _bg0HasHighestPrio;
	
	u8 _sprBoundary;
	u8 _sprBMPBoundary;
	u8 _sprBMPMode;
	u32 _sprEnable;
	
	u16 *_currentFadeInColors;
	u16 *_currentFadeOutColors;
	
	bool _blend1;
	bool _blend2[8];
	
	TBlendTable *_blendTable;
	
	u32 _BG_bmp_large_ram[4];
	u32 _BG_bmp_ram[4];
	u32 _BG_tile_ram[4];
	u32 _BG_map_ram[4];
	
	BGType _BGTypes[4];
	
	GPUDisplayMode _dispMode;
	u8 _vramBlock;
	
	CACHE_ALIGN u8 _sprNum[256];
	CACHE_ALIGN u8 _h_win[2][GPU_FRAMEBUFFER_NATIVE_WIDTH];
	const u8 *_curr_win[2];
	
	NDSDisplayID _targetDisplayID;
	u16 *_VRAMaddrNative;
	u16 *_VRAMaddrCustom;
		
	int _finalColorBckFuncID;
	int _finalColor3DFuncID;
	int _finalColorSpriteFuncID;
	
	SpriteRenderMode _spriteRenderMode;
	GPUMasterBrightMode _masterBrightMode;
	u32 _masterBrightFactor;
	bool _isMasterBrightFullIntensity;
	
	u32 _currentScanline;
	u16 *_currentDstColor;
	u16 *_workingDstColorBuffer;
	u8 *_dstLayerID;
	bool _needUpdateWINH[2];
	
	u8 _WIN0H0;
	u8 _WIN0H1;
	u8 _WIN0V0;
	u8 _WIN0V1;
	
	u8 _WIN1H0;
	u8 _WIN1H1;
	u8 _WIN1V0;
	u8 _WIN1V1;
	
	u8 _WININ0;
	bool _WININ0_SPECIAL;
	u8 _WININ1;
	bool _WININ1_SPECIAL;
	
	u8 _WINOUT;
	bool _WINOUT_SPECIAL;
	u8 _WINOBJ;
	bool _WINOBJ_SPECIAL;
	
	u8 _WIN0_ENABLED;
	u8 _WIN1_ENABLED;
	u8 _WINOBJ_ENABLED;
	
	u16 _BLDCNT;
	u8 _BLDALPHA_EVA;
	u8 _BLDALPHA_EVB;
	u8 _BLDY_EVY;
	
	void _InitLUTs();
	void _Reset_Base();
	
	void _MosaicSpriteLinePixel(const size_t x, u16 l, u16 *dst, u8 *dst_alpha, u8 *typeTab, u8 *prioTab);
	void _MosaicSpriteLine(u16 l, u16 *dst, u8 *dst_alpha, u8 *typeTab, u8 *prioTab);
	
	template<rot_fun fun, bool WRAP> void _rot_scale_op(const IOREG_BGnParameter &param, const u16 LG, const s32 wh, const s32 ht, const u32 map, const u32 tile, const u16 *pal);
	template<GPULayerID LAYERID, rot_fun fun> void _apply_rot_fun(const IOREG_BGnParameter &param, const u16 LG, const u32 map, const u32 tile, const u16 *pal);
	
	template<GPULayerID LAYERID, bool MOSAIC, bool ISCUSTOMRENDERINGNEEDED> void _LineLarge8bpp();
	template<GPULayerID LAYERID, bool MOSAIC, bool ISCUSTOMRENDERINGNEEDED> void _RenderLine_TextBG(u16 XBG, u16 YBG, u16 LG);
	
	template<GPULayerID LAYERID, bool MOSAIC, bool ISCUSTOMRENDERINGNEEDED> void _RotBG2(const IOREG_BGnParameter &param, const u16 LG);
	template<GPULayerID LAYERID, bool MOSAIC, bool ISCUSTOMRENDERINGNEEDED> void _ExtRotBG2(const IOREG_BGnParameter &param, const u16 LG);
	
	template<GPULayerID LAYERID, bool MOSAIC, bool ISCUSTOMRENDERINGNEEDED> void _LineText();
	template<GPULayerID LAYERID, bool MOSAIC, bool ISCUSTOMRENDERINGNEEDED> void _LineRot();
	template<GPULayerID LAYERID, bool MOSAIC, bool ISCUSTOMRENDERINGNEEDED> void _LineExtRot();
	
	template<int WIN_NUM> u8 _WithinRect(const size_t x) const;
	template <GPULayerID LAYERID> void _RenderLine_CheckWindows(const size_t srcX, bool &draw, bool &effect) const;
	
	template<bool ISCUSTOMRENDERINGNEEDED> void _RenderLine_Layer(const u16 l, u16 *dstColorLine, const size_t dstLineWidth, const size_t dstLineCount);
	template<bool ISCUSTOMRENDERINGNEEDED> void _RenderLine_MasterBrightness(u16 *dstColorLine, const size_t dstLineWidth, const size_t dstLineCount);
	
	template<size_t WIN_NUM> void _UpdateWINH();
	template<size_t WIN_NUM> void _SetupWindows();
	template<GPULayerID LAYERID, bool MOSAIC, bool ISCUSTOMRENDERINGNEEDED> void _ModeRender();
	
	template<GPULayerID LAYERID, bool BACKDROP, BlendFunc FUNC, bool WINDOW>
	FORCEINLINE FASTCALL bool _master_setFinalBGColor(const size_t srcX, const size_t dstX, const u16 *dstColorLine, const u8 *dstLayerIDLine, u16 &outColor);
	
	template<BlendFunc FUNC, bool WINDOW>
	FORCEINLINE FASTCALL void _master_setFinal3dColor(const size_t srcX, const size_t dstX, u16 *dstColorLine, u8 *dstLayerIDLine, const FragmentColor src);
	
	template<BlendFunc FUNC, bool WINDOW>
	FORCEINLINE FASTCALL void _master_setFinalOBJColor(const size_t srcX, const size_t dstX, u16 *dstColorLine, u8 *dstLayerIDLine, const u16 src, const u8 alpha, const u8 type);
	
	template<GPULayerID LAYERID, bool BACKDROP, int FUNCNUM> void _SetFinalColorBG(const size_t srcX, const size_t dstX, u16 *dstColorLine, u8 *dstLayerIDLine, u16 src);
	void _SetFinalColor3D(const size_t srcX, const size_t dstX, u16 *dstColorLine, u8 *dstLayerIDLine, const FragmentColor src);
	void _SetFinalColorSprite(const size_t srcX, const size_t dstX, u16 *dstColorLine, u8 *dstLayerIDLine, const u16 src, const u8 alpha, const u8 type);
	
	u16 _FinalColorBlend(const u16 colA, const u16 colB);
	FORCEINLINE u16 _FinalColorBlendFunc(const u16 colA, const u16 colB, const TBlendTable *blendTable);
	
	void _RenderSpriteBMP(const u8 spriteNum, const u16 l, u16 *dst, const u32 srcadr, u8 *dst_alpha, u8 *typeTab, u8 *prioTab, const u8 prio, const size_t lg, size_t sprX, size_t x, const s32 xdir, const u8 alpha);
	void _RenderSprite256(const u8 spriteNum, const u16 l, u16 *dst, const u32 srcadr, const u16 *pal, u8 *dst_alpha, u8 *typeTab, u8 *prioTab, const u8 prio, const size_t lg, size_t sprX, size_t x, const s32 xdir, const u8 alpha);
	void _RenderSprite16(const u16 l, u16 *dst, const u32 srcadr, const u16 *pal, u8 *dst_alpha, u8 *typeTab, u8 *prioTab, const u8 prio, const size_t lg, size_t sprX, size_t x, const s32 xdir, const u8 alpha);
	void _RenderSpriteWin(const u8 *src, const bool col256, const size_t lg, size_t sprX, size_t x, const s32 xdir);
	bool _ComputeSpriteVars(const OAMAttributes &spriteInfo, const u16 l, SpriteSize &sprSize, s32 &sprX, s32 &sprY, s32 &x, s32 &y, s32 &lg, s32 &xdir);
	
	u32 _SpriteAddressBMP(const OAMAttributes &spriteInfo, const SpriteSize sprSize, const s32 y);
	
	template<SpriteRenderMode MODE> void _SpriteRenderPerform(u16 *dst, u8 *dst_alpha, u8 *typeTab, u8 *prioTab);
	
public:
	GPUEngineBase();
	virtual ~GPUEngineBase();
	
	virtual void Reset();
	void ResortBGLayers();
	void SetMasterBrightness(const u16 val);
	void SetupFinalPixelBlitter();
	
	void SetVideoProp(const u32 ctrlBits);
	template<GPULayerID LAYERID> void SetBGProp(const u16 ctrlBits);
	
	template<bool ISCUSTOMRENDERINGNEEDED> void RenderLine(const u16 l, bool skip);
	
	u16 BGSize[4][2];
	u8 BGExtPalSlot[4];
	
	bool isCustomRenderingNeeded;
	bool is3DEnabled;
	u8 vramBGLayer;
	u8 vramBlockBGIndex;
	u8 vramBlockOBJIndex;
	
	u16 *customBuffer;
	u16 *nativeBuffer;
	size_t renderedWidth;
	size_t renderedHeight;
	u16 *renderedBuffer;
	
	struct AffineInfo {
		AffineInfo() : x(0), y(0) {}
		u32 x, y;
	} affineInfo[2];
	
	const GPU_IOREG& GetIORegisterMap() const;
	
	bool GetIsMasterBrightFullIntensity() const;
	
	bool GetEnableState();
	void SetEnableState(bool theState);
	bool GetLayerEnableState(const size_t layerIndex);
	void SetLayerEnableState(const size_t layerIndex, bool theState);

	bool GetDebugState();
	void SetDebugState(bool theState);
	
	template<GPULayerID LAYERID, bool BACKDROP, int FUNCNUM, bool ISCUSTOMRENDERINGNEEDED, bool USECUSTOMVRAM> FORCEINLINE void ____setFinalColorBck(const u16 color, const size_t srcX);
	template<GPULayerID LAYERID, bool MOSAIC, bool BACKDROP, int FUNCNUM, bool ISCUSTOMRENDERINGNEEDED, bool USECUSTOMVRAM> FORCEINLINE void ___setFinalColorBck(u16 color, const size_t srcX, const bool opaque);
	template<GPULayerID LAYERID, bool MOSAIC, bool BACKDROP, bool ISCUSTOMRENDERINGNEEDED> FORCEINLINE void __setFinalColorBck(u16 color, const size_t srcX, const bool opaque);
	
	void UpdateVRAM3DUsageProperties_BGLayer(const size_t bankIndex, VRAM3DUsageProperties &outProperty);
	void UpdateVRAM3DUsageProperties_OBJLayer(const size_t bankIndex, VRAM3DUsageProperties &outProperty);
	
	template<GPULayerID LAYERID, int SET_XY> void setAffineStart(u32 val);
	template<GPULayerID LAYERID, int SET_XY, bool HIWORD> void setAffineStartWord(u16 val);
	template<GPULayerID LAYERID, int SET_XY> u32 getAffineStart();
	template<GPULayerID LAYERID, int SET_XY> void refreshAffineStartRegs();
	
	void SpriteRender(u16 *dst, u8 *dst_alpha, u8 *typeTab, u8 *prioTab);
	void SpriteRenderDebug(const size_t targetScanline, u16 *dst, u8 *dst_alpha, u8 *typeTab, u8 *prioTab);
	void ModeRenderDebug(const size_t targetScanline, const GPULayerID layerID, u16 *dstLineColor);

	template<bool ISCUSTOMRENDERINGNEEDED> void HandleDisplayModeOff(u16 *dstColorLine, const size_t l, const size_t dstLineWidth, const size_t dstLineCount);
	template<bool ISCUSTOMRENDERINGNEEDED> void HandleDisplayModeNormal(u16 *dstColorLine, const size_t l, const size_t dstLineWidth, const size_t dstLineCount);
	template<bool ISCUSTOMRENDERINGNEEDED> void HandleDisplayModeVRAM(u16 *dstColorLine, const size_t l, const size_t dstLineWidth, const size_t dstLineCount);
	template<bool ISCUSTOMRENDERINGNEEDED> void HandleDisplayModeMainMemory(u16 *dstColorLine, const size_t l, const size_t dstLineWidth, const size_t dstLineCount);

	void UpdateBLDALPHA();
	void SetBLDALPHA(const u16 val);
	void SetBLDALPHA_EVA(const u8 val);
	void SetBLDALPHA_EVB(const u8 val);
	
	// Blending
	void SetBLDCNT_LOW(const u8 val);
	void SetBLDCNT_HIGH(const u8 val);
	void SetBLDCNT(const u16 val);
	void SetBLDY_EVY(const u8 val);
	
	void SetWIN0_H(const u16 val);
	void SetWIN0_H0(const u8 val);
	void SetWIN0_H1(const u8 val);
	
	void SetWIN0_V(const u16 val);
	void SetWIN0_V0(const u8 val);
	void SetWIN0_V1(const u8 val);
	
	void SetWIN1_H(const u16 val);
	void SetWIN1_H0(const u8 val);
	void SetWIN1_H1(const u8 val);
	
	void SetWIN1_V(const u16 val);
	void SetWIN1_V0(const u8 val);
	void SetWIN1_V1(const u8 val);
	
	void SetWININ(const u16 val);
	void SetWININ0(const u8 val);
	void SetWININ1(const u8 val);
	
	void SetWINOUT16(const u16 val);
	void SetWINOUT(const u8 val);
	void SetWINOBJ(const u8 val);
	
	int GetFinalColorBckFuncID() const;
	void SetFinalColorBckFuncID(int funcID);

	NDSDisplayID GetDisplayByID() const;
	void SetDisplayByID(const NDSDisplayID theDisplayID);
	
	GPUEngineID GetEngineID() const;
	
	virtual void SetCustomFramebufferSize(size_t w, size_t h);
	void BlitNativeToCustomFramebuffer();
	
	void REG_DISPx_pack_test();
};

class GPUEngineA : public GPUEngineBase
{
protected:
	FragmentColor *_3DFramebufferRGBA6665;
	u16 *_3DFramebufferRGBA5551;
	
	template<bool ISCUSTOMRENDERINGNEEDED> void _RenderLine_Layer(const u16 l, u16 *dstColorLine, const size_t dstLineWidth, const size_t dstLineCount);
	template<bool ISCUSTOMRENDERINGNEEDED, size_t CAPTURELENGTH> void _RenderLine_DisplayCapture(const u16 l);
	void _RenderLine_DispCapture_FIFOToBuffer(u16 *fifoLineBuffer);
	
	template<int SOURCESWITCH, size_t CAPTURELENGTH, bool CAPTUREFROMNATIVESRC, bool CAPTURETONATIVEDST>
	void _RenderLine_DispCapture_Copy(const u16 *__restrict src, u16 *__restrict dst, const size_t captureLengthExt, const size_t captureLineCount);
	
	u16 _RenderLine_DispCapture_BlendFunc(const u16 srcA, const u16 srcB, const u8 blendEVA, const u8 blendEVB);
	
#ifdef ENABLE_SSE2
	__m128i _RenderLine_DispCapture_BlendFunc_SSE2(__m128i &srcA, __m128i &srcB, const __m128i &blendEVA, const __m128i &blendEVB);
#endif
	
	template<bool CAPTUREFROMNATIVESRCA, bool CAPTUREFROMNATIVESRCB>
	void _RenderLine_DispCapture_BlendToCustomDstBuffer(const u16 *__restrict srcA, const u16 *__restrict srcB, u16 *__restrict dst, const u8 blendEVA, const u8 blendEVB, const size_t length, size_t l);
	
	template<size_t CAPTURELENGTH, bool CAPTUREFROMNATIVESRCA, bool CAPTUREFROMNATIVESRCB, bool CAPTURETONATIVEDST>
	void _RenderLine_DispCapture_Blend(const u16 *__restrict srcA, const u16 *__restrict srcB, u16 *__restrict dst, const size_t captureLengthExt, const size_t l);
	
public:
	DISPCAPCNT_parsed dispCapCnt;
	
	GPUEngineA();
	~GPUEngineA();
	
	virtual void Reset();
	void SetDISPCAPCNT(u32 val);
	GPUDisplayMode GetDisplayMode() const;
	u8 GetVRAMBlock() const;
	FragmentColor* Get3DFramebufferRGBA6665() const;
	u16* Get3DFramebufferRGBA5551() const;
	virtual void SetCustomFramebufferSize(size_t w, size_t h);
		
	template<bool ISCUSTOMRENDERINGNEEDED> void RenderLine(const u16 l, bool skip);
};

class GPUEngineB : public GPUEngineBase
{
protected:
	template<bool ISCUSTOMRENDERINGNEEDED> void _RenderLine_Layer(const u16 l, u16 *dstColorLine, const size_t dstLineWidth, const size_t dstLineCount);
	
public:
	GPUEngineB();
	
	virtual void Reset();
	template<bool ISCUSTOMRENDERINGNEEDED> void RenderLine(const u16 l, bool skip);
};

class NDSDisplay
{
private:
	NDSDisplayID _ID;
	GPUEngineBase *_gpu;
	
public:
	NDSDisplay();
	NDSDisplay(const NDSDisplayID displayID);
	NDSDisplay(const NDSDisplayID displayID, GPUEngineBase *theEngine);
	
	GPUEngineBase* GetEngine();
	void SetEngine(GPUEngineBase *theEngine);
	
	GPUEngineID GetEngineID();
	void SetEngineByID(const GPUEngineID theID);
};

class GPUSubsystem
{
private:
	GPUEngineA *_engineMain;
	GPUEngineB *_engineSub;
	NDSDisplay *_displayMain;
	NDSDisplay *_displayTouch;
	
	bool _willAutoBlitNativeToCustomBuffer;
	VRAM3DUsageProperties _VRAM3DUsage;
	u16 *_customVRAM;
	u16 *_customVRAMBlank;
	
	CACHE_ALIGN u16 _nativeFramebuffer[GPU_FRAMEBUFFER_NATIVE_WIDTH * GPU_FRAMEBUFFER_NATIVE_HEIGHT * 2];
	u16 *_customFramebuffer;
	
	NDSDisplayInfo _displayInfo;
	
public:
	GPUSubsystem();
	~GPUSubsystem();
	
	void Reset();
	VRAM3DUsageProperties& GetVRAM3DUsageProperties();
	const NDSDisplayInfo& GetDisplayInfo(); // Frontends need to call this whenever they need to read the video buffers from the emulator core
	void SetDisplayDidCustomRender(NDSDisplayID displayID, bool theState);
	
	GPUEngineA* GetEngineMain();
	GPUEngineB* GetEngineSub();
	NDSDisplay* GetDisplayMain();
	NDSDisplay* GetDisplayTouch();
	
	u16* GetNativeFramebuffer();
	u16* GetNativeFramebuffer(const NDSDisplayID theDisplayID);
	u16* GetCustomFramebuffer();
	u16* GetCustomFramebuffer(const NDSDisplayID theDisplayID);
	
	u16* GetCustomVRAMBuffer();
	u16* GetCustomVRAMBlankBuffer();
	
	size_t GetCustomFramebufferWidth() const;
	size_t GetCustomFramebufferHeight() const;
	void SetCustomFramebufferSize(size_t w, size_t h);
	
	void UpdateVRAM3DUsageProperties();
	
	// Normally, the GPUs will automatically blit their native buffers to the master
	// framebuffer at the end of V-blank so that all rendered graphics are contained
	// within a single common buffer. This is necessary for when someone wants to read
	// the NDS framebuffers, but the reader can only read a single buffer at a time.
	// Certain functions, such as taking screenshots, as well as many frontends running
	// the NDS video displays, require that they read from only a single buffer.
	//
	// However, if SetWillAutoBlitNativeToCustomBuffer() is passed "false", then the
	// frontend becomes responsible for calling GetDisplayInfo() and reading the native
	// and custom buffers properly for each display. If a single buffer is still needed
	// for certain cases, then the frontend must manually call
	// GPUEngineBase::BlitNativeToCustomFramebuffer() for each GPU before reading the
	// master framebuffer.
	bool GetWillAutoBlitNativeToCustomBuffer() const;
	void SetWillAutoBlitNativeToCustomBuffer(const bool willAutoBlit);
	
	void RenderLine(const u16 l, bool skip = false);
	void ClearWithColor(const u16 colorBGRA5551);
};

extern GPUSubsystem *GPU;
extern MMU_struct MMU;

inline FragmentColor MakeFragmentColor(const u8 r, const u8 g, const u8 b, const u8 a)
{
	FragmentColor ret;
	ret.r = r; ret.g = g; ret.b = b; ret.a = a;
	return ret;
}

#endif
