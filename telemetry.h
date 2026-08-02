#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdint.h>

// ---- L2 Frame Formatı ----
// [SYNC][SEQ_NUM][FAKE_TEMP][FAKE_PRESSURE][CRC16]
#pragma pack(push, 1)
typedef struct {
    uint8_t  sync;          // 0xAA
    uint16_t seq_num;
    uint16_t fake_temp;
    uint16_t fake_pressure;
    uint16_t crc16;
} TelemetryFrame_t;
#pragma pack(pop)

#define TELEMETRY_SYNC_BYTE   0xAA
#define TELEMETRY_FRAME_SIZE  sizeof(TelemetryFrame_t)
#define TELEMETRY_CRC_DATA_LEN (TELEMETRY_FRAME_SIZE - sizeof(uint16_t))

uint16_t crc16_ccitt(const uint8_t *data, uint16_t length);
void telemetry_build_frame(TelemetryFrame_t *frame, uint16_t seq_num);

#endif