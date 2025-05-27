/*
 * st7735.h
 *
 *  Created on: May 23, 2025
 *      Author: Oleksiy
 */

#ifndef ST7735_ST7735_H_
#define ST7735_ST7735_H_

#include "../interfaces.h"
#include "./stack.h"



using namespace st7735;


class ST7735 {
public:
	struct DrawImageContext_t {
		uint8_t *data;
		uint32_t dataSize;
	};

	struct Context_t{
		uint32_t value[4];
	};

	struct Event_t{
		ST7735 * self;
		void * userData;
		Context_t context;
	};

	struct StackItem_t{
		Context_t context;
		void * userData;
		void (*callback)(const Event_t&);
	};

	enum VerticalRefreshOrder_t{
		REFRESH_TOP_TO_BOTTOM,
		REFRESH_BOTTOM_TO_TOP
	};

	typedef void(* user_callback_t)(const Event_t&);

	enum HorisontalRefreshOrder_t{
		REFRESH_LEFT_TO_RIGHT,
		REFRESH_RIGHT_TO_LEFT
	};

	enum Command_t{
		COMMAND_NOP,
		COMMAND_SWRESET,
		COMMAND_RDDID			= 0x04,
		COMMAND_RDDST			= 0x09,
		COMMAND_SLPIN			= 0x10,
		COMMAND_SLPOUT,
		COMMAND_PTLON,
		COMMAND_NORON,
		COMMAND_INVON			= 0x21,
		COMMAND_DISPOFF			= 0x28,
		COMMAND_DISPON			= 0x29,
		COMMAND_CASET,
		COMMAND_RASET,
		COMMAND_RAMWR,
		COMMAND_MADCTL			= 0x36,
		COMMAND_COLMOD			= 0x3A
	};

	enum MADCTL_Register_t{
		MADCTL_MH 		= 2,
		MADCTL_RGB,
		MADCTL_ML,
		MADCTL_MV,
		MADCTL_MX,
		MADCTL_MY
	};

	enum ColorDepth_t{
		COLOR_DEPTH_RGB_444 	= 3,
		COLOR_DEPTH_RGB_565		= 5,
		COLOR_DEPTH_RGB_666		= 6
	};

	enum Size_t{
		DISPLAY_SIZE_160_x_80 = 1
	};

	enum Orientation_t{
		ORIENTATION_VERTICAL,
		ORIENTATION_HORISONTAL
	};

	enum ColorOrder_t{
		COLOR_ORDER_RGB,
		COLOR_ORDER_BGR
	};

	enum BusyBehavior_t{
		BUSY_BEHAVIOR_SKIP,
		BUSY_BEHAVIOR_BLOCK
	};


private:
	Transmitter * transmitter;
	OutputPin * DCpin;
	OutputPin * RESpin;
	function<void(uint32_t)> delay;

	uint16_t displaySize[2] = {132, 162};

	uint16_t fullWidth;
	uint16_t fullHeight;

	uint16_t width;
	uint16_t height;

	uint8_t data[3] = {0xFF, 0xFF, 0xFF};
	uint32_t counter;

	DrawImageContext_t drawCtx;

	HorisontalRefreshOrder_t horisontalRefreshOrder;
	VerticalRefreshOrder_t verticalRefreshOrder;
	ColorDepth_t colorDepth;
	Orientation_t orientation;
	ColorOrder_t colorOrder;
	bool x_mirror;
	bool y_mirror;
	uint8_t offset_x;
	uint8_t offset_y;

	volatile bool busyState;
	BusyBehavior_t busyBehavior;


	Stack<StackItem_t, 5> stack;
//	EventService eventService;

	void finish();
	void transmit(uint8_t *, uint32_t);
	void writeCommand(uint8_t command);
	void writeData(uint8_t * data, uint32_t data_size);
	void delayAndFinish(uint32_t ms){
//		delay(ms);
		finish();
	}

	void createStackItem(void(*)(const Event_t&) = nullptr, void * = nullptr, const Context_t * = nullptr);
	uint8_t computeMADCTL();
	uint16_t getSmallerSize();
	uint16_t getLargerSize();

	void initHandler1();

	void setMADCTLHandlder1(uint8_t);
	void setMADCTLHandlder2(uint8_t);
	void setColorDepthHandler(ColorDepth_t);
	void setAddressWindowHandler(uint8_t, uint8_t);
	void clearScreenHandler1();
	void clearScreenHandler2();
	void clearScreenHandler3();
	void setOrientationHandler(Orientation_t);

public:

	ST7735(uint16_t, uint16_t, Transmitter *, OutputPin *, OutputPin *, function<void(uint32_t)>);


	void setBusyBehavior(BusyBehavior_t);

	void softwareReset(user_callback_t = nullptr, void * = nullptr);
	void setSleepInMode(user_callback_t = nullptr, void * = nullptr);
	void setSleepOutMode(void(*)(const Event_t&) = nullptr, void * = nullptr);
	void setDisplayOnMode(user_callback_t = nullptr, void * = nullptr);
	void setDisplayOffMode(user_callback_t = nullptr, void * = nullptr);
	void setDsplayInversionOn(user_callback_t = nullptr, void * = nullptr);
	void clearScreen(user_callback_t = nullptr, void * = nullptr);
	void setColorDepth(ColorDepth_t, user_callback_t = nullptr, void * = nullptr );
	void setMADCTL(uint8_t, user_callback_t = nullptr, void * = nullptr);
	void setPixelSequence(uint8_t *, uint32_t, user_callback_t = nullptr, void * = nullptr);
	void setColumnAddressWindow(uint8_t, uint8_t, user_callback_t = nullptr, void * = nullptr);
	void setRawAddressWindow(uint8_t, uint8_t, user_callback_t = nullptr, void * = nullptr);
	void setAddressWindow(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, user_callback_t = nullptr, void * = nullptr);
	void setOrientation(Orientation_t, user_callback_t = nullptr, void * = nullptr);
	void setColorOrder(ColorOrder_t, user_callback_t = nullptr, void * = nullptr);
	void setMirrorX(user_callback_t = nullptr, void * = nullptr);
	void setMirrorY(user_callback_t = nullptr, void * = nullptr);
	void drawImage(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t*, uint32_t, user_callback_t = nullptr, void * = nullptr);
};





#endif /* ST7735_ST7735_H_ */
