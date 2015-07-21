/*
 * Sistemas operativos empotrados
 * Driver de las uart
 */

#include <fcntl.h>
#include <errno.h>
#include "system.h"
#include "circular_buffer.h"

/*****************************************************************************/

/**
 * Acceso estructurado a los registros de control de las uart del MC1322x
 */

typedef struct
{
	/* ESTA ESTRUCTURA SE DEFINIRÁ EN LA PRÁCTICA 10 */
	
	// El procesador MC1322x es little-endian
	// El bit menos significativo esta en la direccion base
	
	// UCON
	
	struct
	{
		uint32_t txe:1;
		uint32_t rxe:1;
		uint32_t pen:1;
		uint32_t ep:1;
		uint32_t st2:1;
		uint32_t sb:1;
		uint32_t contx:1;
		uint32_t tx_oen_b:1;
		uint32_t :1;
		uint32_t res:1;
		uint32_t xtim:1;
		uint32_t fcp:1;
		uint32_t fce:1;
		uint32_t mtxr:1;
		uint32_t mrxr:1;
		uint32_t tst:1;
	};
	
	// USTAT
	
	struct
	{
		uint32_t se:1;
		uint32_t pe:1;
		uint32_t fe:1;
		uint32_t toe:1;
		uint32_t roe:1;
		uint32_t rue:1;
		uint32_t rxrdy:1;
		uint32_t txrdy:1;
	};
	
	// Alineamiento a la direccion base
	
	union 
	{
		uint8_t udata; // Solo son validos los bits 7:0
		uint32_t _udata32;
	};
	
	// URXCON
	
	union
	{
		struct
		{
			uint32_t rx_fifo_addr_diff:6; // Bytes disponibles (validos) para leer
		};
		
		struct
		{
			uint32_t rxlevel:5;
		};
	};
	
	
	// UTXCON
	
	union
	{
		struct
		{
			uint32_t tx_fifo_addr_diff:6; // Bytes disponibles (vacios) para escribir
		};
		
		struct
		{
			uint32_t txlevel:5;
		};
	};
	
	// UCTS
	
	struct
	{
		uint32_t cts_level:5;
	};
	
	struct 
	{
		uint32_t ubrmod:16;
		uint32_t ubrinc:16;
	};
	
} uart_regs_t;

#define ENABLE 1
#define DISABLE 0
#define IRQ_ENABLED 0
#define IRQ_MASKED 1
#define TX_LEVEL 31
#define RX_LEVEL 1

/*****************************************************************************/

/**
 * Acceso estructurado a los pines de las uart del MC1322x
 */
typedef struct
{
	gpio_pin_t tx,rx,cts,rts;
} uart_pins_t;

/*****************************************************************************/

/**
 * Definición de las UARTS
 */
static volatile uart_regs_t* const uart_regs[uart_max] = {UART1_BASE, UART2_BASE};

static const uart_pins_t uart_pins[uart_max] = {
		{gpio_pin_14, gpio_pin_15, gpio_pin_16, gpio_pin_17},
		{gpio_pin_18, gpio_pin_19, gpio_pin_20, gpio_pin_21} };

static void uart_1_isr (void);
static void uart_2_isr (void);
static const itc_handler_t uart_irq_handlers[uart_max] = {uart_1_isr, uart_2_isr};

/*****************************************************************************/

/** 
 * Tamaño de los búferes circulares
 */
#define __UART_BUFFER_SIZE__	256

static volatile uint8_t uart_rx_buffers[uart_max][__UART_BUFFER_SIZE__];
static volatile uint8_t uart_tx_buffers[uart_max][__UART_BUFFER_SIZE__];

static volatile circular_buffer_t uart_circular_rx_buffers[uart_max];
static volatile circular_buffer_t uart_circular_tx_buffers[uart_max];

/*****************************************************************************/

/**
 * Gestión de las callbacks
 */
typedef struct
{
	uart_callback_t tx_callback;
	uart_callback_t rx_callback;
} uart_callbacks_t;

static volatile uart_callbacks_t uart_callbacks[uart_max];

/*****************************************************************************/

/**
 * Inicializa una uart
 * @param uart	Identificador de la uart
 * @param br	Baudrate
 * @param name	Nombre del dispositivo
 * @return		Cero en caso de éxito o -1 en caso de error.
 * 				La condición de error se indica en la variable global errno
 */
