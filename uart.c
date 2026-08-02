#include "uart.h"

#define RCC_BASE    0x40021000UL
#define GPIOA_BASE  0x40010800UL
#define USART1_BASE 0x40013800UL

#define RCC_APB2ENR (*(volatile uint32_t*)(RCC_BASE + 0x18))
#define GPIOA_CRH   (*(volatile uint32_t*)(GPIOA_BASE + 0x04))

#define USART1_SR   (*(volatile uint32_t*)(USART1_BASE + 0x00))
#define USART1_DR   (*(volatile uint32_t*)(USART1_BASE + 0x04))
#define USART1_BRR  (*(volatile uint32_t*)(USART1_BASE + 0x08))
#define USART1_CR1  (*(volatile uint32_t*)(USART1_BASE + 0x0C))

void uart_init(void) {
    RCC_APB2ENR |= (1 << 0) | (1 << 2) | (1 << 14);

    GPIOA_CRH &= ~(0xF << 4);
    GPIOA_CRH |=  (0xB << 4);

    GPIOA_CRH &= ~(0xF << 8);
    GPIOA_CRH |=  (0x4 << 8);

    USART1_BRR = (468 << 4) | 12;

    USART1_CR1 |= (1 << 13) | (1 << 3) | (1 << 2);
}

void uart_send_char(char c) {
    while (!(USART1_SR & (1 << 7)));
    USART1_DR = c;
}

char uart_read_char(void) {
    while (!(USART1_SR & (1 << 5)));
    return USART1_DR;
}