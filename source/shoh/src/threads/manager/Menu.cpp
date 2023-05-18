/*
 * Menu.cpp
 *
 *  Created on: 29 Apr 2023
 *      Author: dave
 */

#include "Menu.h"
#include <assert.h>
#include "UserInterface.h"
#include "Log.h"

Menu::Menu(ThreadCommon::QueueManager* qm): _qm(qm),
current(&Menu::sInitView), ext_temp(-99, 99, 1), set_point(-99, 99, 1),
main_text     ("CURRENT %3d     DESIRED %3d     "),
set_point_text("CURRENT %3d     DESIRED[%3d]    ")
{
    LOG_DEBUG("Creating Menu");
	this->SetState(&Menu::sInitView);
    ext_temp.setCurrent(0);
    set_point.setCurrent(0);
    readSetPointFromEEPROM();
}

Menu::~Menu() 
{
    LOG_DEBUG("Deleting Menu");
}

void
Menu::readSetPointFromEEPROM (void)
{
  EventRawData *data = (EventRawData *)eeprom.read_from (EEPROM_START_ADDR,
                                                   sizeof(EventRawData));
  if ((*data) > 0 && (*data) < 100)
    {
      set_point.setCurrent(*data);
      Event e(Event::EventType::SetPoint, set_point.getCurrent());
      _qm->send<Event>(ThreadCommon::QueueManager::master_event_all, &e , 1);
    }
}

void Menu::parseEvent (Event *ep)
{
    switch(ep->getType()/*EventType*/)
    {
        case Event::Rotary:
            // can be wrapped in a function, but this was found to be more clear solution,
            // since we are still handling eventpair here, although nested
            switch(static_cast<ThreadCommon::RotaryAction>(ep->getData())/*RawData*/)
            {
                case ThreadCommon::RotaryAction::Right:
                    this->HandleObj(MenuObjEvent (MenuObjEvent::eRollClockWise));
                    break;
                case ThreadCommon::RotaryAction::Left:
                    this->HandleObj(MenuObjEvent (MenuObjEvent::eRollCClockWise));
                    break;
                case ThreadCommon::RotaryAction::Press:
                    this->HandleObj(MenuObjEvent (MenuObjEvent::eClick));
                    break;
                case ThreadCommon::RotaryAction::Idle:
                    /*I guess this is left for debugging purposes ;D*/
                    this->HandleObj(MenuObjEvent (MenuObjEvent::eRefresh));
                    break;
                default:
                    assert(0);
                    /* Pretty damn bad code if reached here */
                    break;
            }
            break;
        case Event::ExternalTemp:
            //Change ExternalTemp value. -99 <= ext_temp <= 99
            this->ext_temp.setCurrent(ep->getData());
            //Refresh the menu screen.
            this->HandleObj(MenuObjEvent (MenuObjEvent::eRefresh));
            break;
        default:
            assert(0);
            /* Manager has big issues... */
            break;
    }
	return;
}

/* States */
void Menu::sInitView(const MenuObjEvent &e)
{
    switch (e.type)
    {
    case MenuObjEvent::eFocus:
        LOG_DEBUG("enter sInitView");
        this->NotifyAndRefreshUI("Loading...");
        break;
    case MenuObjEvent::eUnFocus:
        LOG_DEBUG("leave sInitView");
        this->NotifyAndRefreshUI("");
        break;
    case MenuObjEvent::eRollClockWise:
        break;
    case MenuObjEvent::eRollCClockWise:
        break;
    case MenuObjEvent::eClick:
        LOG_DEBUG("click sInitView");
        this->SetState(&Menu::sMainView);
        break;
    case MenuObjEvent::eRefresh:
        LOG_DEBUG("refersh sInitView");
        this->SetState(&Menu::sMainView);
        break;
    default:
        break;
    }
}

void Menu::sMainView(const MenuObjEvent &e)
{
    static char screen_text[64];
    switch (e.type)
    {
    case MenuObjEvent::eFocus: 
        LOG_DEBUG("enter sMainView");
        sprintf(screen_text, main_text, this->ext_temp.getCurrent(),
                                        this->set_point.getCurrent());
        this->NotifyAndRefreshUI(screen_text);
        break;
    case MenuObjEvent::eUnFocus:
        LOG_DEBUG("leave sMainView");
        this->NotifyAndRefreshUI("");
        break;
    case MenuObjEvent::eRollClockWise:
        break;
    case MenuObjEvent::eRollCClockWise:
        break;
    case MenuObjEvent::eClick:
        LOG_DEBUG("click sMainView");
        this->SetState(&Menu::sSetPointMod);
        break;
    case MenuObjEvent::eRefresh:
        sprintf(screen_text, main_text, this->ext_temp.getCurrent(),
                                        this->set_point.getCurrent());
        LOG_DEBUG("refresh sMainView");
        this->NotifyAndRefreshUI(screen_text);
        break;
    default:
        break;
    }
}

void Menu::sSetPointMod(const MenuObjEvent &e)
{
    static char screen_text[64];
    EventRawData sp;
    Event event_sp (Event::EventType::SetPoint, set_point.getCurrent());
    switch (e.type)
    {
    case MenuObjEvent::eFocus:
        LOG_DEBUG("enter sSetPointMod");
        sprintf(screen_text, set_point_text, this->ext_temp.getCurrent(),
                                             this->set_point.getCurrent());
        this->NotifyAndRefreshUI(screen_text);
        break;
    case MenuObjEvent::eUnFocus:
        LOG_DEBUG("leave sSetPointMod");
        this->NotifyAndRefreshUI("");
        break;
    case MenuObjEvent::eRollClockWise:
        set_point.inc();
        sprintf(screen_text, set_point_text, this->ext_temp.getCurrent(),
                                             this->set_point.getCurrent());
        this->NotifyAndRefreshUI(screen_text);
        break;
    case MenuObjEvent::eRollCClockWise:
        set_point.dec();
        sprintf(screen_text, set_point_text, this->ext_temp.getCurrent(),
                                             this->set_point.getCurrent());
        this->NotifyAndRefreshUI(screen_text);
        break;
    case MenuObjEvent::eClick:
        LOG_DEBUG("click sSetPointMod");
        sp = set_point.getCurrent();
        // Write to EEPROM
        eeprom.write_to(EEPROM_START_ADDR, (void*)&sp, sizeof(EventRawData));

        event_sp.setEvent(Event::EventType::SetPoint, sp);
        _qm->send<Event>(ThreadCommon::QueueManager::master_event_all, &event_sp, 1);
        
        this->SetState(&Menu::sMainView);
    
        break;
    case MenuObjEvent::eRefresh:
        LOG_DEBUG("refresh sSetPointMod");
        sprintf(screen_text, set_point_text, this->ext_temp.getCurrent(),
                                             this->set_point.getCurrent());
        this->NotifyAndRefreshUI(screen_text);
        break;
    default:
        break;
    }
}

void Menu::SetState (p_state new_state)
{
	(this->*current) (MenuObjEvent (MenuObjEvent::eUnFocus));
	current = new_state;
	(this->*current) (MenuObjEvent (MenuObjEvent::eFocus));
}

void Menu::HandleObj (const MenuObjEvent &event)
{
	(this->*current) (event);
}

void Menu::NotifyAndRefreshUI (const char *str)
{
    //Send string on a queue to UI task.
    UserInterface::InterfaceWithData ud = {UserInterface::LCD1, str};
    this->_qm->send<UserInterface::InterfaceWithData>(
                    ThreadCommon::QueueManager::ui_event_manager, &ud, portMAX_DELAY);
}
