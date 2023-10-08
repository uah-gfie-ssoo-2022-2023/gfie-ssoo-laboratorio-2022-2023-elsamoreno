/*
 * serialize.h
 *
 *  Created on: Mar 31, 2022
 *      Author: atcsol
 */

#ifndef INCLUDE__SERIALIZE_H_
#define INCLUDE__SERIALIZE_H_

#include "basic_types.h"

/**
 * \brief Serializes a 16-bit word
 *
 * \param data_bytes vector containing the MSB and LSB bytes of the word
 *
 * \return the serialized 16-bit word
 */
uint16_t deserialize_uint16(uint8_t data_bytes[2]);

/**
 * \brief Serializes a 32-bit word
 *
 * \param data_bytes vector containing the 4 bytes of the word
 *
 * \return the serialized 32-bit word
 */
uint32_t deserialize_uint32(uint8_t data_bytes[4]);

/**
 * \brief Serializes a 16-bit word
 *
 * \param var the 16-bit word to serialize
 * \param data_bytes vector that will store the serialized 16-bit word
 *
 */
void serialize_uint16(uint16_t aux, uint8_t data_bytes[2]);

/**
 * \brief Serializes a 32-bit word
 *
 * \param var the 32-bit word to serialize
 * \param data_bytes vector that will store the serialized 32-bit word
 *
 */
void serialize_uint32(uint32_t aux, uint8_t data_bytes[4]);

#endif
