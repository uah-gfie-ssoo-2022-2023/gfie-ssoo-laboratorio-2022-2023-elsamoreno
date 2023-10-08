#include "basic_types.h"
#include "serialize.h"

uint16_t deserialize_uint16(uint8_t data_bytes[2]) {

    uint16_t aux = 0;

    aux = (uint16_t)data_bytes[0];

    aux = aux << 8;

    aux = aux | (uint16_t)data_bytes[1];

    return aux;

}

uint32_t deserialize_uint32(uint8_t data_bytes[4]) {

    uint32_t aux = 0;

    aux = (uint32_t)data_bytes[0];

    aux = aux << 8;

    aux = aux | (uint32_t)data_bytes[1];

    aux = aux << 8;

    aux = aux | (uint32_t)data_bytes[2];

    aux = aux << 8;

    aux = aux | (uint32_t)data_bytes[3];

    return aux;

}

void serialize_uint16(uint16_t aux, uint8_t data_bytes[2]) {

    data_bytes[0] = (uint8_t)((uint16_t)(aux & 65280) >> 8);

    data_bytes[1] = (uint8_t)(aux & 255);

    return;

}

void serialize_uint32(uint32_t aux, uint8_t data_bytes[4]) {

    data_bytes[0] = (uint8_t)((uint32_t)(aux & 4278190080) >> 24);

    data_bytes[1] = (uint8_t)((uint32_t)(aux & 16711680) >> 16);

    data_bytes[2] = (uint8_t)((uint32_t)(aux & 65280) >> 8);

    data_bytes[3] = (uint8_t)(aux & 255);

    return;

}
