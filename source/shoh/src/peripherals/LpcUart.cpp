/*
 * LpcUart.cpp
 *
 *  Created on: 4.2.2019
 *      Author: keijo
 */

#include <cstring>
#include <mutex>
#include "LpcUart.h"

/* shoh: Important differences
 * We don't have movable pins -> not needed.
 *
 * Muxing is like this:
 * Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
 * Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
 */

static LpcUart *u1;
static LpcUart *u2;
static LpcUart *u3;
static LpcUart *u4;

extern "C" {
/**
 * @brief	UART interrupt handler using ring buffers
 * @par shoh: Unlike in lpc15xx, lpc11u68 has shared interrupts for 1 and 4, 2 and 3.
 * shoh: Not sure how exactly it should be handled. UART0 is completely different compared to 1-4.
 * @return	Nothing
 */
void USART1_4_IRQHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if(u1) {
		u1->isr(&xHigherPriorityTaskWoken);
	}

	if(u4) {
		u4->isr(&xHigherPriorityTaskWoken);
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

void USART2_3_IRQHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if(u2) {
		u2->isr(&xHigherPriorityTaskWoken);
	}

	if(u3) {
		u3->isr(&xHigherPriorityTaskWoken);
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

}


void LpcUart::isr(portBASE_TYPE *hpw) {
	// get interrupt status for notifications
	uint32_t istat = Chip_UARTN_GetIntStatus(uart);

	// chip library is used to handle receive and transmit
	Chip_UARTN_IRQRBHandler(uart, &rxring, &txring);

	// notify of the events handled
	if(notify_rx && (istat & UARTN_STAT_RXRDY) ) vTaskNotifyGiveFromISR(notify_rx, hpw);
	if(notify_tx && (istat & UARTN_STAT_TXRDY) ) vTaskNotifyGiveFromISR(notify_tx, hpw);
	if(on_receive && (istat & UARTN_STAT_RXRDY) ) on_receive();
}

bool LpcUart::init = false;

LpcUart::LpcUart(const LpcUartConfig &cfg) {
	//shoh: removed handshakes and movable pins.
	bool use_rts = false; //(cfg.rts.port >= 0);
	bool use_cts = false; //(cfg.cts.port >= 0);

	if(!init) {
		init = true;
		/* Before setting up the UART, the global UART clock for USARTS 1-4
		 * must first be setup. This requires setting the UART divider and
		 * the UART base clock rate to 16x the maximum UART rate for all
		 * UARTs.
		 * */
		/* Use main clock rate as base for UART baud rate divider */
		Chip_Clock_SetUSARTNBaseClockRate(Chip_Clock_GetMainClockRate(), false);
	}

	uart = nullptr; // set default value before checking which UART to configure

	if(cfg.pUART == LPC_USART1) {
		if(u1) return; // already exists
		else u1 = this;
		irqn = USART1_4_IRQn; //Shares interrupt with 4
	}
	else if(cfg.pUART == LPC_USART2) {
		if(u2) return; // already exists
		else u2 = this;
		irqn = USART2_3_IRQn; //Shares interrupt with 3
	}
	else if(cfg.pUART == LPC_USART3) {
		if(u3) return; // already exists
		else u3 = this;
		irqn = USART2_3_IRQn;
	}
	else if(cfg.pUART == LPC_USART4) {
		if(u4) return; // already exists
		else u4 = this;
		irqn = USART1_4_IRQn;
	}
	else {
		return;
	}

	uart = cfg.pUART; // set the actual value after validity checking


	if(cfg.tx.port >= 0) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, cfg.tx.port, cfg.tx.pin, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	}

	if(cfg.rx.port >= 0) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, cfg.rx.port, cfg.rx.pin, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	}

	if(use_cts) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, cfg.cts.port, cfg.cts.pin, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	}

	if(use_rts) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, cfg.rts.port, cfg.rts.pin, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	}

	notify_rx = nullptr;
	notify_tx = nullptr;
	on_receive = nullptr;
	/* Setup UART */
	Chip_UARTN_Init(uart);
	Chip_UARTN_ConfigData(uart, cfg.data);
	Chip_UARTN_SetBaud(uart, cfg.speed);

	if(use_rts && cfg.rs485) {
		uart->CFG |= (1 << 20); // enable rs485 mode
		//uart->CFG |= (1 << 18); // OE turnaraound time
		uart->CFG |= (1 << 21);// driver enable polarity (active high)
	}

	Chip_UARTN_Enable(uart);
	Chip_UARTN_TXEnable(uart);

	/* Before using the ring buffers, initialize them using the ring
	   buffer init function */
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RB_SIZE);
	RingBuffer_Init(&txring, txbuff, 1, UART_RB_SIZE);


	/* Enable receive data and line status interrupt */
	Chip_UARTN_IntEnable(uart, UARTN_INTEN_RXRDY);
	Chip_UARTN_IntDisable(uart, UARTN_INTEN_TXRDY);	/* May not be needed */

	NVIC_SetPriority(irqn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
	/* Enable UART interrupt */
	NVIC_EnableIRQ(irqn);
}

