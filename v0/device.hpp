/*
 * device.hpp
 *
 *  Created on: 19 мая 2026 г.
 *      Author: Oleksiy
 */

#ifndef ST7735_V0_DEVICE_HPP_
#define ST7735_V0_DEVICE_HPP_

#define CHAIN_SIZE 15
#define TASK_SIZE 5


#include "chained_device.hpp"
#include "spi_transmit_only.hpp"
#include "output_pin.hpp"
#include "commands.hpp"





namespace nikolex::st7735::v0 {


	enum MODEL {
		ST7735s_160x80,
	};


	struct DisplayParams {
		MODEL model;
		uint16_t width;
		uint16_t height;
		uint16_t offset_x;
		uint16_t offset_y;
	};



	class Device;

	struct Transaction {
		uint8_t cmd;
		uint8_t* params;
		uint32_t paramSize;
		uint32_t delayAfterMs;
	};


	struct WriteCommandEventData {
		uint8_t cmd;
	};

	struct WriteDataEventData {
		uint8_t* params;
		uint32_t paramSize;
	};

	struct SetAddressWindowEventData {
		uint8_t column[4];
		uint8_t raw[4];
	};


	struct DrawEventData {
		uint16_t x;
		uint16_t y;
		uint16_t width;
		uint16_t height;
		uint8_t* map;
		uint32_t size;
	};

	union EventData {
		WriteCommandEventData writeCommandEventData;
		WriteDataEventData writeDataEventData;
		Transaction transactionEventData;
		SetAddressWindowEventData setAddressWindowEventData;
		DrawEventData drawEventData;
	};

	struct Context {
		Device* device;
		EventData evtData;
	};







class Device : public ChainedDevice::v1::Device<Device, Context, 10, 5> {



	SpiTransmitOnly* spi;
	OutputPin* resPin;
	OutputPin* dcPin;
	OutputPin* csPin;

	bool isCompleteRequestActive;

	uint32_t counter = 0;
	uint32_t nextCounterValue = 0;

	volatile bool busy;

	uint16_t width;
	uint16_t height;
	uint16_t offset_x;
	uint16_t offset_y;

	void attachDisplayParams(MODEL mod);

public:


	Device(
			SpiTransmitOnly* spiDriver,
			OutputPin* resPinDriver,
			OutputPin* dcPinDriver,
			OutputPin* csPinDriver,
			MODEL model
		);

	void init();

	void timerInc(uint32_t ms){
		counter += ms;
	}



	void writeCommandSync(uint8_t);
	void writeDataSync(uint8_t* data, uint32_t dataSize);
	void exec(uint8_t cmd, uint8_t* params, uint8_t size, uint32_t delay = 0);
	void execSync(uint8_t cmd, uint8_t* params, uint8_t size, uint32_t delay = 0);
	void setAddressWindowSync(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
	void fillRectangleSync(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
	void fillScreenSync(uint16_t color);
	void drawSync(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t* map, uint32_t size);


	DeviceChain& writeCommand(uint8_t);
	DeviceChain& writeData(uint8_t* data, uint32_t dataSize);
	DeviceChain& setAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
	DeviceChain& draw(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t* map, uint32_t size);

};


}


#endif /* ST7735_V0_DEVICE_HPP_ */
