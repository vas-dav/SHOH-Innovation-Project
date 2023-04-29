/*
 * Menu.h
 *
 *  Created on: 29 Apr 2023
 *      Author: dave
 */

#include "Counter.h"
#include "MenuObjEvent.h"
#include "ThreadCommon.h"
#include "Event.h"

#ifndef THREADS_MANAGER_MENU_H_
#define THREADS_MANAGER_MENU_H_

class Menu;
typedef void (Menu::*obj_pointer) (const MenuObjEvent &);

class Menu
{
public:
  Menu ();
  virtual ~Menu ();
  void HandleEventPair (Event::EventPair *ep);

private:
  /* Variables and objects */
  obj_pointer current;
  /* Methods */
  void SetEvent (obj_pointer newevent);
  void HandleObj (const MenuObjEvent &event);
};

#endif /* THREADS_MANAGER_MENU_H_ */
