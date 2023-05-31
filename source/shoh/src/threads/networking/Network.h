/*
 * Network.h
 *
 *  Created on: 31 May 2023
 */

#ifndef THREADS_NETWORKING_NETWORK_H_
#define THREADS_NETWORKING_NETWORK_H_

#include <string>

#include "Event.h"
#include "ThreadCommon.h"

class Network {
public:
  Network(ThreadCommon::QueueManager* qm);
  virtual ~Network();
  void taskFunction();
private:
  ThreadCommon::QueueManager* _qm;
  std::string ssid;
  std::string password;
  std::string ip;
};

void thread_network(void* pvParams);

#endif /* THREADS_NETWORKING_NETWORK_H_ */
