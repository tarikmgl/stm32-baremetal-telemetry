#include "can.h"

#define RCC_BASE     0x40021000UL
#define GPIOA_BASE   0x40010800UL
#define CAN1_BASE    0x40006400UL

#define RCC_APB1ENR   (*(volatile uint32_t*)(RCC_BASE + 0x1C))
#define RCC_APB2ENR   (*(volatile uint32_t*)(RCC_BASE + 0x18))
#define GPIOA_CRH     (*(volatile uint32_t*)(GPIOA_BASE + 0x04))

#define CAN1_MCR  (*(volatile uint32_t*)(CAN1_BASE + 0x00))
#define CAN1_MSR  (*(volatile uint32_t*)(CAN1_BASE + 0x04))
#define CAN1_BTR  (*(volatile uint32_t*)(CAN1_BASE + 0x1C))
#define CAN1_FMR   (*(volatile uint32_t*)(CAN1_BASE + 0x200))
#define CAN1_FA1R  (*(volatile uint32_t*)(CAN1_BASE + 0x21C))
#define CAN1_F0R1  (*(volatile uint32_t*)(CAN1_BASE + 0x240)) // Filter bank 0 register 1
#define CAN1_F0R2  (*(volatile uint32_t*)(CAN1_BASE + 0x244)) // Filter bank 0 register 2
#define CAN1_TI0R  (*(volatile uint32_t*)(CAN1_BASE + 0x180)) // Transmit identifier register
#define CAN1_TDT0R (*(volatile uint32_t*)(CAN1_BASE + 0x184)) // Transmit data length and time stamp register
#define CAN1_TDL0R (*(volatile uint32_t*)(CAN1_BASE + 0x188)) // Transmit data low register
#define CAN1_TDH0R (*(volatile uint32_t*)(CAN1_BASE + 0x18C)) // Transmit data high register
#define CAN1_TSR   (*(volatile uint32_t*)(CAN1_BASE + 0x08)) // Transmit status register
#define CAN1_RF0R  (*(volatile uint32_t*)(CAN1_BASE + 0x0C)) // Receive FIFO 0 register
#define CAN1_RI0R  (*(volatile uint32_t*)(CAN1_BASE + 0x1B0)) // Receive identifier register
#define CAN1_RDT0R (*(volatile uint32_t*)(CAN1_BASE + 0x1B4)) // Receive data length and time stamp register
#define CAN1_RDL0R (*(volatile uint32_t*)(CAN1_BASE + 0x1B8)) // Receive data low register
#define CAN1_RDH0R (*(volatile uint32_t*)(CAN1_BASE + 0x1BC)) // Receive data high register

void can_init(void) {
    RCC_APB1ENR |= (1 << 25);
    RCC_APB2ENR |= (1 << 2);

    GPIOA_CRH &= ~(0xF << 12);
    GPIOA_CRH |=  (0x4 << 12);

    GPIOA_CRH &= ~(0xF << 16);
    GPIOA_CRH |=  (0xB << 16);

    CAN1_MCR &= ~(1 << 1);              // SLEEP = 0
    while (CAN1_MSR & (1 << 1));        // SLAK sifirlanana kadar bekle

    CAN1_MCR |= (1 << 0);               // INRQ = 1
    while (!(CAN1_MSR & (1 << 0)));     // INAK set olana kadar bekle

    CAN1_BTR = (1 << 30)
             | (0x0 << 24)
             | (0x1 << 20)
             | (0xC << 16)
             | (17  << 0);
}

void can_normal_mode(void){
    CAN1_MCR &= ~(1 << 0);              // INRQ = 0, normal mode'a don
    while (CAN1_MSR & (1 << 0));        // INAK sifirlanana kadar bekle

    CAN1_FMR  |= (1 << 0);
    CAN1_F0R1  = 0x00000000;
    CAN1_F0R2  = 0x00000000;
    CAN1_FA1R |= (1 << 0);
    CAN1_FMR  &= ~(1 << 0);
}

uint8_t can_send(uint32_t id, uint8_t *data, uint8_t len) {
    CAN1_TI0R = (id << 21);              // Standard ID (11-bit), bit 21'den itibaren

    CAN1_TDT0R = len & 0xF;              // DLC = veri uzunlugu

    CAN1_TDL0R = (data[3]<<24)|(data[2]<<16)|(data[1]<<8)|data[0];
    CAN1_TDH0R = (data[7]<<24)|(data[6]<<16)|(data[5]<<8)|data[4];

    CAN1_TI0R |= (1 << 0);               // TXRQ: gonderme talebi

    while (!(CAN1_TSR & (1 << 0)));      // RQCP0: istek tamamlandi mi bekle
    return 1;
}

uint8_t can_receive(uint32_t *id, uint8_t *data) {
    if (!(CAN1_RF0R & 0x3)) return 0;    // FIFO'da mesaj yok

    CAN1_RI0R &= ~(1 << 2);                // IDE = 0, standard ID
    *id = (CAN1_RI0R >> 21);
    uint8_t len = CAN1_RDT0R & 0xF;

    uint32_t low = CAN1_RDL0R, high = CAN1_RDH0R;
    data[0]=low; data[1]=low>>8; data[2]=low>>16; data[3]=low>>24;
    data[4]=high; data[5]=high>>8; data[6]=high>>16; data[7]=high>>24;

    CAN1_RF0R |= (1 << 5);               // RFOM0: FIFO'yu serbest birak

    return len;
}