int32_t uart_init (uart_id_t uart, uint32_t br, const char *name)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LAS PRÁCTICAS 10, 11 y 12 */
	
	// Comprobacion de errores
	
	if (uart >= uart_max) {
		errno = ENODEV;
		return -1;
	}
	
	if (!name) {
		errno = EFAULT;
		return -1;
	}
	
	// Deshabilitar dispositivo y enmascarar interrupciones

	uart_regs[uart]->txe = DISABLE;
	uart_regs[uart]->rxe = DISABLE;
	uart_regs[uart]->mtxr = IRQ_MASKED;
	uart_regs[uart]->mrxr = IRQ_MASKED;
	
	// Fijar frecuencia de operacion
	
	uart_regs[uart]->ubrmod = 9999;
	uart_regs[uart]->ubrinc = (br * 9999) / (CPU_FREQ >> 4);
	
	// Habilitar dispositivo
	
	uart_regs[uart]->txe = ENABLE;
	uart_regs[uart]->rxe = ENABLE;
	
	// Funcion y direccion de los pines
	
	gpio_set_pin_dir_input(uart_pins[uart].tx);
	gpio_set_pin_dir_input(uart_pins[uart].rx);
	gpio_set_pin_dir_input(uart_pins[uart].cts);
	gpio_set_pin_dir_input(uart_pins[uart].rts);
	
	gpio_set_pin_func(uart_pins[uart].tx, gpio_func_alternate_1);
	gpio_set_pin_func(uart_pins[uart].rx, gpio_func_alternate_1);
	gpio_set_pin_func(uart_pins[uart].cts, gpio_func_alternate_1);
	gpio_set_pin_func(uart_pins[uart].rts, gpio_func_alternate_1);
	
	// Inicializacion L1
	
	circular_buffer_init(&uart_circular_rx_buffers[uart], (uint8_t *)uart_rx_buffers[uart], __UART_BUFFER_SIZE__);
	circular_buffer_init(&uart_circular_tx_buffers[uart], (uint8_t *)uart_tx_buffers[uart], __UART_BUFFER_SIZE__);
	
	uart_regs[uart]->txlevel = TX_LEVEL;
	uart_regs[uart]->rxlevel = RX_LEVEL;
	
	switch (uart) {
	case uart_1:
		itc_set_priority(itc_src_uart1, itc_priority_normal);
		itc_set_handler(itc_src_uart1, uart_1_isr);
		itc_enable_interrupt(itc_src_uart1);
		break;
	case uart_2:
		itc_set_priority(itc_src_uart2, itc_priority_normal);
		itc_set_handler(itc_src_uart2, uart_2_isr);
		itc_enable_interrupt(itc_src_uart2);
		break;
	default: ;
	}
	
	uart_regs[uart]->mrxr = IRQ_ENABLED;

	return 0;
}

/*****************************************************************************/

/**
 * Transmite un byte por la uart
 * Implementación del driver de nivel 0. La llamada se bloquea hasta que transmite el byte
 * @param uart	Identificador de la uart
 * @param c		El carácter
 */
void uart_send_byte (uart_id_t uart, uint8_t c)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 10 */
	
	uint32_t old_mtxr = uart_regs[uart]->mtxr;
	uart_regs[uart]->mtxr = IRQ_MASKED;
	
	while (!circular_buffer_is_empty(&uart_circular_tx_buffers[uart])) {
		while (!uart_regs[uart]->tx_fifo_addr_diff);
		uart_regs[uart]->udata = circular_buffer_read(&uart_circular_tx_buffers[uart]);
	}
	
	while (!uart_regs[uart]->tx_fifo_addr_diff);
	uart_regs[uart]->udata = c;
	
	uart_regs[uart]->mtxr = old_mtxr;
}

/*****************************************************************************/

/**
 * Recibe un byte por la uart
 * Implementación del driver de nivel 0. La llamada se bloquea hasta que recibe el byte
 * @param uart	Identificador de la uart
 * @return		El byte recibido
 */
uint8_t uart_receive_byte (uart_id_t uart)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 10 */
	
	uint8_t byte;
	uint32_t old_mrxr = uart_regs[uart]->mrxr;
	
	uart_regs[uart]->mrxr = IRQ_MASKED;
	
	if (!circular_buffer_is_empty(&uart_circular_rx_buffers[uart]))
		byte = circular_buffer_read(&uart_circular_rx_buffers[uart]);
	else {
		while (!uart_regs[uart]->rx_fifo_addr_diff);
		byte = uart_regs[uart]->udata;
	}
	
	uart_regs[uart]->mrxr = old_mrxr;
	return byte;
}

/*****************************************************************************/

/**
 * Transmisión de bytes
 * Implementación del driver de nivel 1. La llamada es no bloqueante y se realiza mediante interrupciones
 * @param uart	Identificador de la uart
 * @param buf	Búfer con los caracteres
 * @param count	Número de caracteres a escribir
 * @return	El número de bytes almacenados en el búfer de transmisión en caso de éxito o
 *              -1 en caso de error.
 * 		La condición de error se indica en la variable global errno
 */
