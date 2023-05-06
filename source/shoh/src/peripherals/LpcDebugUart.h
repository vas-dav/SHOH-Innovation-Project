/*
 * LpcDebugUart.h
 *
 *  Created on: 6.5.2023
 */

#ifndef LPCDEBUGUART_H_
#define LPCDEBUGUART_H_

#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Fmutex.h"
#include "LpcUart.h"

struct LpcDebugUartConfig {
	LPC_USART0_T *pUART;
	uint32_t speed;
	uint32_t data;
	LpcPinMap tx;
	LpcPinMap rx;
};


class LpcDebugUart {
public:
	LpcDebugUart(const LpcDebugUartConfig &cfg);
	LpcDebugUart(const LpcDebugUart &) = delete;
	virtual ~LpcDebugUart();
	int  free(); /* get amount of free space in transmit buffer */
	int  peek(); /* get number of received characters in receive buffer */
	int  write(char c);
	int  write(const char *s);
	int  write(const char *buffer, int len);
	int  read(char &c); /* get a single character. Returns number of characters read --> returns 0 if no character is available */
	int  read(char *buffer, int len);
	int  read(char *buffer, int len, TickType_t total_timeout, TickType_t ic_timeout = portMAX_DELAY);
	void txbreak(bool brk); /* set break signal on */
	bool rxbreak(); /* check if break is received */
	void speed(int bps); /* change transmission speed */
	bool txempty();
	void set_on_receive(void(*cb)(void));

	void isr(portBASE_TYPE *hpw); /* ISR handler. This will be called by the HW ISR handler. Do not call from application */
private:
	LPC_USART0_T *uart;
	IRQn_Type irqn;
	/* currently we support only fixed size ring buffers */
	static const int UART_RB_SIZE = 128;
	/* Transmit and receive ring buffers */
	RINGBUFF_T txring;
	RINGBUFF_T rxring;
	uint8_t rxbuff[UART_RB_SIZE];
	uint8_t txbuff[UART_RB_SIZE];
	TaskHandle_t notify_rx;
	TaskHandle_t notify_tx;
	void (*on_receive)(void); // callback for received data notifications
	Fmutex read_mutex;
	Fmutex write_mutex;
};

#endif /* LPCUART_H_ */
