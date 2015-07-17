/*****************************************************************************/
/*                                                                           */
/* Sistemas Empotrados                                                       */
/* El "hola mundo" en la Redwire EconoTAG en C                               */
/*                                                                           */
/*****************************************************************************/

#include <stdint.h>

/*
 * Constantes relativas a la plataforma
 */

/* Dirección del registro de control de dirección del GPIO32-GPIO63 */
volatile uint32_t * const reg_gpio_pad_dir1    = (uint32_t *) 0x80000004;

/* Dirección del registro de activación de bits del GPIO32-GPIO63 */
volatile uint32_t * const reg_gpio_data_set1   = (uint32_t *) 0x8000004c;

/* Dirección del registro de limpieza de bits del GPIO32-GPIO63 */
volatile uint32_t * const reg_gpio_data_reset1 = (uint32_t *) 0x80000054;

/* El led rojo está en el GPIO 44 (el bit 12 de los registros GPIO_X_1) */
uint32_t const led_red_mask = (1 << (44-32));

// Led verde (GPIO 45)
uint32_t const led_green_mask = (1 << (45-32));

// Registro de control de dirección del GPIO00-GPIO31
volatile uint32_t * const reg_gpio_pad_dir0 = 0x80000000;

// Registro de activación de bits del GPIO00-GPIO31
volatile uint32_t * const reg_gpio_pad_set0 = 0x80000048;

// Registro de consulta de bits del GPIO00-GPIO31
volatile uint32_t * const reg_gpio_data0 = 0x80000008;

// Boton S2 (GPIO 23 y 27)
uint32_t const button_s2_mask0 = 1 << 23;
uint32_t const button_s2_mask1 = 1 << 27;

// Boton S3 (GPIO 22 y 26)
uint32_t const button_s3_mask0 = 1 << 22;
uint32_t const button_s3_mask1 = 1 << 26;

/*
 * Constantes relativas a la aplicacion
 */
uint32_t const delay = 0x10000;
 
/*****************************************************************************/

/*
 * Inicialización de los pines de E/S
 */
void gpio_init(void)
{
	/* Configuramos el GPIO44 y GPIO45 para que sean de salida */
	*reg_gpio_pad_dir1 = led_red_mask | led_green_mask;
	
	/* Configuramos el GPIO22 y GPIO23 para que sean de salida */
	*reg_gpio_pad_dir0 = button_s2_mask0 | button_s3_mask0;
}

/*****************************************************************************/

/*
 * Enciende los leds indicados en la máscara
 * @param mask Máscara para seleccionar leds
 */
void leds_on (uint32_t mask)
{
	/* Encendemos los leds indicados en la máscara */
	*reg_gpio_data_set1 = mask;
}

/*****************************************************************************/

/*
 * Apaga los leds indicados en la máscara
 * @param mask Máscara para seleccionar leds
 */
void leds_off (uint32_t mask)
{
	/* Apagamos los leds indicados en la máscara */
	*reg_gpio_data_reset1 = mask;
}

/*****************************************************************************/

/*
 * Retardo para el parpedeo
 */
void pause(void)
{
        uint32_t i;
	for (i=0 ; i<delay ; i++);
}

/*****************************************************************************/

/*
 * Máscara del led que se hará parpadear
 */
uint32_t the_led;

/*
 * Consulta de boton
 */

void test_buttons()
{
	uint32_t result = *reg_gpio_data0;
	
	if (result & button_s2_mask1)
		the_led = led_red_mask;
	else if (result & button_s3_mask1)
		the_led = led_green_mask;
}

/*
 * Programa principal
 */
int main ()
{
	gpio_init();
	the_led = led_red_mask;

	while (1)
	{
		test_buttons();
		
		leds_on(the_led);
        pause();

		leds_off(the_led);
        pause();
	}

        return 0;
}

/*****************************************************************************/

