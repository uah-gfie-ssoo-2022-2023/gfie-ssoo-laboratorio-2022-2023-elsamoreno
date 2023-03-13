#include <rtems.h>

// Include the configuration header file
#include "rtems_config.h"

// Luckily, we have libc! :)
#include <stdio.h>

rtems_task Init(rtems_task_argument ignored)
{
    printf("Hello World\n");

    // End the system execution
    rtems_shutdown_executive(0);
}
