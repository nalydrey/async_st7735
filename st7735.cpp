/*
 * st7735.cpp
 *
 *  Created on: May 23, 2025
 *      Author: Oleksiy
 */

#include "st7735.h"



ST7735::ST7735(
		uint16_t w,
		uint16_t h,
		Transmitter * trans,
		OutputPin * DC_pin,
		OutputPin * RES_pin,
		function<void(uint32_t)> delayFunction
		) {

	busyState = false;
	busyBehavior = BUSY_BEHAVIOR_BLOCK;

	width = w;
	height = h;
	x_mirror = false;
	y_mirror = false;
	busyState = false;
	colorOrder = COLOR_ORDER_RGB;
	horisontalRefreshOrder = REFRESH_LEFT_TO_RIGHT;
	verticalRefreshOrder = REFRESH_TOP_TO_BOTTOM;
	colorDepth = COLOR_DEPTH_RGB_666;

	transmitter = trans;
	DCpin = DC_pin;
	RESpin = RES_pin;

	transmitter->setDataTransmittedCb([](void * ctx){
		ST7735 * self = (ST7735 *)ctx;
		self->finish();
	}, this);

	delay = delayFunction;

	RESpin->setHigh();

	softwareReset([](const Event_t& evt){
		evt.self->setSleepOutMode([](const Event_t& evt){
			evt.self->setDisplayOnMode([](const Event_t& evt){
				evt.self->initHandler1();
			});
		});
	});

//	busyBehavior = BUSY_BEHAVIOR_SKIP;
}


void ST7735::initHandler1(){
	setOrientation(width > height ? ORIENTATION_HORISONTAL : ORIENTATION_VERTICAL, [](const Event_t& evt){
		evt.self->clearScreen();
	});
}



void ST7735::transmit(uint8_t * data, uint32_t dataSize){
	busyState = true;
	transmitter->transmit(data, dataSize);
//	if(busyBehavior == BUSY_BEHAVIOR_BLOCK)
//		while(busyState){
//			uint8_t a;
//			a = 10;
//			};
}


void ST7735::setBusyBehavior(BusyBehavior_t behavior){
	busyBehavior = behavior;
}

uint8_t ST7735::computeMADCTL(){
	return 	(y_mirror << 7) 				|
			(x_mirror << 6) 				|
			(orientation << 5) 				|
			(verticalRefreshOrder<<4) 		|
			(colorOrder << 3) 				|
			(horisontalRefreshOrder << 2);
}


void ST7735::createStackItem(user_callback_t callback, void * userData, const Context_t * ctx){
	StackItem_t stackItem = {0};
	stackItem.callback = callback;
	stackItem.userData = userData;
	stackItem.context = *ctx;
	stack.push(stackItem);
}



void ST7735::writeCommand(uint8_t command)
{
	DCpin->setLow();
	transmit(&command, 1);
}


void ST7735::writeData(uint8_t * data, uint32_t data_size)
{
	DCpin->setHigh();
	transmit(data, data_size);
}


void ST7735::softwareReset(user_callback_t callback, void * userData){
	createStackItem(callback, userData);

	Context_t ctx;
	ctx.value[0] = 150;
	createStackItem([](const Event_t& evt){
		evt.self->delayAndFinish(evt.context.value[0]);
	}, nullptr, &ctx);

	writeCommand(COMMAND_SWRESET);
}



void ST7735::setSleepInMode(user_callback_t callback, void * userData){
	createStackItem(callback, userData);

	Context_t ctx;
	ctx.value[0] = 20;
	createStackItem([](const Event_t& evt){
		evt.self->delayAndFinish(evt.context.value[0]);
	}, nullptr, &ctx);

	writeCommand(COMMAND_SLPIN);
}



void ST7735::setSleepOutMode(user_callback_t callback, void * userData){
	createStackItem(callback, userData);

	Context_t ctx = {0};
	ctx.value[0] = 120;
	createStackItem([](const Event_t& evt){
		evt.self->delayAndFinish(evt.context.value[0]);
	}, nullptr, &ctx);

	writeCommand(COMMAND_SLPOUT);
}



void ST7735::setDisplayOnMode(user_callback_t callback, void * userData){
	createStackItem(callback, userData);

	Context_t ctx = {0};
	ctx.value[0] = 20;
	createStackItem([](const Event_t& evt){
		evt.self->delayAndFinish(evt.context.value[0]);
	}, nullptr, &ctx);

	writeCommand(COMMAND_DISPON);
}


