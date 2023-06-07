#ifndef INCLUDE__RISCV_GPIO_H_
#define INCLUDE__RISCV_GPIO_H_

#include "basic_types.h"

/**
 * \brief GPIO initialization.
 */
void init_gpio();

/**
 * \brief Read GPIO switch status.
 *
 * \param sw the number of the switch to read [0..2].
 *
 * \return 1 if the switch is ON and 0 otherwise.
 */
uint8_t read_switch(uint8_t sw);

/**
 * \brief Sets the status of a LED
 *
 * @param led the number of the led [0..3]
 * @param value 0 if the LED must be switched off or any other value if the
 *                LED must be switched on.
 */
void write_led(uint8_t led, uint8_t value);

#endif /* INCLUDE__RISCV_GPIO_H_ */
