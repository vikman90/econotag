/*****************************************************************************/
/*                                                                           */
/* Sistemas Empotrados                                                       */
/* El "hola mundo" en la Redwire EconoTAG en C                               */
/*                                                                           */
/*****************************************************************************/

#include <stdint.h>
#include "system.h"

/*
 * Constantes relativas a la plataforma
 */

gpio_pin_t const LED_RED = gpio_pin_44;
gpio_pin_t const LED_GREEN = gpio_pin_45;
gpio_pin_t const BUTTON_S2_0 = gpio_pin_23;
gpio_pin_t const BUTTON_S2_1 = gpio_pin_27;
gpio_pin_t const BUTTON_S3_0 = gpio_pin_22;
gpio_pin_t const BUTTON_S3_1 = gpio_pin_26;

/*
 * Constantes relativas a la aplicacion
 */
uint32_t const DELAY = 0x10000;

/*****************************************************************************/

/*
 * Inicialización de los pines de E/S
 */
void gpio_init(void)
{
	gpio_set_pin_dir_output(LED_RED);
	gpio_set_pin_dir_output(LED_GREEN);
	gpio_set_pin_dir_output(BUTTON_S2_0);
	gpio_set_pin_dir_output(BUTTON_S3_0);
	gpio_set_pin(BUTTON_S2_0);
	gpio_set_pin(BUTTON_S3_0);
}

/*****************************************************************************/

/*
 * Retardo para el parpedeo
 */
void pause(void)
{
	uint32_t i;
	
	for (i = 0; i < DELAY; i++);
}

/*****************************************************************************/

/*
 * Máscara del led que se hará parpadear
 */
gpio_pin_t the_led;

/*
 * Consulta de boton
 */

void test_buttons()
{
	uint32_t result;
	gpio_get_pin(BUTTON_S2_1, &result);	
	
	if (result)
		the_led = LED_RED;
	else {
		gpio_get_pin(BUTTON_S3_1, &result);	
		
		if (result)
			the_led = LED_GREEN;
	}
}

/*
 * Programa principal
 */
int main ()
{
	gpio_init();
	the_led = LED_RED;

	while (1)
	{
		test_buttons();
		gpio_set_pin(the_led);
        pause();
		gpio_clear_pin(the_led);
        pause();
	}

	return 0;
}

/*****************************************************************************/

