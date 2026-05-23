/*
 * device.cpp
 *
 *  Created on: 19 мая 2026 г.
 *      Author: Oleksiy
 */


#include "device.hpp"
#include "main.h"




// based on Adafruit ST7735 library for Arduino
using namespace nikolex::st7735::v0;
using COMMAND = nikolex::st7735::COMMAND;

uint8_t frmctr1Params[] =  {0x01, 0x2C, 0x2D};
uint8_t frmctr2Params[] =  {0x01, 0x2C, 0x2D};
uint8_t frmctr3Params[] =  {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D};
uint8_t invctrParams[] 	=  {0x07};
uint8_t pwrctr1Params[] =  {0xA2, 0x02, 0x84};
uint8_t pwrctr2Params[] =  {0xC5};
uint8_t pwrctr3Params[] =  {0x0A, 0x00};
uint8_t pwrctr4Params[] =  {0x8A, 0x2A};
uint8_t vmctr1Params[] 	=  {0x0E};
uint8_t madctlParams[] 	=  {0x40 | 0x20 | 0x08};
uint8_t colmodParams[] 	=  {0x05};


Transaction initList[] = {
		{COMMAND::SWRESET, nullptr, 0, 150},
		{COMMAND::SLPOUT, nullptr, 0, 500},
		{COMMAND::FRMCTR1, frmctr1Params, sizeof(frmctr1Params)},
		{COMMAND::FRMCTR2, frmctr2Params, sizeof(frmctr2Params)},
		{COMMAND::FRMCTR3, frmctr3Params, sizeof(frmctr3Params)},
		{COMMAND::INVCTR, invctrParams, sizeof(invctrParams)},
		{COMMAND::PWCTR1, pwrctr1Params, sizeof(pwrctr1Params)},
		{COMMAND::PWCTR2, pwrctr2Params, sizeof(pwrctr2Params)},
		{COMMAND::PWCTR3, pwrctr3Params, sizeof(pwrctr3Params)},
		{COMMAND::PWCTR4, pwrctr4Params, sizeof(pwrctr4Params)},
		{COMMAND::VMCTR1, vmctr1Params, sizeof(vmctr1Params)},
		{COMMAND::IVNOFF, nullptr, 0},
		{COMMAND::MADCTL, madctlParams, sizeof(madctlParams)},
		{COMMAND::COLMOD, colmodParams, sizeof(colmodParams)},
};


uint8_t casetParams[] = {0x00, 0x00, 0x00, 0x4F};
uint8_t rasetParams[] = {0x00, 0x00, 0x00, 0x9F};

Transaction dispSpecificInitList[] = {
		{COMMAND::CASET, casetParams, sizeof(casetParams)},
		{COMMAND::RASET, rasetParams, sizeof(rasetParams)},
		{COMMAND::IVNON, nullptr, 0}
};

uint8_t gmctrp1Params[] =  {0x02, 0x1c, 0x07, 0x12,
							0x37, 0x32, 0x29, 0x2d,
							0x29, 0x25, 0x2B, 0x39,
							0x00, 0x01, 0x03, 0x10};
uint8_t gmctrn1Params[] =  {0x03, 0x1d, 0x07, 0x06,
							0x2E, 0x2C, 0x29, 0x2D,
							0x2E, 0x2E, 0x37, 0x3F,
							0x00, 0x00, 0x02, 0x10};

Transaction finishInitList[] = {
		{COMMAND::GMCTRP1, gmctrp1Params, sizeof(gmctrp1Params)},
		{COMMAND::GMCTRN1, gmctrn1Params, sizeof(gmctrn1Params)},
		{COMMAND::NORON, nullptr, 0, 10},
		{COMMAND::DISPON, nullptr, 0, 100}
};

Device::Device(
		SpiTransmitOnly* spiDriver,
		OutputPin* resPinDriver,
		OutputPin* dcPinDriver,
		OutputPin* csPinDriver,
		MODEL model
) :
	spi(spiDriver),
	resPin(resPinDriver),
	dcPin(dcPinDriver),
	csPin(csPinDriver)
{
	attachDisplayParams(model);
	spi->setDataReadyCb([](void * ctx){
		Device* dev = (Device*)ctx;
		dev->busy = false;
		if(dev->isCompleteRequestActive){
			dev->csPin->setHigh();
		}

	}, this);
}


void Device::init(){
	spi->init();
	resPin->init();
	dcPin->init();
	csPin->init();
	resPin->setHigh();

	for(const auto item : initList){
		execSync(item.cmd, item.params, item.paramSize, item.delayAfterMs);
	}

	for(const auto item : dispSpecificInitList){
		execSync(item.cmd, item.params, item.paramSize, item.delayAfterMs);
	}

	for(const auto item : finishInitList){
		execSync(item.cmd, item.params, item.paramSize, item.delayAfterMs);
	}
}


void Device::attachDisplayParams(MODEL mod){
	DisplayParams modelList[] = {
			{ ST7735s_160x80, 160, 80, 1, 26 }
	};

	for(auto item : modelList){
		if(item.model == mod){
			width = item.width;
			height = item.height;
			offset_x = item.offset_x;
			offset_y = item.offset_y;
		}
	}
}

void Device::writeCommandSync(uint8_t cmd){
	busy = true;
	dcPin->setLow();
	spi->transmit(&cmd, 1);
	while(busy){};
}

void Device::writeDataSync(uint8_t* data, uint32_t dataSize){
	busy = true;
	dcPin->setHigh();
	spi->transmit(data, dataSize);
	while(busy){};
}



void Device::execSync(uint8_t cmd, uint8_t* params, uint8_t size, uint32_t delay){
	isCompleteRequestActive = !size ? true : false;
	csPin->setLow();
	writeCommandSync(cmd);

	if(!size) return;
	isCompleteRequestActive = true;
	writeDataSync(params, size);
}



