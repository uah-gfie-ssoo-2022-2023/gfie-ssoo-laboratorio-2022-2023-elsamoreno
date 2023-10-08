#ifndef __RTEMS_CONFIG_H__
#define __RTEMS_CONFIG_H__

#include <rtems.h>
#include "driver.h"
#include "tc_descriptor.h"


rtems_task Init(rtems_task_argument arg);

// We require the Console Driver
//#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

// For the time being, we will require the clock driver
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

// Default value of microseconds per tick
#define CONFIGURE_MICROSECONDS_PER_TICK (10000)

#define CONFIGURE_APPLICATION_EXTRA_DRIVERS { \
            .initialization_entry = riscv_uart_driver_initialize, \
            .open_entry = riscv_uart_driver_open, \
            .close_entry = riscv_uart_driver_close, \
            .read_entry = riscv_uart_driver_read, \
            .write_entry = riscv_uart_driver_write, \
            .control_entry = NULL \
        }

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS (4)

// Maximum number of tasks
#define CONFIGURE_MAXIMUM_TASKS (4)

// Ensure that the default initialization table is defined
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

//Static memory reservation for semaphores handling
#define CONFIGURE_MAXIMUM_SEMAPHORES (2)

//Static memory reservation for message queues handling
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES (2)
#define CONFIGURE_MESSAGE_BUFFER_MEMORY ( \
 CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( \
 15, \
 sizeof(tc_descriptor_t) \
 )\
 )


#define CONFIGURE_INIT


#include <rtems/confdefs.h>

#endif /* __RTEMS_CONFIG_H__ */

