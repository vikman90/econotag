/*
 * Sistemas operativos empotrados
 * Driver para el GPIO del MC1322x
 */

#include "system.h"

/*****************************************************************************/

/**
 * Acceso estructurado a los registros de control del gpio del MC1322x
 */
typedef struct
{
	/* ESTA ESTRUCTURA SE DEFINIRÁ EN LA PRÁCTICA 9 */
	
	uint32_t GPIO_PAD_DIR0;
	uint32_t GPIO_PAD_DIR1;
	uint32_t GPIO_DATA0;
	uint32_t GPIO_DATA1;
	uint32_t GPIO_PAD_PU_EN0;
	uint32_t GPIO_PAD_PU_EN1;
	uint32_t GPIO_FUNC_SEL0;
	uint32_t GPIO_FUNC_SEL1;
	uint32_t GPIO_FUNC_SEL2;
	uint32_t GPIO_FUNC_SEL3;
	uint32_t GPIO_DATA_SEL0;
	uint32_t GPIO_DATA_SEL1;
	uint32_t GPIO_PAD_PU_SEL0;
	uint32_t GPIO_PAD_PU_SEL1;
	uint32_t GPIO_PAD_HYST_EN0;
	uint32_t GPIO_PAD_HYST_EN1;
	uint32_t GPIO_PAD_KEEP0;
	uint32_t GPIO_PAD_KEEP1;
	uint32_t GPIO_DATA_SET0;
	uint32_t GPIO_DATA_SET1;
	uint32_t GPIO_DATA_RESET0;
	uint32_t GPIO_DATA_RESET1;
	uint32_t GPIO_PAD_DIR_SET0;
	uint32_t GPIO_PAD_DIR_SET1;
	uint32_t GPIO_PAD_DIR_RESET0;
	uint32_t GPIO_PAD_DIR_RESET1;
} gpio_regs_t;

static volatile gpio_regs_t* const gpio_regs = GPIO_BASE;

/*****************************************************************************/

/**
 * Fija la dirección los pines seleccionados en la máscara como de entrada
 *
 * @param 	port 	Puerto
 * @param 	mask 	Máscara para seleccionar los pines
 * @return	gpio_no_error si los parámetros de entrada son corectos o
 *			gpio_invalid_parameter en otro caso
 */
inline gpio_err_t gpio_set_port_dir_input (gpio_port_t port, uint32_t mask)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 9 */

	if (port == gpio_port_0)
		gpio_regs->GPIO_PAD_DIR_RESET0 = mask;
	else
		gpio_regs->GPIO_PAD_DIR_RESET1 = mask;
	
	return gpio_no_error;
}

/*****************************************************************************/

/**
 * Fija la dirección los pines seleccionados en la máscara como de salida
 *
 * @param	port 	Puerto
 * @param	mask 	Máscara para seleccionar los pines
 * @return	gpio_no_error si los parámetros de entrada son corectos o
 *			gpio_invalid_parameter en otro caso
 */
inline gpio_err_t gpio_set_port_dir_output (gpio_port_t port, uint32_t mask)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 9 */
	
	if (port == gpio_port_0)
		gpio_regs->GPIO_PAD_DIR_SET0 = mask;
	else
		gpio_regs->GPIO_PAD_DIR_SET1 = mask;

	return gpio_no_error;
}

/*****************************************************************************/

/**
 * Fija la dirección del pin indicado como de entrada
 *
 * @param	pin 	Número de pin
 * @return	gpio_no_error si los parámetros de entrada son corectos o
 *			gpio_invalid_parameter en otro caso
 */
inline gpio_err_t gpio_set_pin_dir_input (gpio_pin_t pin)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 9 */
	
	if (pin < 32)
		gpio_regs->GPIO_PAD_DIR_RESET0 = 1 << pin;
	else
		gpio_regs->GPIO_PAD_DIR_RESET1 = 1 << (pin - 32);

	return gpio_no_error;
}

/*****************************************************************************/

/**
 * Fija la dirección del pin indicado como de salida
 *
 * @param	pin 	Número de pin
 * @return	gpio_no_error si los parámetros de entrada son corectos o
 *			gpio_invalid_parameter en otro caso
 */
inline gpio_err_t gpio_set_pin_dir_output (gpio_pin_t pin)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 9 */
	
	if (pin < 32)
		gpio_regs->GPIO_PAD_DIR_SET0 = 1 << pin;
	else
		gpio_regs->GPIO_PAD_DIR_SET1 = 1 << (pin - 32);

	return gpio_no_error;
}

/*****************************************************************************/

/**
 * Escribe unos en los pines seleccionados en la máscara
 *
 * @param	port 	Puerto
 * @param	mask 	Máscara para seleccionar los pines
 * @return	gpio_no_error si los parámetros de entrada son corectos o
 *			gpio_invalid_parameter en otro caso
 */
inline gpio_err_t gpio_set_port (gpio_port_t port, uint32_t mask)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 9 */
	
	if (port == gpio_port_0)
		gpio_regs->GPIO_DATA_SET0 = mask;
	else
		gpio_regs->GPIO_DATA_SET1 = mask;

	return gpio_no_error;
}

/*****************************************************************************/

/**
 * Escribe ceros en los pines seleccionados en la máscara
 *
 * @param	port 	Puerto
 * @param	mask 	Máscara para seleccionar los pines
 * @return	gpio_no_error si los parámetros de entrada son corectos o
 *			gpio_invalid_parameter en otro caso
 */
