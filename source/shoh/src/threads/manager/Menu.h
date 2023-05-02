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
typedef void (Menu::*p_state) (const MenuObjEvent &);

class Menu
{
public:
  Menu ();
  virtual ~Menu ();
  void HandleEventPair (Event::EventPair *ep);

private:
  /* Variables and objects */
  p_state current;
  /* Methods */
  void SetState (p_state new_state);
  void HandleObj (const MenuObjEvent &event);
};

#endif /* THREADS_MANAGER_MENU_H_ */
