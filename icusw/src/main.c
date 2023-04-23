#include <rtems.h>

// Include the configuration header file
#include "rtems_config.h"

// Luckily, we have libc! :)
#include <stdio.h>

#include "housekeeping.h"
#include "delay.h"
#include "tmtc_pool.h"
#include "tmtc_channel.h"
#include "tc_descriptor.h"
#include "emu_tc_rx.h"
#include "manager.h"


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
	/*if (rtems_task_create(rtems_build_name('H','k','T','k'), 10,
			RTEMS_MINIMUM_STACK_SIZE,
			RTEMS_DEFAULT_MODES,
			RTEMS_DEFAULT_ATTRIBUTES,
			&housekeeping_task_id) != RTEMS_SUCCESSFUL) {
		rtems_shutdown_executive(0);
	}

	if (rtems_task_start(housekeeping_task_id, housekeeping_task, 0) != RTEMS_SUCCESSFUL) {
		rtems_shutdown_executive(0);
	}

*/

	init_tmtc_pool();

	rtems_message_queue_create(rtems_build_name('T','C','M','Q'), 10,
			sizeof(tc_descriptor_t),RTEMS_FIFO,&tc_message_queue_id);

	if (rtems_task_create(rtems_build_name('T','C','R','X'), 5,
			RTEMS_MINIMUM_STACK_SIZE,
			RTEMS_DEFAULT_MODES,
			RTEMS_DEFAULT_ATTRIBUTES,
			&emu_tc_rx_task_id) != RTEMS_SUCCESSFUL) {
		rtems_shutdown_executive(0);
	}

	if (rtems_task_start(emu_tc_rx_task_id, emu_tc_rx_task, 0) != RTEMS_SUCCESSFUL) {
		rtems_shutdown_executive(0);
	}

	if (rtems_task_create(rtems_build_name('T','M','N','R'), 8,
				RTEMS_MINIMUM_STACK_SIZE,
				RTEMS_DEFAULT_MODES,
				RTEMS_DEFAULT_ATTRIBUTES,
				&manager_task_id) != RTEMS_SUCCESSFUL) {
			rtems_shutdown_executive(0);
		}

		if (rtems_task_start(manager_task_id, manager_task, 0) != RTEMS_SUCCESSFUL) {
			rtems_shutdown_executive(0);
		}



	if (rtems_task_delete(RTEMS_SELF) != RTEMS_SUCCESSFUL) {
		rtems_shutdown_executive(0);
	}



}




