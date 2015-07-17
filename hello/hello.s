@
@ Sistemas Empotrados
@ El "hola mundo" en la Redwire EconoTAG
@

@
@ Constantes
@

        @ Registro de control de dirección del GPIO32-GPIO63
        .set GPIO_PAD_DIR1,    0x80000004

        @ Registro de activación de bits del GPIO32-GPIO63
        .set GPIO_DATA_SET1,   0x8000004c

        @ Registro de limpieza de bits del GPIO32-GPIO63
        .set GPIO_DATA_RESET1, 0x80000054

        @ El led rojo está en el GPIO 44 (el bit 12 de los registros GPIO_X_1)
        .set LED_RED_MASK,     (1 << (44-32))

        @ Retardo para el parpadeo
        .set DELAY,            0x00080000

        @ Registro de control de dirección del GPIO00-GPIO31
        .set GPIO_PAD_DIR0,    0x80000000

        @ Registro de activación de bits del GPIO00-GPIO31
        .set GPIO_DATA_SET0,   0x80000048

        @ Registro de consulta de bits del GPIO00-GPIO31
        .set GPIO_DATA0,       0x80000008

        @ Led verde (GPIO 45)
        .set LED_GREEN_MASK, (1 << (45-32))

        @ Boton S2 (GPIO 23 y 27)
        .set BUTTON_S2_MASK0, 1 << 23
        .set BUTTON_S2_MASK1, 1 << 27

        @ Boton S3 (GPIO 22 y 26)
        .set BUTTON_S3_MASK0, 1 << 22
        .set BUTTON_S3_MASK1, 1 << 26

@
@ Punto de entrada
@

        .code 32
        .text
        .global _start
        .type   _start, %function

_start:
        bl 		gpio_init

        @ Led por defecto (rojo)
        ldr     r5, =LED_RED_MASK

        @ Direcciones de los registros GPIO_DATA_SET1 y GPIO_DATA_RESET1
        ldr     r6, =GPIO_DATA_SET1
        ldr     r7, =GPIO_DATA_RESET1

loop:

		bl		test_buttons

        @ Encendemos el led
        str     r5, [r6]

        @ Pausa corta
        ldr     r0, =DELAY
        bl      pause

        @ Apagamos el led
        str     r5, [r7]

        @ Pausa corta
        ldr     r0, =DELAY
        bl      pause

        @ Bucle infinito
        b       loop
        
@
@ Función que produce un retardo
@ r0: iteraciones del retardo
@
        .type   pause, %function
pause:
        subs    r0, r0, #1
        bne     pause
        mov     pc, lr

@ Inicializacion del GPIO

gpio_init:
		@ Configuramos el GPIO43 y GPIO44 para que sea de salida
        ldr     r4, =GPIO_PAD_DIR1
        ldr     r5, =LED_RED_MASK
        ldr		r6, =LED_GREEN_MASK
        orr		r5, r5, r6
        str     r5, [r4]

        @ Configuramos el GPIO22 y GPIO23 para que sea de salida
        ldr     r4, =GPIO_PAD_DIR0
        ldr     r5, =BUTTON_S2_MASK0
        ldr		r6, =BUTTON_S3_MASK0
        orr		r5, r5, r6
        str     r5, [r4]

        @ Activamos botones S2 y S3
		ldr     r4, =GPIO_DATA_SET0
		str     r5, [r4]

		mov		pc, lr

@ Consultar botones
@ r5 (out): Mascara del led que hay que hacer parpadear

test_buttons:

		ldr		r4, =GPIO_DATA0
		ldr		r1, [r4]

		@ Consultar boton S2 y activar (en su caso) el led rojo

		ldr		r3, =BUTTON_S2_MASK1
		ands	r2, r1, r3
		ldrne	r5, =LED_RED_MASK
		movne	pc, lr

		@ Consultar boton S3 y activar (en su caso) el led verde

		ldr		r3, =BUTTON_S3_MASK1
		ands	r2, r1, r3
		ldrne	r5, =LED_GREEN_MASK
		mov		pc, lr
