#include <rtems.h>

#include <stdio.h>
#include <stdlib.h>

#include "housekeeping.h"

#define N_HK_DATA 16
#define HK_INITIAL_INTERVAL 10

static uint8_t hk_parameters[N_HK_DATA];

static uint32_t interval = HK_INITIAL_INTERVAL;
static uint32_t interval_control = 0;

// This variable will store the random seed
static unsigned int seed;

void do_housekeeping(void) {

        for(uint8_t i = 0 ; i < N_HK_DATA ; i = i + 1){
        	hk_parameters[i] = rand_r(&seed)%128;
        }

		interval_control = interval_control + 1;

		if(interval_control == interval){
			for(uint8_t j = 0 ; j < N_HK_DATA ; j = j + 1){
				printf("Parameter %d -value: %d\n", j, hk_parameters[j]);
			}

			interval_control = 0;
		}
	}



void init_housekeeping(void) {

    // Initialize random seed with a default value
    seed = 0;

}
