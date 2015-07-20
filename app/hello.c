/*****************************************************************************/
/*                                                                           */
/* Sistemas Empotrados                                                       */
/* El "hola mundo" en la Redwire EconoTAG en C                               */
/*                                                                           */
/*****************************************************************************/

#include <stdint.h>
#include "system.h"

const uint8_t MESSAGE[] = "Error: Pulse g o r.\n\r";

// Asignacion de pines

#define LED_RED gpio_pin_44
#define LED_GREEN gpio_pin_45

/*****************************************************************************/

void print(const uint8_t *message) 
{
	uint32_t i;
	
	for (i = 0; message[i] != '\0'; i++)
		uart_send_byte(uart_1, message[i]);
}

// Programa principal

int main ()
{
	gpio_pin_t led = LED_RED;
	
	gpio_set_pin_dir_output(LED_RED);
	gpio_set_pin_dir_output(LED_GREEN);
	
	gpio_set_pin(led);

	while (1)
	{
		uint8_t byte = uart_receive_byte(uart_1);
		
		if (byte == 'r') {
			gpio_clear_pin(led);
			led = LED_RED;
			gpio_set_pin(led);
		} else if (byte == 'g') {
			gpio_clear_pin(led);
			led = LED_GREEN;
			gpio_set_pin(led);
		} else
			print(MESSAGE);
	}

	return 0;
}

/*****************************************************************************/