LpcUart::~LpcUart() {
	if(uart != nullptr) {
		NVIC_DisableIRQ(irqn);
		Chip_UARTN_IntDisable(uart, UARTN_INTEN_RXRDY);
		Chip_UARTN_IntDisable(uart, UARTN_INTEN_TXRDY);

		if(uart == LPC_USART1) {
			u1 = nullptr;
		}
		else if(uart == LPC_USART2) {
			u2 = nullptr;
		}
		else if(uart == LPC_USART3) {
			u3 = nullptr;
		}
		else if(uart == LPC_USART4) {
			u4 = nullptr;
		}
	}
}

void LpcUart::set_on_receive(void(*cb)(void))
{
	on_receive = cb;
}


int  LpcUart::free()
{
	std::lock_guard<Fmutex> lock(write_mutex);

	return UART_RB_SIZE - RingBuffer_GetCount(&txring);
}

int  LpcUart::peek()
{
	std::lock_guard<Fmutex> lock(read_mutex);

	return RingBuffer_GetCount(&rxring);
}

int  LpcUart::read(char &c)
{
	return read(&c, 1);
}

int  LpcUart::read(char *buffer, int len)
{
	std::lock_guard<Fmutex> lock(read_mutex);

	if(RingBuffer_GetCount(&rxring) <= 0) {
		notify_rx = xTaskGetCurrentTaskHandle();
		while(RingBuffer_GetCount(&rxring) <= 0) {
			ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		}
		notify_rx = nullptr;
	}

	return Chip_UARTN_ReadRB(uart, &rxring, buffer, len);
}


int  LpcUart::read(char *buffer, int len, TickType_t total_timeout, TickType_t ic_timeout)
{
	std::lock_guard<Fmutex> lock(read_mutex);

	// we can't read more than ring buffer size at a time
	if(len > UART_RB_SIZE) len = UART_RB_SIZE;

	TimeOut_t timeoutState;
	vTaskSetTimeOutState(&timeoutState);

	notify_rx = xTaskGetCurrentTaskHandle();
	while(RingBuffer_GetCount(&rxring) < len && xTaskCheckForTimeOut(&timeoutState, &total_timeout) == pdFALSE) {
		TickType_t timeout = total_timeout > ic_timeout ? ic_timeout : total_timeout;
		 if(ulTaskNotifyTake( pdTRUE, timeout ) == 0) break;
	}
	notify_rx = nullptr;

	return Chip_UARTN_ReadRB(uart, &rxring, buffer, len);;
}

int LpcUart::write(char c)
{
	return write(&c, 1);
}

int LpcUart::write(const char *s)
{
	return write(s, strlen(s));
}

int LpcUart::write(const char *buffer, int len)
{
	std::lock_guard<Fmutex> lock(write_mutex);

	int pos = 0;
	notify_tx = xTaskGetCurrentTaskHandle();

	while(len > pos) {
		// restrict single write to ring buffer size
		int size = (len - pos) > UART_RB_SIZE ? UART_RB_SIZE : (len - pos);

		// wait until we have space in the ring buffer
		while(UART_RB_SIZE - RingBuffer_GetCount(&txring) < size) {
			ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		}
		pos += Chip_UARTN_SendRB(uart, &txring, buffer+pos, size);
	}
	notify_tx = nullptr;

	return pos;
}

void LpcUart::txbreak(bool brk)
{
	// break handling not implemented yet
}

bool LpcUart::rxbreak()
{
	// break handling not implemented yet
	return false;
}

void LpcUart::speed(int bps)
{
	std::lock_guard<Fmutex> lockw(write_mutex);
	std::lock_guard<Fmutex> lockr(read_mutex);

	Chip_UARTN_SetBaud(uart, bps);
}

bool LpcUart::txempty()
{
	std::lock_guard<Fmutex> lock(write_mutex);

	return (RingBuffer_GetCount(&txring) == 0);
}
