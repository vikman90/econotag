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

void receive_callback()
{	
	char data = getchar();
	
	if (data == 'r')
		blink_red_led = !blink_red_led;
	
	if (data == 'g')
		blink_green_led = !blink_green_led;

}

// Programa principal

int main()
{	
	gpio_set_pin_dir_output(LED_RED);
	gpio_set_pin_dir_output(LED_GREEN);
	uart_set_receive_callback(uart_1, receive_callback);

	while (1)
	{
		gpio_set_pin(LED_RED);
		gpio_set_pin(LED_GREEN);
		
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

