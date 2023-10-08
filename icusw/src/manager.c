#include <rtems.h>

#include <stdio.h>

#include "manager.h"
#include "tc_descriptor.h"

#include "tmtc_channel.h"
#include "tmtc_pool.h"
#include "delay.h"
#include "ccsds_pus_format.h"
#include "epd_pus_tmtc.h"
#include "crc.h"
#include "housekeeping.h"



rtems_id manager_task_id;


uint8_t accept_tc(tc_descriptor_t * tc_descriptor) {

	uint8_t ret = 1;

	ccsds_pus_tmtc_packet_header_t tc_packet_header;
	ccsds_pus_tc_df_header_t tc_df_header;
	uint16_t tc_packet_err_ctrl;

	ccsds_pus_tc_get_fields(tc_descriptor->p_tc_bytes,
			&tc_packet_header, &tc_df_header, &tc_packet_err_ctrl);

	// Calculate CRC
	uint16_t crc_value = cal_crc_16(tc_descriptor->p_tc_bytes,
			tc_descriptor->tc_num_bytes - 2);

	if (crc_value != tc_packet_err_ctrl) {

		// Generate TM (1,2) - Reject Incorrect CRC

		tm_descriptor_t tm_descriptor;

		tm_descriptor.p_tm_bytes = tmtc_pool_alloc();

		if(tm_descriptor.p_tm_bytes != NULL){

			uint16_t tm_count = tm_channel_get_next_tm_count();

			epd_pus_build_tm_1_2_crc_error(&tm_descriptor, tm_count,
					tc_packet_header.packet_id,
					tc_packet_header.packet_seq_ctrl,
					tc_packet_err_ctrl, crc_value);

			tm_channel_send_tm(tm_descriptor);

			// We should not use the TM descriptor beyond this point!

		}

		ret = 0;

	}

	if (ret == 1) {

		// If we are here, it means that the CRC of the TC was OK

		tm_descriptor_t tm_descriptor;

		switch (tc_df_header.type) {

		case 17:

			// Check if the TC is TC[17,1]. Otherwise, send TM[1,2]
			if (tc_df_header.subtype != 1) {

				// Generate TM[1,2] - Reject Illegal subtype
				tm_descriptor.p_tm_bytes = tmtc_pool_alloc();

				if(tm_descriptor.p_tm_bytes != NULL){

					uint16_t tm_count = tm_channel_get_next_tm_count();

					epd_pus_build_tm_1_2_illegal_subtype(&tm_descriptor, tm_count,
							tc_packet_header.packet_id,
							tc_packet_header.packet_seq_ctrl);


					tm_channel_send_tm(tm_descriptor);


				}
				ret = 0;
			}

			break;

		case 3:

			// Check if the TC is TC[3,31]. Otherwise, send TM[1,2]
			if (tc_df_header.subtype != 31) {

				// TODO: Generate TM[1,2] - Reject Illegal subtype
				tm_descriptor.p_tm_bytes = tmtc_pool_alloc();

				if(tm_descriptor.p_tm_bytes != NULL){

					uint16_t tm_count = tm_channel_get_next_tm_count();

					epd_pus_build_tm_1_2_illegal_subtype(&tm_descriptor, tm_count,
							tc_packet_header.packet_id,
							tc_packet_header.packet_seq_ctrl);


					tm_channel_send_tm(tm_descriptor);
				}


				ret = 0;
			}
			break;

		default:

			// Generate TM[1,2] - Reject Illegal type
			tm_descriptor.p_tm_bytes = tmtc_pool_alloc();

			if(tm_descriptor.p_tm_bytes != NULL){

				uint16_t tm_count = tm_channel_get_next_tm_count();

				epd_pus_build_tm_1_2_illegal_type(&tm_descriptor, tm_count,
						tc_packet_header.packet_id,
						tc_packet_header.packet_seq_ctrl);


				tm_channel_send_tm(tm_descriptor);
			}

			ret = 0;
			break;

		}
	}

	// Check if there have been any errors
	if (ret == 1) {

		// If we are here, it means that the TC was not rejected

		//Generate TM[1,1] - Accept
		tm_descriptor_t tm_descriptor;

		tm_descriptor.p_tm_bytes = tmtc_pool_alloc();

		if(tm_descriptor.p_tm_bytes != NULL){

			uint16_t tm_count = tm_channel_get_next_tm_count();

			epd_pus_build_tm_1_1(&tm_descriptor, tm_count,
					tc_packet_header.packet_id,
					tc_packet_header.packet_seq_ctrl);


			tm_channel_send_tm(tm_descriptor);

		}

	}

	return ret;
}



rtems_task manager_task (rtems_task_argument ignored) {

	while (1) {

		tc_descriptor_t tc_descriptor;

		size_t size;

		// Receive message
		rtems_message_queue_receive(tc_message_queue_id,
				&tc_descriptor, &size,
				RTEMS_WAIT, RTEMS_NO_TIMEOUT);

		// Let's see if we shall accept the TC...
		uint8_t tc_accepted = accept_tc(&tc_descriptor);

		if (tc_accepted == 1) {

			// Yep, we accepted the TC!

			ccsds_pus_tmtc_packet_header_t tc_packet_header;
			ccsds_pus_tc_df_header_t tc_df_header;
			uint16_t tc_packet_err_ctrl;

			ccsds_pus_tc_get_fields(tc_descriptor.p_tc_bytes,
					&tc_packet_header, &tc_df_header, &tc_packet_err_ctrl);

			if (tc_df_header.type == 3 && tc_df_header.subtype == 31) {

				// TODO: Deliver the TC to the housekeeping task
				rtems_message_queue_send(hk_message_queue_id, &(tc_descriptor),sizeof(tc_descriptor_t));


			} else {
				tm_descriptor_t tm_descriptor1;
				tm_descriptor_t tm_descriptor2;

				tm_descriptor1.p_tm_bytes = tmtc_pool_alloc();
				tm_descriptor2.p_tm_bytes = tmtc_pool_alloc();

				// TODO: Generate TM[17,2]
				if(tm_descriptor1.p_tm_bytes != NULL){

					uint16_t tm_count = tm_channel_get_next_tm_count();

					epd_pus_build_tm_17_2(&tm_descriptor1, tm_count);


					tm_channel_send_tm(tm_descriptor1);
				}



				// TODO: Generate TM[1,7]
				if(tm_descriptor2.p_tm_bytes != NULL){

					uint16_t tm_count = tm_channel_get_next_tm_count();

					epd_pus_build_tm_1_7(&tm_descriptor2, tm_count,
							tc_packet_header.packet_id,
							tc_packet_header.packet_seq_ctrl);


					tm_channel_send_tm(tm_descriptor2);
				}

				tmtc_pool_free(tc_descriptor.p_tc_bytes);

			}



		} else {

			// We did not accept the TC, so nothing further has to be done
			tmtc_pool_free(tc_descriptor.p_tc_bytes);

		}
	}
}






