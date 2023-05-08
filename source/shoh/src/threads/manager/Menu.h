/*
 * Menu.h
 *
 *  Created on: 29 Apr 2023
 *      Author: dave
 */

#ifndef THREADS_MANAGER_MENU_H_
#define THREADS_MANAGER_MENU_H_

#include "Counter.h"
#include "MenuObjEvent.h"
#include "ThreadCommon.h"
#include "Event.h"

class Menu;
typedef void (Menu::*p_state) (const MenuObjEvent &);

class Menu
{
public:
  Menu (ThreadCommon::QueueManager* qm);
  virtual ~Menu ();
  void HandleEventPair (Event::EventPair *ep);

private:
  /* Variables and objects */
  ThreadCommon::QueueManager* _qm;
  p_state current;
  short ext_temp;
  Counter<EventRawData> set_point;
  const char main_text[64];
  const char set_point_text[64];
  /* States */
  void sInitView(const MenuObjEvent &e);
  void sMainView(const MenuObjEvent &e);
  void sSetPointMod(const MenuObjEvent &e);
  /* Methods */
  void SetState (p_state new_state);
  void HandleObj (const MenuObjEvent &event);
  void NotifyAndRefreshUI (const char *str);
};

#endif /* THREADS_MANAGER_MENU_H_ */
