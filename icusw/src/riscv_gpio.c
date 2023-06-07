#include "basic_types.h"
#include "riscv_gpio.h"


struct GPIO_regs
{
    /** \brief GPIO Data Register */
    volatile uint32_t Data;     /* 0xFC001000 */
    /** \brief GPIO Output Register */
    volatile uint32_t Output;   /* 0xFC001004 */
    /** \brief GPIO  Direction Register */
    volatile uint32_t Direction;     /* 0xFC001008 */
};

/*
  - Switches:
  SW[0..2] DIP switch: GPIO inputs [0..2]

  - LEDs:
  LED[0..3]: Connected to GPIO0 outputs [16..19]

*/

/** Mapping of the GPIO's registers */
volatile struct GPIO_regs * const pGPIO_REGS =
        (struct GPIO_regs *)0xFC083000;

#define GPIO_LED0_MASK (1 << 16)
#define GPIO_LED1_MASK (1 << 17)
#define GPIO_LED2_MASK (1 << 18)
#define GPIO_LED3_MASK (1 << 19)

#define GPIO_SW0_MASK  (1 << 0)
#define GPIO_SW1_MASK  (1 << 1)
#define GPIO_SW2_MASK  (1 << 2)

void init_gpio() {

    // We have to setup the LEDs as outputs and the switches as inputs by
    // modifying the Direction register

    // Set the bits corresponding to the LEDs
    pGPIO_REGS->Direction = pGPIO_REGS->Direction |
            (GPIO_LED0_MASK | GPIO_LED1_MASK |
             GPIO_LED2_MASK | GPIO_LED3_MASK);

    // Clear the bits corresponding to the Switches
    pGPIO_REGS->Direction = pGPIO_REGS->Direction &
            ~(GPIO_SW0_MASK | GPIO_SW1_MASK | GPIO_SW2_MASK);

}

uint8_t read_switch(uint8_t sw) {

    uint8_t ret = 0;

    // Checking input parameter
    if (sw < 3) {

        // If the input parameter is valid

        uint32_t data = pGPIO_REGS->Data;

        // Check if the switch is ON or OFF
        ret = (data & (1 << sw)) != 0;

    }

    return ret;

}

void write_led(uint8_t led, uint8_t value) {

    // Checking input parameter
    if (led < 4) {

        uint32_t bit = led + 16;

        if (value == 0) {
            // We need to turn it off
            uint32_t output = pGPIO_REGS->Output;
            output = output & ~(1 << bit);
            pGPIO_REGS->Output = output;
        } else {
            // We need to turn it on
            uint32_t output = pGPIO_REGS->Output;
            output = output | (1 << bit);
            pGPIO_REGS->Output = output;
        }

    }

}
