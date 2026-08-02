# STM32F103 Bare-Metal Telemetry Firmware

STM32F103C8T6 (Blue Pill) üzerinde, HAL kütüphanesi kullanmadan, doğrudan register seviyesinde yazılmış bare-metal firmware projesi.

## Özellikler
- 72MHz PLL clock konfigürasyonu (8MHz HSE kristalden)
- Register-level GPIO kontrolü
- Register-level UART (USART1) haberleşmesi
- TIM2 timer interrupt (NVIC ile)
- CRC-16/CCITT-FALSE ile telemetri paket üretimi

## Derleme
```bash
make
```

## Flashleme (ST-Link gerekli)
```bash
make flash
```

## Araçlar
- gcc-arm-none-eabi
- OpenOCD
- ST-Link V2
