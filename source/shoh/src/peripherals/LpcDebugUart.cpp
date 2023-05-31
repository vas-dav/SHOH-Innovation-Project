/*
 * LpcDebugUart.cpp
 *
 *  Created on: 6.5.2023
 */

#include <cstring>
#include <mutex>
#include "LpcDebugUart.h"

/* shoh: Important differences
 * We don't have movable pins -> not needed.
 *
 * USART0 (the only debug uart):
 * Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
 * Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
 */

static LpcDebugUart *u0;

extern "C" {
/**
 * @brief	UART interrupt handler using ring buffers
 * @return	Nothing
 */
void USART0_IRQHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if(u0) {
		u0->isr(&xHigherPriorityTaskWoken);
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);	
}

}


void LpcDebugUart::isr(portBASE_TYPE *hpw) {
	// get interrupt status for notifications
	uint32_t istat = Chip_UART0_ReadLineStatus(uart);

	// chip library is used to handle receive and transmit
	Chip_UART0_IRQRBHandler(uart, &rxring, &txring);

	// notify (hopefully) of the events handled
	if(notify_rx && (istat & UART0_LSR_RDR) ) vTaskNotifyGiveFromISR(notify_rx, hpw);
	if(notify_tx && (istat & UART0_LSR_THRE) ) vTaskNotifyGiveFromISR(notify_tx, hpw);
	if(on_receive && (istat & UART0_LSR_RDR) ) on_receive();
}

LpcDebugUart::LpcDebugUart(const LpcDebugUartConfig &cfg) {
	uart = nullptr; // set default value before checking which UART to configure

	if(cfg.pUART == LPC_USART0) {
		if(u0) return; // already exists
		else u0 = this;
		irqn = USART0_IRQn;
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

	notify_rx = nullptr;
	notify_tx = nullptr;
	on_receive = nullptr;
	/* Setup UART */
	Chip_UART0_Init(uart);
	Chip_UART0_SetBaud(LPC_USART0, cfg.speed);
	Chip_UART0_ConfigData(LPC_USART0, cfg.data);
	Chip_UART0_SetupFIFOS(LPC_USART0, (UART0_FCR_FIFO_EN | UART0_FCR_TRG_LEV2));
	Chip_UART0_TXEnable(uart);

	/* Before using the ring buffers, initialize them using the ring
	   buffer init function */
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RB_SIZE);
	RingBuffer_Init(&txring, txbuff, 1, UART_RB_SIZE);


	/* Enable receive data and line status interrupt */
	Chip_UART0_IntEnable(uart, UART0_IER_RBRINT | UART0_IER_RLSINT);

	NVIC_SetPriority(irqn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
	/* Enable UART interrupt */
	NVIC_EnableIRQ(irqn);
}

LpcDebugUart::~LpcDebugUart() {
	if(uart != nullptr) {
		NVIC_DisableIRQ(irqn);
		Chip_UART0_IntDisable(uart, UART0_IER_RBRINT | UART0_IER_RLSINT);

		if(uart == LPC_USART0) {
			u0 = nullptr;
		}
	}
}

void LpcDebugUart::set_on_receive(void(*cb)(void))
{
	on_receive = cb;
}


int  LpcDebugUart::free()
{
	std::lock_guard<Fmutex> lock(write_debug_mutex);

	return UART_RB_SIZE - RingBuffer_GetCount(&txring);
}

int  LpcDebugUart::peek()
{
	std::lock_guard<Fmutex> lock(read_debug_mutex);

	return RingBuffer_GetCount(&rxring);
}

int  LpcDebugUart::read(char &c)
{
	return read(&c, 1);
}

int  LpcDebugUart::read(char *buffer, int len)
{
	std::lock_guard<Fmutex> lock(read_debug_mutex);

	if(RingBuffer_GetCount(&rxring) <= 0) {
		notify_rx = xTaskGetCurrentTaskHandle();
		while(RingBuffer_GetCount(&rxring) <= 0) {
			ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		}
		notify_rx = nullptr;
	}

	return Chip_UART0_ReadRB(uart, &rxring, buffer, len);
}


int  LpcDebugUart::read(char *buffer, int len, TickType_t total_timeout, TickType_t ic_timeout)
{
	std::lock_guard<Fmutex> lock(read_debug_mutex);

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

	return Chip_UART0_ReadRB(uart, &rxring, buffer, len);;
}

int LpcDebugUart::write(char c)
{
	return write(&c, 1);
}

int LpcDebugUart::write(const char *s)
{
	return write(s, strlen(s));
}

int LpcDebugUart::write(const char *buffer, int len)
{
	std::lock_guard<Fmutex> lock(write_debug_mutex);

	int pos = 0;
	notify_tx = xTaskGetCurrentTaskHandle();

	while(len > pos) {
		// restrict single write to ring buffer size
		int size = (len - pos) > UART_RB_SIZE ? UART_RB_SIZE : (len - pos);

		// wait until we have space in the ring buffer
		while(UART_RB_SIZE - RingBuffer_GetCount(&txring) < size) {
			ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		}
		pos += Chip_UART0_SendRB(uart, &txring, buffer+pos, size);
	}
	notify_tx = nullptr;

	return pos;
}

void LpcDebugUart::txbreak(bool brk)
{
	// break handling not implemented yet
}

bool LpcDebugUart::rxbreak()
{
	// break handling not implemented yet
	return false;
}

void LpcDebugUart::speed(int bps)
{
	std::lock_guard<Fmutex> lockw(write_debug_mutex);
	std::lock_guard<Fmutex> lockr(read_debug_mutex);

	Chip_UART0_SetBaud(uart, bps);
}

bool LpcDebugUart::txempty()
{
	std::lock_guard<Fmutex> lock(write_debug_mutex);

	return (RingBuffer_GetCount(&txring) == 0);
}
