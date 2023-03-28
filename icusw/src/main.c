#include <rtems.h>

// Include the configuration header file
#include "rtems_config.h"

// Luckily, we have libc! :)
#include <stdio.h>

#include "housekeeping.h"
#include "delay.h"


rtems_id housekeeping_task_id;

rtems_task housekeeping_task (rtems_task_argument ignored) {

	rtems_interval current_time = 0;
	rtems_interval ticks = rtems_clock_get_ticks_per_second();
	rtems_interval next_activation = rtems_clock_get_ticks_since_boot() + ticks;

	init_housekeeping();


	    while (1) {

	        current_time = rtems_clock_get_ticks_since_boot();

	        printf("Current time: %d\n", current_time);

	        //rtems_task_wake_after(ticks);
	        task_delay_until(next_activation);

            do_housekeeping();

            next_activation = next_activation + ticks;
	    }

	}

rtems_task Init(rtems_task_argument ignored)
{
	if (rtems_task_create(rtems_build_name('H','k','T','k'), 10,
	 RTEMS_MINIMUM_STACK_SIZE,
	 RTEMS_DEFAULT_MODES,
	 RTEMS_DEFAULT_ATTRIBUTES,
	 &housekeeping_task_id) != RTEMS_SUCCESSFUL) {
	 rtems_shutdown_executive(0);
	 }

	if (rtems_task_start(housekeeping_task_id, housekeeping_task, 0) != RTEMS_SUCCESSFUL) {
	 rtems_shutdown_executive(0);
	}

	if (rtems_task_delete(RTEMS_SELF) != RTEMS_SUCCESSFUL) {
	 rtems_shutdown_executive(0);
	}




}




