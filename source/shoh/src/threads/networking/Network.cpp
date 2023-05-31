/*
 * Network.cpp
 *
 *  Created on: 31 May 2023
 */

#include "Network.h"
#include "esp8266_socket.h"
#include "Log.h"

#define NETWORK_DEMO 0

Network::Network(ThreadCommon::QueueManager* qm) : _qm(qm)
{
  this->ssid = "SSID";
  this->password = "PASSWORD";
  this->ip = "127.0.0.1";
}

Network::~Network() {}

void Network::taskFunction()
{
  #if NETWORK_DEMO

  int soc = esp_socket(this->ssid.c_str(), this->password.c_str());
  LOG_INFO("Connected to network");
	int stat = esp_connect(soc, this->ip.c_str(), 5000);

  #endif

  for (;;)
  {
    #if NETWORK_DEMO

    LOG_DEBUG("ESP socket status: %d", stat);
		if(stat == 0)
			stat = esp_connect(soc, this->ip.c_str(), 5000);

    #else

    vTaskDelay(portMAX_DELAY);

    #endif
  }
  
}

void thread_network(void* pvParams)
{
  ThreadCommon::CommonManagers * manager = static_cast<ThreadCommon::CommonManagers*>(pvParams);
	Network n(manager->qm);
	n.taskFunction();
}