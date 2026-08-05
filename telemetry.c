#include "telemetry.h"

uint16_t crc16_ccitt(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

void telemetry_build_frame(TelemetryFrame_t *frame, uint16_t seq_num)
{
    frame->sync = TELEMETRY_SYNC_BYTE;
    frame->seq_num = seq_num;
    
    frame->fake_temp = 2500 + (seq_num % 50);       
    frame->fake_pressure = 10132 + (seq_num % 20);  

    frame->crc16 = crc16_ccitt((uint8_t*)frame, TELEMETRY_CRC_DATA_LEN);
}