void ST7735::setDisplayOffMode(user_callback_t callback, void * userData){
	createStackItem(callback, userData);

	Context_t ctx = {0};
	ctx.value[0] = 20;
	createStackItem([](const Event_t& evt){
		evt.self->delayAndFinish(evt.context.value[0]);
	}, nullptr, &ctx);

	writeCommand(COMMAND_DISPOFF);
}



void ST7735::setDsplayInversionOn(user_callback_t callback, void * userData){
	createStackItem(callback, userData);

	Context_t ctx = {0};
	ctx.value[0] = 20;
	createStackItem([](const Event_t& evt){
		evt.self->delayAndFinish(evt.context.value[0]);
	}, nullptr, &ctx);

	writeCommand(COMMAND_INVON);
}



void ST7735::setMADCTL(uint8_t value, user_callback_t callback, void * userData){
	createStackItem(callback, userData);

	Context_t ctx = {0};
	ctx.value[0] = value;
	createStackItem([](const Event_t& evt){
		evt.self->setMADCTLHandlder1(evt.context.value[0]);
	}, nullptr, &ctx);

	writeCommand(COMMAND_MADCTL);
}


void ST7735::setMADCTLHandlder1(uint8_t value){
	Context_t ctx = {0};
	ctx.value[0] = value;
	createStackItem([](const Event_t& evt){
		evt.self->setMADCTLHandlder2(evt.context.value[0]);
	}, nullptr, &ctx);

	writeData(&value, 1);
}


void ST7735::setMADCTLHandlder2(uint8_t value){

	x_mirror = (value >> 6) & 0x01;
	orientation = Orientation_t((value >> 5) & 0x01);
	verticalRefreshOrder = VerticalRefreshOrder_t((value >> 4) & 0x01);
	colorOrder = ColorOrder_t((value >> 3) & 0x01);
	horisontalRefreshOrder = HorisontalRefreshOrder_t((value >> 2) & 0x01);
	finish();
}


void ST7735::setOrientation(Orientation_t orient, user_callback_t callback, void * userData){
	createStackItem(callback, userData);
	uint8_t mask = 1<<5;
	uint8_t oldValue = computeMADCTL();
	uint8_t result = (oldValue & ~mask) | ((orient << 5) & mask);
	setMADCTL(result, [](const Event_t& evt){
		evt.self->setOrientationHandler(evt.self->orientation);
	});
}


void ST7735::setOrientationHandler(Orientation_t orient){
	if(orient == ORIENTATION_HORISONTAL){
		fullWidth = getLargerSize();
		fullHeight = getSmallerSize();
	}
	else{
		fullWidth = getSmallerSize();
		fullHeight = getLargerSize();
	}
	offset_x = (fullWidth - width)/2;
	offset_y = (fullHeight - height)/2;
	finish();
}


void ST7735::setColorOrder(ColorOrder_t order, user_callback_t callback, void * userData){
	uint8_t mask = 1<<3;
	uint8_t oldValue = computeMADCTL();
	uint8_t result = (oldValue & ~mask) | ((order << 3) & mask);
	setMADCTL(result, callback, userData);
}


void ST7735::setMirrorX(user_callback_t callback, void * userData){
	uint8_t mask = 1<<6;
	uint8_t oldValue = computeMADCTL();
	uint8_t result = (oldValue & ~mask) | ((!x_mirror << 6) & mask);
	setMADCTL(result, callback, userData);
}

void ST7735::setMirrorY(user_callback_t callback, void * userData){
	uint8_t mask = 1<<7;
	uint8_t oldValue = computeMADCTL();
	uint8_t result = (oldValue & ~mask) | ((!y_mirror << 7) & mask);
	setMADCTL(result, callback, userData);
}


uint16_t ST7735::getSmallerSize(){
	return displaySize[0] < displaySize[1] ? displaySize[0] : displaySize[1];
}

uint16_t ST7735::getLargerSize(){
	return displaySize[0] > displaySize[1] ? displaySize[0] : displaySize[1];
}


void ST7735::setPixelSequence(uint8_t * pixelSequence, uint32_t size, user_callback_t callback, void * userData){
	createStackItem(callback, userData);

	Context_t ctx = {0};
	ctx.value[0] = size;
	createStackItem([](const Event_t& evt){
		evt.self->writeData((uint8_t*)evt.userData, evt.context.value[0]);
	}, pixelSequence, &ctx);

	writeCommand(COMMAND_RAMWR);
}