ssize_t uart_send (uint32_t uart, char *buf, size_t count)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 11 */
	
	// count es size_t (unsigned) => no puede ser negativo
	
	if (uart >= uart_max) {
		errno = ENODEV;
		return -1;
	}
	
	if (!buf) {
		errno = EFAULT;
		return -1;
	}
	
	size_t i;
	
	uart_regs[uart]->mtxr = IRQ_MASKED;
	
	for (i = 0; i < count && !circular_buffer_is_full(&uart_circular_tx_buffers[uart]); i++)
		circular_buffer_write(&uart_circular_tx_buffers[uart], buf[i]);
		
	uart_regs[uart]->mtxr = IRQ_ENABLED;
	
    return i;
        
}

/*****************************************************************************/

/**
 * Recepción de bytes
 * Implementación del driver de nivel 1. La llamada es no bloqueante y se realiza mediante interrupciones
 * @param uart	Identificador de la uart
 * @param buf	Búfer para almacenar los bytes
 * @param count	Número de bytes a leer
 * @return	El número de bytes realmente leídos en caso de éxito o
 *              -1 en caso de error.
 * 		La condición de error se indica en la variable global errno
 */
ssize_t uart_receive (uint32_t uart, char *buf, size_t count)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 11 */

	// count es size_t (unsigned) => no puede ser negativo
	
	if (uart >= uart_max) {
		errno = ENODEV;
		return -1;
	}
	
	if (!buf) {
		errno = EFAULT;
		return -1;
	}
	
	size_t i;
	
	uart_regs[uart]->mrxr = IRQ_MASKED;
	
	for (i = 0; i < count && !circular_buffer_is_empty(&uart_circular_rx_buffers[uart]); i++)
		buf[i] = circular_buffer_read(&uart_circular_rx_buffers[uart]);
		
	uart_regs[uart]->mrxr = IRQ_ENABLED;
	
    return i;
}

/*****************************************************************************/

/**
 * Fija la función callback de recepción de una uart
 * @param uart	Identificador de la uart
 * @param func	Función callback. NULL para anular una selección anterior
 * @return	Cero en caso de éxito o -1 en caso de error.
 * 		La condición de error se indica en la variable global errno
 */
int32_t uart_set_receive_callback (uart_id_t uart, uart_callback_t func)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 11 */

	if (uart >= uart_max) {
		errno = ENODEV;
		return -1;
	}
	
	uart_callbacks[uart].rx_callback = func;
	return 0;
}

/*****************************************************************************/

/**
 * Fija la función callback de transmisión de una uart
 * @param uart	Identificador de la uart
 * @param func	Función callback. NULL para anular una selección anterior
 * @return	Cero en caso de éxito o -1 en caso de error.
 * 		La condición de error se indica en la variable global errno
 */
int32_t uart_set_send_callback (uart_id_t uart, uart_callback_t func)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 11 */

	if (uart >= uart_max) {
		errno = ENODEV;
		return -1;
	}
	
	uart_callbacks[uart].tx_callback = func;
	return 0;
}

/*****************************************************************************/

/**
 * Manejador genérico de interrupciones para las uart.
 * Cada isr llamará a este manejador indicando la uart en la que se ha
 * producido la interrupción.
 * Lo declaramos inline para reducir la latencia de la isr
 * @param uart	Identificador de la uart
 */
static inline void uart_isr (uart_id_t uart)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 11 */
	
	if (uart_regs[uart]->rxrdy) {
		while (uart_regs[uart]->rx_fifo_addr_diff && !circular_buffer_is_full(&uart_circular_rx_buffers[uart]))
			circular_buffer_write(&uart_circular_rx_buffers[uart], uart_regs[uart]->udata);
		
		if (uart_callbacks[uart].rx_callback)
			uart_callbacks[uart].rx_callback();
		
		if (circular_buffer_is_full(&uart_circular_rx_buffers[uart]))
			uart_regs[uart]->mrxr = IRQ_MASKED;
	}
	
	if (uart_regs[uart]->txrdy) {
		while (uart_regs[uart]->tx_fifo_addr_diff && !circular_buffer_is_empty(&uart_circular_tx_buffers[uart]))
			uart_regs[uart]->udata = circular_buffer_read(&uart_circular_tx_buffers[uart]);
		
		if (uart_callbacks[uart].tx_callback)
			uart_callbacks[uart].tx_callback();
		
		if (circular_buffer_is_empty(&uart_circular_tx_buffers[uart]))
			uart_regs[uart]->mtxr = IRQ_MASKED;
	}
}

/*****************************************************************************/

/**
 * Manejador de interrupciones para la uart1
 */
static void uart_1_isr (void)
{
	uart_isr(uart_1);
}

/*****************************************************************************/

/**
 * Manejador de interrupciones para la uart2
 */
static void uart_2_isr (void)
{
	uart_isr(uart_2);
}

/*****************************************************************************/
