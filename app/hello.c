/*****************************************************************************/
/*                                                                           */
/* Sistemas Empotrados                                                       */
/* El "hola mundo" en la Redwire EconoTAG en C                               */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "system.h"

// Asignacion de pines

#define LED_RED gpio_pin_44
#define LED_GREEN gpio_pin_45
#define DELAY 0x100000
#define RX_MAX 256

static uint32_t blink_red_led = 1;
static uint32_t blink_green_led = 1;
static char rx_buffer[RX_MAX];

/*****************************************************************************/

void pause(uint32_t delay)
{
    uint32_t i;
	for (i = 0; i < delay ; i++);
}

// Programa principal

int main()
{	
	gpio_set_pin_dir_output(LED_RED);
	gpio_set_pin_dir_output(LED_GREEN);
	
	while (1)
	{
		//gpio_set_pin(LED_RED);
		//gpio_set_pin(LED_GREEN);
		
		// El problema esta en que se llama a uart_receive una sola vez en toda la ejecucion, aparentemente porque recibe EOF
		
		char byte = getchar();
		iprintf("Leido: %c\n\r", byte);
		
		if (ferror(stdin))
			gpio_set_pin(LED_RED); // no se enciende
			
		freopen(UART1_NAME, O_RDONLY, stdin);
		
		if (byte == 'r')
			blink_red_led = !blink_red_led;
		else if (byte == 'g')
			blink_green_led = !blink_green_led;
		
		pause(DELAY);
		
		if (blink_red_led)
			gpio_clear_pin(LED_RED);
		
		if (blink_green_led)
			gpio_clear_pin(LED_GREEN);
		
		pause(DELAY);
	}

	return 0;
}

/*****************************************************************************/

