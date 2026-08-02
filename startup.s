.syntax unified
.cpu cortex-m3
.thumb

.global Reset_Handler
.global _estack

.section .isr_vector, "a"
.word _estack
.word Reset_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word 0,0,0,0
.word Default_Handler
.word Default_Handler
.word 0
.word Default_Handler
.word Default_Handler
/* --- Peripheral Interrupts (IRQ0'dan itibaren) --- */
.word Default_Handler    /* IRQ0: WWDG */
.word Default_Handler    /* IRQ1: PVD */
.word Default_Handler    /* IRQ2: TAMPER */
.word Default_Handler    /* IRQ3: RTC */
.word Default_Handler    /* IRQ4: FLASH */
.word Default_Handler    /* IRQ5: RCC */
.word Default_Handler    /* IRQ6: EXTI0 */
.word Default_Handler    /* IRQ7: EXTI1 */
.word Default_Handler    /* IRQ8: EXTI2 */
.word Default_Handler    /* IRQ9: EXTI3 */
.word Default_Handler    /* IRQ10: EXTI4 */
.word Default_Handler    /* IRQ11: DMA1_Channel1 */
.word Default_Handler    /* IRQ12: DMA1_Channel2 */
.word Default_Handler    /* IRQ13: DMA1_Channel3 */
.word Default_Handler    /* IRQ14: DMA1_Channel4 */
.word Default_Handler    /* IRQ15: DMA1_Channel5 */
.word Default_Handler    /* IRQ16: DMA1_Channel6 */
.word Default_Handler    /* IRQ17: DMA1_Channel7 */
.word Default_Handler    /* IRQ18: ADC1_2 */
.word Default_Handler    /* IRQ19: USB_HP_CAN1_TX */
.word Default_Handler    /* IRQ20: USB_LP_CAN1_RX0 */
.word Default_Handler    /* IRQ21: CAN1_RX1 */
.word Default_Handler    /* IRQ22: CAN1_SCE */
.word Default_Handler    /* IRQ23: EXTI9_5 */
.word Default_Handler    /* IRQ24: TIM1_BRK */
.word Default_Handler    /* IRQ25: TIM1_UP */
.word Default_Handler    /* IRQ26: TIM1_TRG_COM */
.word Default_Handler    /* IRQ27: TIM1_CC */
.word TIM2_IRQHandler    /* IRQ28: TIM2 */

.section .text
.thumb_func
Reset_Handler:
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata
copy_loop:
    cmp r1, r2
    ittt lt
    ldrlt r3, [r0], #4
    strlt r3, [r1], #4
    blt copy_loop

    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0
zero_loop:
    cmp r0, r1
    it lt
    strlt r2, [r0], #4
    it lt
    addlt r0, r0, #4
    blt zero_loop

    bl main
hang: b hang

.thumb_func
Default_Handler:
    b Default_Handler

.section .stack
.align 3
.equ Stack_Size, 0x400
.space Stack_Size
_estack:
