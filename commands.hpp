/*
 * Commands.hpp
 *
 *  Created on: 19 мая 2026 г.
 *      Author: Oleksiy
 */

#ifndef ST7735_COMMANDS_HPP_
#define ST7735_COMMANDS_HPP_


namespace nikolex::st7735 {
	enum COMMAND{
		NOP			= 0x00,
		SWRESET		= 0x01,
		RDDID		= 0x04,
		RDDST		= 0x09,
		RDDPM		= 0x0A,
		RDDMADCTL 	= 0x0B,
		RDDCOLMOD 	= 0x0C,
		RDDIM		= 0x0D,
		RDDSM 		= 0x0E,
		RDDSDR 		= 0x0F,
		SLPIN 		= 0x10,
		SLPOUT 		= 0x11,
		PTLON 		= 0x12,
		NORON 		= 0x13,
		IVNOFF 		= 0x20,
		IVNON 		= 0x21,
		GAMSET 		= 0x26,
		DISPOFF 	= 0x28,
		DISPON 		= 0x29,
		CASET 		= 0x2A,
		RASET 		= 0x2B,
		RAMWR 		= 0x2C,
		RGBSET 		= 0x2D,
		RAMRD 		= 0x2E,
		PTLAR 		= 0x30,
		SCRLAR 		= 0x33,
		TEOFF 		= 0x34,
		TEON 		= 0x35,
		MADCTL 		= 0x36,
		VSCSAD 		= 0x37,
		IDMOFF 		= 0x38,
		IDMON 		= 0x39,
		COLMOD 		= 0x3A,
		RDID1 		= 0xDA,
		RDID2 		= 0xDB,
		RDID3 		= 0xDC,

		FRMCTR1 	= 0xB1,
		FRMCTR2 	= 0xB2,
		FRMCTR3 	= 0xB3,
		INVCTR 		= 0xB4,
		PWCTR1 		= 0xC0,
		PWCTR2 		= 0xC1,
		PWCTR3 		= 0xC2,
		PWCTR4 		= 0xC3,
		PWCTR5 		= 0xC4,
		VMCTR1 		= 0xC5,
		VMOFCTR 	= 0xC7,
		WRID2 		= 0xD1,
		WRID3 		= 0xD2,
		NVFCTR1 	= 0xD9,
		NVFCTR2 	= 0xDE,
		NVFCTR3 	= 0xDF,
		GMCTRP1 	= 0xE0,
		GMCTRN1 	= 0xE1,
		GCV 		= 0xFC
	};
}



#endif /* ST7735_COMMANDS_HPP_ */
