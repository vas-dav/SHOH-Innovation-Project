/*
 * UserInterface.cpp
 *
 *  Created on: 8 May 2023
 */

#include "UserInterface.h"
#include <cstring>

UserInterface::UserInterface(ThreadCommon::QueueManager* qm) :
_qm(qm), lcd1(nullptr)
{
	this->initLCD1();
}

UserInterface::~UserInterface()
{
	delete this->lcd1;
	delete this->lcd1_rs;
	delete this->lcd1_en;
	delete this->lcd1_d4;
	delete this->lcd1_d5;
	delete this->lcd1_d6;
	delete this->lcd1_d7;
}

void UserInterface::taskFunction()
{
	InterfaceWithData ui_with_data;

	for (;;)
	{
		this->_qm->receive<UserInterface::InterfaceWithData>(ThreadCommon::QueueManager::ui_event_manager, &ui_with_data, portMAX_DELAY);
		this->handleEvent(&ui_with_data);
	}
	
}

void UserInterface::handleEvent(InterfaceWithData* ui_data)
{
	switch (ui_data->display)
	{
	case LCD1:
		this->handleLCD(this->lcd1, ui_data->data.str);
		break;
	default:
		//Should never happen.
		printf("WARNING: [UserInterface::handleEvent] executed default case.\n");
		break;
	}
}

/******************************/
/*Interface specific functions*/
/******************************/

void UserInterface::handleLCD(LiquidCrystal *lcd, const char *str)
{
	//Interpret empty string as clear.
	if(!strlen(str))
		lcd->clear();
	//Print the text otherwise.
	else
	{
		lcd->setCursor(0, 0);
		lcd->print(str);
	}
}

void UserInterface::initLCD1()
{
	this->lcd1_rs = new DigitalIoPin(1, 9, false);
	this->lcd1_en = new DigitalIoPin(0, 14, false);
	this->lcd1_d4 = new DigitalIoPin(0, 13, false);
	this->lcd1_d5 = new DigitalIoPin(0, 12, false);
	this->lcd1_d6 = new DigitalIoPin(0, 23, false);
	this->lcd1_d7 = new DigitalIoPin(0, 11, false);

	this->lcd1_rs->write(false);
	this->lcd1_en->write(false);
	this->lcd1_d4->write(false);
	this->lcd1_d5->write(false);
  	this->lcd1_d6->write(false);
  	this->lcd1_d7->write(false);
	// LCD init.
	this->lcd1 = new LiquidCrystal(this->lcd1_rs, this->lcd1_en, this->lcd1_d4,
								   this->lcd1_d5, this->lcd1_d6, this->lcd1_d7);
	// LCD configure display geometry.
	this->lcd1->begin(16, 2);
	this->lcd1->setCursor (0, 0);
}

void thread_user_interface(void* pvParams)
{
	UserInterface ui(static_cast<ThreadCommon::QueueManager*>(pvParams));
	ui.taskFunction();
}
