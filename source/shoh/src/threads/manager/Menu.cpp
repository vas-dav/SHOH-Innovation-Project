/*
 * Menu.cpp
 *
 *  Created on: 29 Apr 2023
 *      Author: dave
 */

#include "Menu.h"
#include <assert.h>
#include <string.h>
#include "UserInterface.h"
#include "Log.h"

enum 
{
    CURRENT_TEMP,
    DESIRED_TEMP,
    DESIRED_TEMP_F,
    LOADING,
    ERROR_SCREEN,
    DEVICE_CORRUPTED
};

static const char * interface_messages [] = 
{
    "CURRENT %3d     ",
    "DESIRED  %2d    ",
    "DESIRED [%2d]   ",
    "   Loading...   ",
    "    ERROR       ",
    "Device Corrupted"
};

Menu::Menu(ThreadCommon::QueueManager* qm): _qm(qm),
current(&Menu::sInitView), ext_temp(-99, 99, 1), set_point(10, 99, 1)
{
    LOG_DEBUG("Creating Menu");
	this->SetState(&Menu::sInitView);
    ext_temp.setCurrent(0);
    set_point.setCurrent(0);
    memset(screen_text, 0, 64); // Clear screen
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
    EventRawData ext_temp;
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
            ext_temp = ep->getData();
            if(ext_temp == -128)
            {
              LOG_ERROR("Menu recieved -128 as ext_temp.");
              this->SetState(&Menu::sErrorView);
              break;
            }
            this->ext_temp.setCurrent(ext_temp);
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
        constructUIString(0, interface_messages[LOADING], this->ext_temp.getCurrent());
        this->NotifyAndRefreshUI();
        break;
    case MenuObjEvent::eUnFocus:
        LOG_DEBUG("leave sInitView");
        memset(screen_text, 0, 64); // Clear screen
        this->NotifyAndRefreshUI();
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
        constructUIString(0, interface_messages[CURRENT_TEMP], this->ext_temp.getCurrent());
        constructUIString(1, interface_messages[DESIRED_TEMP], this->set_point.getCurrent());
        this->NotifyAndRefreshUI();
        break;
    case MenuObjEvent::eUnFocus:
        LOG_DEBUG("leave sMainView");
        memset(screen_text, 0, 64); // Clear screen
        this->NotifyAndRefreshUI();
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
        constructUIString(0, interface_messages[CURRENT_TEMP], this->ext_temp.getCurrent());
        LOG_DEBUG("refresh sMainView");
        this->NotifyAndRefreshUI();
        break;
    default:
        break;
    }
}

void Menu::sErrorView(const MenuObjEvent &e)
{
    static char screen_text[64];
    switch (e.type)
    {
    case MenuObjEvent::eFocus: 
        LOG_DEBUG("enter sErrorView");
        constructUIString(0, interface_messages[ERROR_SCREEN]);
        constructUIString(1, interface_messages[DEVICE_CORRUPTED]);
        this->NotifyAndRefreshUI();
        break;
    case MenuObjEvent::eUnFocus:
        LOG_DEBUG("leave sErrorView");
        memset(screen_text, 0, 64); // Clear screen
        this->NotifyAndRefreshUI();
        break;
    case MenuObjEvent::eRollClockWise:
        break;
    case MenuObjEvent::eRollCClockWise:
        break;
    case MenuObjEvent::eClick:
        break;
    case MenuObjEvent::eRefresh:
        LOG_DEBUG("refresh sErrorView");
        if (this->ext_temp.getCurrent() != -128)
        {
          this->SetState(&Menu::sInitView);
        }
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
        constructUIString(0, interface_messages[CURRENT_TEMP], this->ext_temp.getCurrent());
        constructUIString(1, interface_messages[DESIRED_TEMP_F], this->set_point.getCurrent());
        this->NotifyAndRefreshUI();
        break;
    case MenuObjEvent::eUnFocus:
        LOG_DEBUG("leave sSetPointMod");
        memset(screen_text, 0, 64); // Clear screen
        this->NotifyAndRefreshUI();
        break;
    case MenuObjEvent::eRollClockWise:
        set_point.inc();
        constructUIString(1, interface_messages[DESIRED_TEMP_F], this->set_point.getCurrent());
        this->NotifyAndRefreshUI();
        break;
    case MenuObjEvent::eRollCClockWise:
        set_point.dec();
        constructUIString(1, interface_messages[DESIRED_TEMP_F], this->set_point.getCurrent());
        this->NotifyAndRefreshUI();
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
        constructUIString(0, interface_messages[CURRENT_TEMP], this->ext_temp.getCurrent());
        this->NotifyAndRefreshUI();
        break;
    default:
        break;
    }
}

void Menu::constructUIString(uint8_t line, const char *fmt, ...)
{
    char buff[17];
    va_list args;
    va_start (args, fmt);

    size_t char_line_counter = 0;
    for (uint8_t i = 0; i < line; i++)
    {
        char_line_counter += 16;
    }
    
    vsprintf (buff, fmt, args);
    memcpy(screen_text + char_line_counter, buff, 16);
    va_end (args);
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

void Menu::NotifyAndRefreshUI ()
{
    //Send string on a queue to UI task.
    UserInterface::InterfaceWithData ud = {UserInterface::LCD1, screen_text};
    this->_qm->send<UserInterface::InterfaceWithData>(
                    ThreadCommon::QueueManager::ui_event_manager, &ud, portMAX_DELAY);
}
