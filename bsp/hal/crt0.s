@
@ Sistemas Empotrados
@ CRT0 para el Econotag. El boot loader de la ROM limpia la RAM y
@ carga la imagen desde la Flash
@

	.set _IRQ_DISABLE, 0x80 @ cuando el bit I está activo, IRQ está deshabilitado
	.set _FIQ_DISABLE, 0x40 @ cuando el bit F está activo, FIQ está deshabilitado

	.set _USR_MODE, 0x10
	.set _FIQ_MODE, 0x11
	.set _IRQ_MODE, 0x12
	.set _SVC_MODE, 0x13
	.set _ABT_MODE, 0x17
	.set _UND_MODE, 0x1B
	.set _SYS_MODE, 0x1F

@
@ Sección de código de arranque
@
	.code 32
	.section .startup, "xa"


@
@ Vectores de excepción en la RAM
@
	.globl _vector_table
_vector_table:
	ldr	pc, [pc, #24]	@ Soft reset
	ldr	pc, [pc, #24]	@ Undefined
	ldr	pc, [pc, #24]	@ SWI
	ldr	pc, [pc, #24]	@ Prefetch abort
	ldr	pc, [pc, #24]	@ Data abort
	nop					@ Reserved
	ldr	pc, [pc, #24]	@ IRQ
	ldr	pc, [pc, #24]	@ FIQ


@
@ Tabla de direcciones absolutas de los manejadores
@
	.globl	_excep_handlers
_excep_handlers:
	.word	_soft_reset_handler
	.word	_undef_handler
	.word	_swi_handler
	.word	_pabt_handler
	.word	_dabt_handler
	nop
	.word	_irq_handler
	.word	_fiq_handler

@
@ Incluimos la nota del copyright al principio de la ROM
@
	.string "Copyright (C) Universidad de Granada. All Rights Reserved."

	@ Las instrucciones deben estar alineadas a una frontera de 32 bits
	.align	4

@
@ Manejadores por defecto
@
_soft_reset_handler:
	b	_start
_undef_handler:
	b	.
_swi_handler:
	b	.
_pabt_handler:
	b	.
_dabt_handler:
	b	.
_irq_handler:
	b	.
_fiq_handler:
	b	.

@
@ Comienza el CRT
@
	.align	4
	.global	_start
	.type	_start, %function
_start:

@
@ Inicializamos las pilas para cada modo
@

@ ESTA PARTE SE COMPLETARÁ EN LA PRÁCTICA 6

	msr cpsr_c, #(_UND_MODE | _IRQ_DISABLE | _FIQ_DISABLE)
	ldr sp, =_und_stack_top
	msr cpsr_c, #(_ABT_MODE | _IRQ_DISABLE | _FIQ_DISABLE)
	ldr sp, =_abt_stack_top
	msr cpsr_c, #(_SYS_MODE | _IRQ_DISABLE | _FIQ_DISABLE)
	ldr sp, =_sys_stack_top
	msr cpsr_c, #(_FIQ_MODE | _IRQ_DISABLE | _FIQ_DISABLE)
	ldr sp, =_fiq_stack_top
	msr cpsr_c, #(_IRQ_MODE | _IRQ_DISABLE | _FIQ_DISABLE)
	ldr sp, =_irq_stack_top
	msr cpsr_c, #(_SVC_MODE | _IRQ_DISABLE | _FIQ_DISABLE)
	ldr sp, =_svc_stack_top

@
@ Inicialización de la plataforma (llamada a bsp_init)
@

@ ESTA PARTE SE COMPLETARÁ EN LA PRÁCTICA 6

	bl bsp_init

@
@ Cambiamos a modo User y habilitamos las interrupciones
@

@ ESTA PARTE SE COMPLETARÁ EN LA PRÁCTICA 6

	msr cpsr_c, #_USR_MODE

@
@ Salto a main
@

@ ESTA PARTE SE COMPLETARÁ EN LA PRÁCTICA 6

	bl main

@
@ Colgamos el sistema si main retorna
@
	b	.			@ Colgamos el sistema si main retorna

	.size   _start, .-_start

