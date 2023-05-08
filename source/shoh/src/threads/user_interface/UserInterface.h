/*
 * UserInterface.h
 *
 *  Created on: 8 May 2023
 */

#ifndef THREADS_USER_INTERFACE_USERINTERFACE_H_
#define THREADS_USER_INTERFACE_USERINTERFACE_H_

#include "ThreadCommon.h"
#include "Event.h"
#include "LiquidCrystal.h"

class UserInterface {
public:
	UserInterface(ThreadCommon::QueueManager* qm);
	virtual ~UserInterface();

	enum Interface
	{
		LCD1
	};

	union InterfaceData
	{
		char str[64];
	};

	struct InterfaceWithData
	{
		Interface display;
		InterfaceData data;
	};

	void taskFunction();
private:
	/* Variables */
	ThreadCommon::QueueManager* _qm;
	LiquidCrystal* lcd1;
	DigitalIoPin *lcd1_rs, *lcd1_en, *lcd1_d4, *lcd1_d5, *lcd1_d6, *lcd1_d7;
	
	/* Methods */
	void handleEvent(InterfaceWithData* ui_data);

	//UIs
	void handleLCD(LiquidCrystal *lcd, const char *str);
	void initLCD(LiquidCrystal *lcd,
				 DigitalIoPin *rs, DigitalIoPin *en,
				 DigitalIoPin *d4, DigitalIoPin *d5,
				 DigitalIoPin *d6, DigitalIoPin *d7);
};

void thread_user_interface(void* pvParams);

#endif /* THREADS_USER_INTERFACE_USERINTERFACE_H_ */