void ST7735::setColorDepth(ColorDepth_t colDepth, user_callback_t callback, void * userData){
	createStackItem(callback, userData);

	Context_t ctx = {0};
	ctx.value[0] = colDepth;
	createStackItem([](const Event_t& evt){
		evt.self->setColorDepthHandler((ColorDepth_t)evt.context.value[0]);
	}, nullptr, &ctx);
	writeCommand(COMMAND_COLMOD);
}


void ST7735::setColorDepthHandler(ColorDepth_t colDepth){
	Context_t ctx = {0};
	ctx.value[0] = colDepth;
	createStackItem([](const Event_t& evt){
		evt.self->colorDepth = (ColorDepth_t)evt.context.value[0];
		evt.self->finish();
	}, nullptr, &ctx);
	uint8_t value = colDepth & 0x07;
	writeData(&value, 1);
}


void ST7735::setColumnAddressWindow(uint8_t start, uint8_t end, user_callback_t callback, void * userData){
	createStackItem(callback, userData);

	Context_t ctx = {0};
	ctx.value[0] = start;
	ctx.value[1] = end;
	createStackItem([](const Event_t& evt){
		uint8_t columnData[] = {0, (uint8_t)evt.context.value[0], 0, (uint8_t)evt.context.value[1]};
		evt.self->writeData(columnData, 4);
	}, nullptr, &ctx);
	writeCommand(COMMAND_CASET);
}



void ST7735::setRawAddressWindow(uint8_t start, uint8_t end, user_callback_t callback, void * userData){
	createStackItem(callback, userData);

	Context_t ctx = {0};
	ctx.value[0] = start;
	ctx.value[1] = end;
	createStackItem([](const Event_t& evt){
		uint8_t rawData[] = {0, (uint8_t)evt.context.value[0], 0, (uint8_t)evt.context.value[1]};
		evt.self->writeData(rawData, 4);
	}, nullptr, &ctx);
	writeCommand(COMMAND_RASET);
}



void ST7735::setAddressWindow(
		uint8_t x_start,
		uint8_t y_start,
		uint8_t x_end,
		uint8_t y_end,
		user_callback_t callback,
		void * userData
		){
	createStackItem(callback, userData);

	Context_t ctx = {0};
	ctx.value[0] = y_start;
	ctx.value[1] = y_end;
	createStackItem([](const Event_t& evt){
		evt.self->setAddressWindowHandler(evt.context.value[0], evt.context.value[1]);
	}, nullptr, &ctx);

	setColumnAddressWindow(x_start+offset_x, x_end+offset_x, [](const Event_t& evt){
		evt.self->finish();
	});
}


void ST7735::setAddressWindowHandler(uint8_t y_start,uint8_t y_end){
	setRawAddressWindow(y_start+offset_y, y_end+offset_y, [](const Event_t& evt){
		evt.self->finish();
	});
}




void ST7735::clearScreen(user_callback_t callback, void * userData){
	createStackItem(callback, userData);
	counter = 0;

	setColumnAddressWindow(0, fullWidth, [](const Event_t& evt){evt.self->clearScreenHandler1();});
}


void ST7735::clearScreenHandler1(){
	setRawAddressWindow(0, fullHeight, [](const Event_t& evt){evt.self->clearScreenHandler2();});
}


void ST7735::clearScreenHandler2(){
	createStackItem([](const Event_t& evt){evt.self->clearScreenHandler3();});
	writeCommand(COMMAND_RAMWR);
}

void ST7735::clearScreenHandler3(){
	counter++;
	if(counter > (fullWidth*fullHeight)) {
		finish();
		return;
	}
	createStackItem([](const Event_t& evt){
		evt.self->clearScreenHandler3();
	});
	writeData(data, colorDepth == COLOR_DEPTH_RGB_666 ? 3 : 2);
}


void ST7735::drawImage(
		uint8_t x_start,
		uint8_t y_start,
		uint8_t x_end,
		uint8_t y_end,
		uint8_t *pixelSequence,
		uint32_t size,
		user_callback_t callback,
		void * userData){

	createStackItem(callback, userData);

	drawCtx.data = pixelSequence;
	drawCtx.dataSize = size;

	setAddressWindow(x_start, y_start, x_end, y_end,
			[](const Event_t& evt){
				DrawImageContext_t * img= (DrawImageContext_t *)evt.userData;
				evt.self->setPixelSequence(img->data, img->dataSize, [](const Event_t& evt){
						evt.self->finish();
				});
			}, &drawCtx);

}


void ST7735::finish(){
	busyState = false;

	StackItem_t* stackItem = stack.pop();

	Event_t event = {0};
	event.self = this;
	event.context = stackItem->context;
	event.userData = stackItem->userData;

	if(stackItem->callback) stackItem->callback(event);
}





