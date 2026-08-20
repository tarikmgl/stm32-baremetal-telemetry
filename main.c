#include <stdint.h>
#include "uart.h"
#include "telemetry.h"
#include "can.h"

#define RCC_BASE      0x40021000UL
#define GPIOC_BASE    0x40011000UL
#define FLASH_BASE    0x40022000UL
#define TIM2_BASE     0x40000000UL
#define NVIC_ISER0    (*(volatile uint32_t*)(0xE000E100))

#define RCC_CR        (*(volatile uint32_t*)(RCC_BASE + 0x00))
#define RCC_CFGR      (*(volatile uint32_t*)(RCC_BASE + 0x04))
#define RCC_APB1ENR   (*(volatile uint32_t*)(RCC_BASE + 0x1C))
#define RCC_APB2ENR   (*(volatile uint32_t*)(RCC_BASE + 0x18))
#define FLASH_ACR     (*(volatile uint32_t*)(FLASH_BASE + 0x00))

#define GPIOC_CRH     (*(volatile uint32_t*)(GPIOC_BASE + 0x04))
#define GPIOC_BSRR    (*(volatile uint32_t*)(GPIOC_BASE + 0x10))

#define TIM2_CR1      (*(volatile uint32_t*)(TIM2_BASE + 0x00))
#define TIM2_DIER     (*(volatile uint32_t*)(TIM2_BASE + 0x0C))
#define TIM2_SR       (*(volatile uint32_t*)(TIM2_BASE + 0x10))
#define TIM2_PSC      (*(volatile uint32_t*)(TIM2_BASE + 0x28))
#define TIM2_ARR      (*(volatile uint32_t*)(TIM2_BASE + 0x2C))

static volatile uint16_t g_seq_num = 0;

void delay(volatile uint32_t count) {
    while (count--) { __asm__("nop"); }
}

void SystemClock_Config(void) {
    RCC_CR |= (1 << 16);
    while (!(RCC_CR & (1 << 17)));

    FLASH_ACR |= (1 << 4) | (0x2 << 0);

    RCC_CFGR |= (0x4 << 8);

    RCC_CFGR |= (1 << 16);
    RCC_CFGR |= (0x7 << 18);

    RCC_CR |= (1 << 24);
    while (!(RCC_CR & (1 << 25)));

    RCC_CFGR |= (0x2 << 0);
    while ((RCC_CFGR & (0x3 << 2)) != (0x2 << 2));
}

void TIM2_Init(void) {
    RCC_APB1ENR |= (1 << 0);   // TIM2 clock ac

    TIM2_PSC = 7199;           // 72MHz / 7200 = 10kHz
    TIM2_ARR = 9999;           // 10kHz / 10000 = 1Hz

    TIM2_DIER |= (1 << 0);     // Update interrupt enable
    NVIC_ISER0 |= (1 << 28);   // NVIC: IRQ28 (TIM2) enable

    TIM2_CR1 |= (1 << 0);      // Sayaci baslat (CEN)
}

void TIM2_IRQHandler(void) {
    TIM2_SR &= ~(1 << 0);      // UIF flag'ini temizle

    // LED toggle (gorsel gostergemiz)
    static uint8_t led_state = 0;
    if (led_state) {
        GPIOC_BSRR = (1 << 13);
    } else {
        GPIOC_BSRR = (1 << (13 + 16));
    }
    led_state = !led_state;

    // Telemetri frame
    TelemetryFrame_t frame;
    telemetry_build_frame(&frame, g_seq_num++);

    uint8_t *raw = (uint8_t*)&frame;
    for (uint16_t i = 0; i < TELEMETRY_FRAME_SIZE; i++) {
        uart_send_char(raw[i]);
    }
}

int main(void) {
    SystemClock_Config();

    RCC_APB2ENR |= (1 << 4); // GPIOC clock enable
    GPIOC_CRH &= ~(0xF << 20); // PC13 = push-pull output
    GPIOC_CRH |=  (0x2 << 20); // PC13 = push-pull output

    uart_init();
    TIM2_Init();    
    can_init();
    can_normal_mode();

    uint8_t tx_data[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t rx_data[8] = {0};
    uint32_t rx_id = 0;
    uint32_t tx_id = 0x123;

    // CAN mesaj gonder
    can_send(tx_id, tx_data, sizeof(tx_data));
    
    delay(1000);

    uint8_t len = can_receive(&rx_id, rx_data);
    if (len > 0) {
        // Gelen mesaj var, UART'tan gonder
        uart_send_char('R');
        uart_send_char('X');
        uart_send_char(':');
        uart_send_char((rx_id >> 8) & 0xFF);
        uart_send_char(rx_id & 0xFF);
        uart_send_char(' ');
        for (uint8_t i = 0; i < len; i++) {
            uart_send_char(rx_data[i]);
        }
        uart_send_char('\n');
    }

    while (1) {

    }
}
