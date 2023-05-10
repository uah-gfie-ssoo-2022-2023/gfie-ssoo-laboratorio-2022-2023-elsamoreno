#include <rtems.h>

#include "tmtc_channel.h"
#include "tm_descriptor.h"
#include "serialize.h"
#include "riscv_uart.h"
#include "tmtc_pool.h"

rtems_id tc_message_queue_id;

/**
 * \brief Identifier of the TM channel mutex.
 */
static rtems_id tm_channel_mutex_id;


rtems_status_code init_tm_channel() {

    rtems_status_code status;

    riscv_uart_enable_TX();

    status = rtems_semaphore_create(
            rtems_build_name('T','M','C','H'),
            1,
            RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING,
            8,
            &tm_channel_mutex_id
        );


    return status;

}

/**
 * \brief Global variable that stores the next telemetry sequence count to be
 *        assigned.
 */
static uint16_t tm_count = 0;


uint16_t tm_channel_get_next_tm_count() {

    uint16_t ret;

    rtems_semaphore_obtain(tm_channel_mutex_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    ret = tm_count;
    tm_count = (tm_count + 1) & 0x3FFF;
    rtems_semaphore_release(tm_channel_mutex_id);

    return ret;

}


rtems_status_code tm_channel_send_tm(tm_descriptor_t descriptor) {

    uint8_t frame_header[6];

    frame_header[0] = 0xBE;
    frame_header[1] = 0xBA;
    frame_header[2] = 0xBE;
    frame_header[3] = 0xEF;

    serialize_uint16(descriptor.tm_num_bytes, &frame_header[4]);

    rtems_semaphore_obtain(tm_channel_mutex_id, RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT);

    for (uint8_t i = 0; i < 6; i = i + 1) {

        if (riscv_putchar(frame_header[i]) != 0) {
            return RTEMS_IO_ERROR;
        }

    }


   // Send the TM Packet
   for(uint8_t i = 0; i < descriptor.tm_num_bytes ; i++){

	   if (riscv_putchar(*(descriptor.p_tm_bytes + i)) != 0) {
	               return RTEMS_IO_ERROR;
	           }
   }

    // Wait until the last byte has been transmitted
    while(!riscv_uart_tx_fifo_is_empty());


    rtems_semaphore_release(tm_channel_mutex_id);

    //Free the TM descriptor to the pool
    tmtc_pool_free(descriptor.p_tm_bytes);



    return RTEMS_SUCCESSFUL;

}
