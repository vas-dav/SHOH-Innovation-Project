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
	Event data(Event::Null, 0);
	//LCD1 init.
	this->initLCD(this->lcd1, this->lcd1_rs, this->lcd1_en, this->lcd1_d4,
				  this->lcd1_d5, this->lcd1_d6, this->lcd1_d7);

	for (;;)
	{
		this->_qm->receive<Event>(ThreadCommon::QueueManager::ui_event_manager, &data, portMAX_DELAY);
		//Don't mind the type, we care only about the raw_data.
		EventRawData ed = data.getDataOf(Event::NotifyUI);
		if(ed != ERROR_RETURN)
			this->handleEvent(reinterpret_cast<UserInterface::InterfaceWithData*>(&ed));
		else
			printf("ERROR: [UserInterface::taskFunction] Event gave ERROR_RETURN data.");
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
		printf("WARNING: [UserInterface::handleEvent] executed default case.");
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

void UserInterface::initLCD(LiquidCrystal *lcd,
				 			DigitalIoPin *rs, DigitalIoPin *en,
				 			DigitalIoPin *d4, DigitalIoPin *d5,
				 			DigitalIoPin *d6, DigitalIoPin *d7)
{
	rs = new DigitalIoPin(1, 9, false);
	en = new DigitalIoPin(0, 14, false);
	d4 = new DigitalIoPin(0, 13, false);
	d5 = new DigitalIoPin(0, 12, false);
	d6 = new DigitalIoPin(0, 23, false);
	d7 = new DigitalIoPin(0, 11, false);

	//Fix Pin Muxing.
	Chip_IOCON_PinMuxSet (LPC_IOCON, 1, 9, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet (LPC_IOCON, 0, 14, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet (LPC_IOCON, 0, 13, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet (LPC_IOCON, 0, 12, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet (LPC_IOCON, 0, 23, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet (LPC_IOCON, 0, 11, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));

	rs->write(false);
	en->write(false);
	d4->write(false);
	d5->write(false);
  	d6->write(false);
  	d7->write(false);
	// LCD init.
	lcd = new LiquidCrystal(rs, en, d4, d5, d6, d7);
	// LCD configure display geometry.
	lcd->begin(16, 2);
	lcd->setCursor (0, 0);
}

void thread_user_interface(void* pvParams)
{
	UserInterface ui(static_cast<ThreadCommon::QueueManager*>(pvParams));
	ui.taskFunction();
}