void Device::setAddressWindowSync(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
	writeCommandSync(COMMAND::CASET);
    uint8_t data[] = {0x00, x0 + offset_x, 0x00, x1 + offset_x};
    writeDataSync(data, sizeof(data));
    writeCommandSync(COMMAND::RASET);
    data[1] = y0 + offset_y;
    data[3] = y1 + offset_y;
    writeDataSync(data, sizeof(data));
	writeCommandSync(COMMAND::RAMWR);
}


void Device::fillRectangleSync(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {

    if((x >= width) || (y >= height)) return;
    if((x + w - 1) >= width) w = width - x;
    if((y + h - 1) >= height) h = height - y;
    isCompleteRequestActive = false;
    csPin->setLow();
    setAddressWindowSync(x, y, x+w-1, y+h-1);

    uint8_t data[] = { color >> 8, color & 0xFF };
    dcPin->setHigh();
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
        	if(y == 1 && x == 1) isCompleteRequestActive = true;
        	busy = true;
        	spi->transmit(data, sizeof(data));
        	while(busy){}
        }
    }
}

void Device::fillScreenSync(uint16_t color) {
	    fillRectangleSync(0, 0, width, height, color);
}


void Device::drawSync(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t* map, uint32_t size){
		if((x >= width) || (y >= height)) return;
		if((x + w - 1) >= width) w = width - x;
		if((y + h - 1) >= height) h = height - y;
		isCompleteRequestActive = false;
		csPin->setLow();
		setAddressWindowSync(x, y, x+w-1, y+h-1);
		isCompleteRequestActive = true;
		writeDataSync(map, size);
}


Device::DeviceChain& Device::writeCommand(uint8_t cmd){

	ChainAndTask item = createChainAndTask();

	item.p_chain->ctx.evtData.writeCommandEventData.cmd = cmd;

	item.p_task->exec = [](DeviceChain* chain){
		Device* dev = chain->ctx.device;
		dev->busy = true;
		dev->dcPin->setLow();
		dev->spi->transmit(&chain->ctx.evtData.writeCommandEventData.cmd, 1);
	};

	return *item.p_chain;
}


Device::DeviceChain& Device::writeData(uint8_t* data, uint32_t dataSize){
	ChainAndTask item = createChainAndTask();

	WriteDataEventData& evtData = item.p_chain->ctx.evtData.writeDataEventData;
	evtData.params = data;
	evtData.paramSize = dataSize;

	item.p_task->exec = [](DeviceChain* chain){
		Device* dev = chain->ctx.device;
		WriteDataEventData& evtData = chain->ctx.evtData.writeDataEventData;
		dev->busy = true;
		dev->dcPin->setHigh();
		dev->spi->transmit(evtData.params, evtData.paramSize);
	};

	return *item.p_chain;
}


Device::DeviceChain& Device::setAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){

	ChainAndTask item = createChainAndTask();

	SetAddressWindowEventData& evtData = item.p_chain->ctx.evtData.setAddressWindowEventData;


	evtData.column[0] = 0;
	evtData.column[1] = x0 + offset_x;
	evtData.column[2] = 0;
	evtData.column[3] = x1 + offset_x;

	evtData.raw[0] = 0;
	evtData.raw[1] = y0 + offset_y;
	evtData.raw[2] = 0;
	evtData.raw[3] = y1 + offset_y;



	item.p_task->exec = [](DeviceChain* chain){
		chain->blocked = true;
		Device* dev = chain->ctx.device;

		SetAddressWindowEventData& evtData = chain->ctx.evtData.setAddressWindowEventData;

		dev->writeCommand(COMMAND::CASET);
	    dev->writeData(evtData.column, sizeof(evtData.column));
	    dev->writeCommand(COMMAND::RASET);
	    dev->writeData(evtData.raw, sizeof(evtData.raw));
		dev->writeCommand(COMMAND::RAMWR)
			.then([](DeviceChain* chain){
				DeviceChain* ctxChain = (DeviceChain*)chain->userData;
				ctxChain->unblock();
			}, chain);
	};

	return *item.p_chain;
}




Device::DeviceChain& Device::draw(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t* map, uint32_t size){

	ChainAndTask item = createChainAndTask();

	DrawEventData& evtData = item.p_chain->ctx.evtData.drawEventData;

	evtData.x = x;
	evtData.y = y;
	evtData.width = w;
	evtData.height = h;
	evtData.map = map;
	evtData.size = size;


	item.p_task->exec = [](DeviceChain* chain){
		chain->blocked = true;
		Device* dev = chain->ctx.device;

		DrawEventData& evtData = chain->ctx.evtData.drawEventData;

		uint16_t dispWidth = dev->width;
		uint16_t dispHeight = dev->height;
		if((evtData.x >= dispWidth) || (evtData.y >= dispHeight)) return;
		if((evtData.x + evtData.width - 1) >= dispWidth) evtData.width = dispWidth - evtData.x;
		if((evtData.y + evtData.height - 1) >= dispHeight) evtData.height = dispHeight - evtData.y;
		dev->isCompleteRequestActive = false;
		dev->csPin->setLow();
		dev->setAddressWindow(evtData.x, evtData.y, evtData.x+evtData.width-1, evtData.y+evtData.height-1)
			.then([](DeviceChain* chain){
				Device* dev = (Device*)chain->ctx.device;
				dev->isCompleteRequestActive = true;
			}, chain);
		dev->writeData(evtData.map, evtData.size)
			.then([](DeviceChain* chain){
					DeviceChain* ctxChain = (DeviceChain*)chain->userData;
					ctxChain->unblock();
			}, chain);

	};

	return *item.p_chain;
}
