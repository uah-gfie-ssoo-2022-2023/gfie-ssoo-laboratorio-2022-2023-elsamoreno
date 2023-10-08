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
#include "serialize.h"
#include "epd_pus_tmtc.h"
#include "tm_descriptor.h"
#include "ccsds_pus_format.h"
#include "riscv_gpio.h"


rtems_id housekeeping_task_id;

rtems_task housekeeping_task (rtems_task_argument ignored) {

	rtems_interval next_time = 0;

	init_housekeeping();

	rtems_interval period = rtems_clock_get_ticks_per_second();

	while (1) {

		// Check if we have any TC:
		tc_descriptor_t tc_descriptor;


		size_t size;

		// Let's check if there is a telecommand
		if (rtems_message_queue_receive(hk_message_queue_id,
				&tc_descriptor, &size,
				RTEMS_NO_WAIT, 0) == RTEMS_SUCCESSFUL) {

			// If we are here, it means that we have received a TC

			// Decode the telecommand
			ccsds_pus_tmtc_packet_header_t tc_packet_header;
			ccsds_pus_tc_df_header_t tc_df_header;
			uint16_t tc_packet_err_ctrl;

			ccsds_pus_tc_get_fields(tc_descriptor.p_tc_bytes,
					&tc_packet_header, &tc_df_header, &tc_packet_err_ctrl);

			// NOTE: we are assuming that the packet is well formed

			// Get the SID
			uint8_t sid = tc_descriptor.p_tc_bytes[10];

			// Check SID
			if (sid == 0) {

				// TODO: Deserialize the new housekeeping interval
				uint32_t new_interval = deserialize_uint16(&tc_descriptor.p_tc_bytes[11]);
				// TODO: Update interval with the new housekeeping interval
				interval = new_interval;
				// TODO: Clear current interval control
				interval_control = 0;
				// TODO: Generate TM (1,7)
				tm_descriptor_t tm_descriptor;

				tm_descriptor.p_tm_bytes = tmtc_pool_alloc();

				if(tm_descriptor.p_tm_bytes != NULL){

					uint16_t tm_count = tm_channel_get_next_tm_count();

					epd_pus_build_tm_1_7(&tm_descriptor, tm_count,
							tc_packet_header.packet_id,
							tc_packet_header.packet_seq_ctrl);


					tm_channel_send_tm(tm_descriptor);
				}

			} else {

				// Wrong SID

				// TODO: Generate TM (1,8)
				tm_descriptor_t tm_descriptor;

				tm_descriptor.p_tm_bytes = tmtc_pool_alloc();

				if(tm_descriptor.p_tm_bytes != NULL){

					uint16_t tm_count = tm_channel_get_next_tm_count();

					epd_pus_build_tm_1_8(&tm_descriptor, tm_count,
							tc_packet_header.packet_id,
							tc_packet_header.packet_seq_ctrl);


					tm_channel_send_tm(tm_descriptor);
				}

			}

			tmtc_pool_free(tc_descriptor.p_tc_bytes);

		}

		do_housekeeping();

		next_time = next_time + period;

		task_delay_until(next_time);

	}

}

rtems_task Init(rtems_task_argument ignored)
{
	init_gpio();
	init_tmtc_pool();
	init_tmtc_channel();


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





	rtems_message_queue_create(rtems_build_name('T','C','M','Q'), 10,
			sizeof(tc_descriptor_t),RTEMS_FIFO,&tc_message_queue_id);

	rtems_message_queue_create(rtems_build_name('H','K','M','Q'), 5,
			sizeof(tc_descriptor_t),RTEMS_FIFO,&hk_message_queue_id);

	if (rtems_task_create(rtems_build_name('T','C','R','X'), 5,
			RTEMS_MINIMUM_STACK_SIZE,
			RTEMS_DEFAULT_MODES,
			RTEMS_DEFAULT_ATTRIBUTES,
			&tc_rx_task_id) != RTEMS_SUCCESSFUL) {
		rtems_shutdown_executive(0);
	}

	if (rtems_task_start(tc_rx_task_id, tc_rx_task, 0) != RTEMS_SUCCESSFUL) {
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
