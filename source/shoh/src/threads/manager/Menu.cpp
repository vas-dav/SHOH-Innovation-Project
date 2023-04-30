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
    switch(ep->et/*EventType*/)
    {
        case Event::Rotary:
            // can be wrapped in a function, but this was found to be more clear solution,
            // since we are still handling eventpair here, although nested
            switch(static_cast<ThreadCommon::RotaryAction>(ep->rd)/*RawData*/)
            {
                case ThreadCommon::RotaryAction::Right:
                    this->HandleObj(MenuObjEvent::eRollClockWise);
                    break;
                case ThreadCommon::RotaryAction::Left:
                    this->HandleObj(MenuObjEvent::eRollCClockWise);
                    break;
                case ThreadCommon::RotaryAction::Press:
                    this->HandleObj(MenuObjEvent::eClick);
                    break;
                case ThreadCommon::RotaryAction::Idle:
                    /*I guess this is left for debugging purposes ;D*/
                    break;
                case default:
                    /* Pretty damn bad code if reached here */
                    break;
            }
            break;
        case Event::InternalTemp:
            // TODO
            break;
        case Event::ExternalTemp:
            // TODO
            break;
        case default:
            /* Manager has big issues... */
            break;
    }
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

