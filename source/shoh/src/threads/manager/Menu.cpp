/*
 * Menu.cpp
 *
 *  Created on: 29 Apr 2023
 *      Author: dave
 */

#include "Menu.h"

Menu::Menu() 
{
	// TODO Auto-generated constructor stub

}

Menu::~Menu() 
{
	// TODO Auto-generated destructor stub
}

void Menu::HandleEventPair (Event::EventPair *ep)
{
	return;
}

void Menu::SetEvent (obj_pointer newevent)
{
	(this->*current) (MenuObjEvent (MenuObjEvent::eUnFocus));
	current = newevent;
	(this->*current) (MenuObjEvent (MenuObjEvent::eFocus));
}

void Menu::HandleObj (const MenuObjEvent &event)
{
	(this->*current) (event);
}

