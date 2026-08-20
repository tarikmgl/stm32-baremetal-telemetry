#ifndef CAN_H
#define CAN_H

#include <stdint.h>

void can_init(void);
void can_normal_mode(void);
uint8_t can_send(uint32_t id, uint8_t *data, uint8_t len);
uint8_t can_receive(uint32_t *id, uint8_t *data);

#endif