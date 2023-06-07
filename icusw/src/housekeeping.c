#include <rtems.h>

#include <stdio.h>
#include <stdlib.h>

#include "housekeeping.h"
#include "tm_descriptor.h"
#include "epd_pus_tmtc.h"
#include "ccsds_pus_format.h"
#include "tmtc_pool.h"
#include "tmtc_channel.h"
#include "riscv_gpio.h"

rtems_id hk_message_queue_id;

#define N_HK_DATA 16
#define HK_INITIAL_INTERVAL 10

static uint8_t hk_parameters[N_HK_DATA];

uint32_t interval = HK_INITIAL_INTERVAL;
uint32_t interval_control = 0;

// This variable will store the random seed
static unsigned int seed;

void do_housekeeping(void) {

	// Now the first three parameters are the state of SW[0..2]
	for (uint32_t i = 0; i < 3; i = i + 1) {
	    hk_parameters[i] = read_switch(i);
	}

	// And the rest are generted randomly as usual
	for (uint32_t i = 3; i < N_HK_DATA; i = i + 1) {
	    hk_parameters[i] = rand_r(&seed) % 128;
	}

	// We change the state of the LED on each activation
	write_led(0, interval_control % 2);

	interval_control = interval_control + 1;

	if(interval_control == interval){
		/*
			for(uint8_t j = 0 ; j < N_HK_DATA ; j = j + 1){
				printf("Parameter %d -value: %d\n", j, hk_parameters[j]);
			}*/
		tm_descriptor_t tm_descriptor;

		tm_descriptor.p_tm_bytes = tmtc_pool_alloc();

		if(tm_descriptor.p_tm_bytes != NULL){

			uint16_t tm_count = tm_channel_get_next_tm_count();

			epd_pus_build_tm_3_25_sid0(&tm_descriptor, tm_count,
					hk_parameters,
					N_HK_DATA);


			tm_channel_send_tm(tm_descriptor);
		}

		interval_control = 0;
	}
}



void init_housekeeping(void) {

	// Initialize random seed with a default value
	seed = 0;

}
