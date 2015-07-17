@
@ Sistemas Empotrados
@ El "hola mundo" en la Redwire EconoTAG
@

@
@ Datos globales
@
		.data

		@ Registro de control de dirección del GPIO32-GPIO63
        @GPIO_PAD_DIR1:    .word 0x80000004

        @ Registro de activación de bits del GPIO32-GPIO63
        @GPIO_DATA_SET1:   .word 0x8000004c

        @ Registro de limpieza de bits del GPIO32-GPIO63
        @GPIO_DATA_RESET1: .word 0x80000054

        @ El led rojo está en el GPIO 44 (el bit 12 de los registros GPIO_X_1)
        LED_RED_MASK:     .word (1 << (44-32))

        @ Retardo para el parpadeo
        DELAY:            .word 0x00080000

        @ Registro de control de dirección del GPIO00-GPIO31
        @GPIO_PAD_DIR0:    .word 0x80000000

        @ Registro de activación de bits del GPIO00-GPIO31
        @GPIO_DATA_SET0:   .word 0x80000048

        @ Registro de consulta de bits del GPIO00-GPIO31
        @GPIO_DATA0:       .word 0x80000008

        @ Led verde (GPIO 45)
        LED_GREEN_MASK: .word (1 << (45-32))

        @ Boton S2 (GPIO 23 y 27)
        BUTTON_S2_MASK0: .word 1 << 23
        BUTTON_S2_MASK1: .word 1 << 27

        @ Boton S3 (GPIO 22 y 26)
        BUTTON_S3_MASK0: .word 1 << 22
        BUTTON_S3_MASK1: .word 1 << 26

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
        ldr		r5, [r5]

        @ Direcciones de los registros GPIO_DATA_SET1 y GPIO_DATA_RESET1
        ldr     r6, =gpio_data_set1
        @ldr		r6, [r6]
        ldr     r7, =gpio_data_reset1
        @ldr		r7, [r7]

loop:

		bl		test_buttons

        @ Encendemos el led
        str     r5, [r6]

        @ Pausa corta
        ldr     r0, =DELAY
        ldr		r0, [r0]
        bl      pause

        @ Apagamos el led
        str     r5, [r7]

        @ Pausa corta
        ldr     r0, =DELAY
        ldr		r0, [r0]
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

		.type   gpio_init, %function
gpio_init:
		@ Configuramos el GPIO43 y GPIO44 para que sea de salida
        ldr     r4, =gpio_pad_dir1
        @ldr	r4, [r4]
        ldr     r5, =LED_RED_MASK
        ldr		r5, [r5]
        ldr		r6, =LED_GREEN_MASK
        ldr		r6, [r6]
        orr		r5, r5, r6
        str     r5, [r4]

        @ Configuramos el GPIO22 y GPIO23 para que sea de salida
        ldr     r4, =gpio_pad_dir0
        @ldr	r4, [r4]
        ldr     r5, =BUTTON_S2_MASK0
        ldr		r5, [r5]
        ldr		r6, =BUTTON_S3_MASK0
        ldr		r6, [r6]
        orr		r5, r5, r6
        str     r5, [r4]

        @ Activamos botones S2 y S3
		ldr     r4, =gpio_data_set0
		@ldr	r4, [r4]
		str     r5, [r4]

		mov		pc, lr

@ Consultar botones
@ r5 (out): Mascara del led que hay que hacer parpadear

		.type   test_buttons, %function
test_buttons:

		ldr		r4, =gpio_data0
		@ldr		r4, [r4]
		ldr		r1, [r4]

		@ Consultar boton S2 y activar (en su caso) el led rojo

		ldr		r3, =BUTTON_S2_MASK1
		ldr		r3, [r3]
		ands	r2, r1, r3
		ldrne	r5, =LED_RED_MASK
		ldrne	r5, [r5]
		movne	pc, lr

		@ Consultar boton S3 y activar (en su caso) el led verde

		ldr		r3, =BUTTON_S3_MASK1
		ldr		r3, [r3]
		ands	r2, r1, r3
		ldrne	r5, =LED_GREEN_MASK
		ldrne	r5, [r5]
		mov		pc, lr
