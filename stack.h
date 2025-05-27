/*
 * stack.h
 *
 *  Created on: May 19, 2025
 *      Author: Oleksiy
 */

#ifndef ST7735_STACK_H_
#define ST7735_STACK_H_

#include "stdint.h"
#include "functional"

using std::function;

namespace st7735 {

template<typename T, uint8_t size>
class Stack {
	T list[size];
	int8_t index;
public:
	Stack(){index = -1;}

	T * getCurrentItem(){return &list[index];}
	T * push(const T& item){
		index++;
		list[index] = item;
		return &list[index];
	}

	T * pop(){
		T * lastItem = &list[index];
		index--;
		return lastItem;
	}
};

}

#endif /* ST7735_STACK_H_ */