inline gpio_err_t gpio_clear_port (gpio_port_t port, uint32_t mask)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 9 */
	
	if (port == gpio_port_0)
		gpio_regs->GPIO_DATA_RESET0 = mask;
	else
		gpio_regs->GPIO_DATA_RESET1 = mask;

	return gpio_no_error;
}

/*****************************************************************************/

/**
 * Escribe un uno en el pin indicado
 *
 * @param	pin 	Número de pin
 * @return	gpio_no_error si los parámetros de entrada son corectos o
 *			gpio_invalid_parameter en otro caso
 */
inline gpio_err_t gpio_set_pin (gpio_pin_t pin)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 9 */
	
	if (pin < 32) {
		if (!(gpio_regs->GPIO_PAD_DIR0 & (1 << pin)))
			return gpio_invalid_parameter;
		else
			gpio_regs->GPIO_DATA_SET0 = 1 << pin;
	} else {
		if (!(gpio_regs->GPIO_PAD_DIR1 & (1 << (pin - 32))))
			return gpio_invalid_parameter;
		else
			gpio_regs->GPIO_DATA_SET1 = 1 << (pin - 32);
	}

	return gpio_no_error;
}

/*****************************************************************************/

/**
 * Escribe un cero en el pin indicado
 *
 * @param	pin 	Número de pin
 * @return	gpio_no_error si los parámetros de entrada son corectos o
 *			gpio_invalid_parameter en otro caso
 */
inline gpio_err_t gpio_clear_pin (gpio_pin_t pin)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 9 */
	
	if (pin < 32) {
		if (!(gpio_regs->GPIO_PAD_DIR0 & (1 << pin)))
			return gpio_invalid_parameter;
		else
			gpio_regs->GPIO_DATA_RESET0 = 1 << pin;
	} else {
		if (!(gpio_regs->GPIO_PAD_DIR1 & (1 << (pin - 32))))
			return gpio_invalid_parameter;
		else
			gpio_regs->GPIO_DATA_RESET1 = 1 << (pin - 32);
	}

	return gpio_no_error;
}

/*****************************************************************************/

/**
 * Lee el valor de los pines de un puerto
 *
 * @param	port	  Puerto
 * @param	port_data Valor de los pines del puerto
 * @return	gpio_no_error si los parámetros de entrada son corectos o
 *			  gpio_invalid_parameter en otro caso
 */
inline gpio_err_t gpio_get_port (gpio_port_t port, uint32_t *port_data)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 9 */
	
	*port_data = (port == gpio_port_0) ? gpio_regs->GPIO_DATA0 : gpio_regs->GPIO_DATA1;
	return gpio_no_error;
}

/*****************************************************************************/

/**
 * Lee el valor del pin indicado
 *
 * @param	pin	  Número de pin
 * @param       pin_data  Cero si el pin está a cero, distinto de cero en otro caso
 * @return	gpio_no_error si los parámetros de entrada son corectos o
 *			  gpio_invalid_parameter en otro caso
 */
inline gpio_err_t gpio_get_pin (gpio_pin_t pin, uint32_t *pin_data)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 9 */
	
	if (pin < 32) {
		if (gpio_regs->GPIO_PAD_DIR0 & (1 << pin))
			return gpio_invalid_parameter;
	} else
		if (gpio_regs->GPIO_PAD_DIR1 & (1 << (pin - 32)))
			return gpio_invalid_parameter;
		
	*pin_data = (pin < 32) ? (gpio_regs->GPIO_DATA0 & (1 << pin)) >> pin : (gpio_regs->GPIO_DATA1 & (1 << (pin - 32))) >> (pin - 32);

	return gpio_no_error;
}

/*****************************************************************************/

/**
 * Fija los pines seleccionados a una función
 *
 * @param	port 	Puerto
 * @param	func	Función
 * @param	mask	Máscara para seleccionar los pines
 * @return	gpio_no_error si los parámetros de entrada son corectos o
 *			gpio_invalid_parameter en otro caso
 */
inline gpio_err_t gpio_set_port_func (gpio_port_t port, gpio_func_t func, uint32_t mask)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 9 */
	
	int pin = 0;
	
	while (mask) {
		if (mask & 1)
			gpio_set_pin_func(pin, func);
			
		pin++;
		mask >>= 1;
	}

	return gpio_no_error;
}

/*****************************************************************************/

/**
 * Fija el pin seleccionado a una función
 *
 * @param	pin 	Pin
 * @param	func	Función
 * @return	gpio_no_error si los parámetros de entrada son corectos o
 *			gpio_invalid_parameter en otro caso
 */
inline gpio_err_t gpio_set_pin_func (gpio_pin_t pin, gpio_func_t func)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 9 */
	
	if (pin < 16)
		gpio_regs->GPIO_FUNC_SEL0 = (gpio_regs->GPIO_FUNC_SEL0 & ~(3 << (pin * 2))) | (func << (pin * 2));
	else if (pin < 32)
		gpio_regs->GPIO_FUNC_SEL0 = (gpio_regs->GPIO_FUNC_SEL0 & ~(3 << ((pin - 16) * 2))) | (func << ((pin - 16) * 2));
	else if (pin < 48)
		gpio_regs->GPIO_FUNC_SEL0 = (gpio_regs->GPIO_FUNC_SEL0 & ~(3 << ((pin - 32) * 2))) | (func << ((pin - 32) * 2));
	else
		gpio_regs->GPIO_FUNC_SEL0 = (gpio_regs->GPIO_FUNC_SEL0 & ~(3 << ((pin - 48) * 2))) | (func << ((pin - 48) * 2));

	return gpio_no_error;
}

/*****************************************************************************/
