#include <rtems.h>
#include <rtems/libio.h>
#include <bsp/irq.h>
#include <fcntl.h>



#include "tmtc_channel.h"
#include "tm_descriptor.h"
#include "serialize.h"
#include "riscv_uart.h"
#include "tmtc_pool.h"
#include "tc_descriptor.h"
#include "driver.h"


rtems_id tc_message_queue_id;

/**
 * \brief Identifier of the TM channel mutex.
 */
static rtems_id tm_channel_mutex_id;


static int uart_fd;

rtems_status_code init_tmtc_channel() {

    rtems_status_code status;

    uart_fd = open("uart0", O_RDWR);

    if (uart_fd < 0) {
        return RTEMS_IO_ERROR;
    }

    status = rtems_semaphore_create(
            rtems_build_name('T', 'M', 'C', 'H'), 1,
            RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING,
            8, &tm_channel_mutex_id);

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

    rtems_semaphore_obtain(tm_channel_mutex_id, RTEMS_WAIT,
            RTEMS_NO_TIMEOUT);

    write(uart_fd, frame_header, 6);
    write(uart_fd, descriptor.p_tm_bytes, descriptor.tm_num_bytes);

    rtems_semaphore_release(tm_channel_mutex_id);

    tmtc_pool_free(descriptor.p_tm_bytes);

    return RTEMS_SUCCESSFUL;

}


rtems_id tc_rx_task_id;

rtems_task tc_rx_task (rtems_task_argument ignored) {

    // Send message
    tc_descriptor_t tc_descriptor;

    while (1) {

        uint8_t frame_header[6];

        tc_descriptor.tc_num_bytes = 0;

        // TODO: Allocate a new block from the pool to store the telecommand
        tc_descriptor.p_tc_bytes = tmtc_pool_alloc();

        // TODO: Read the frame header (6 bytes)
        read(uart_fd, frame_header, 6);

        // TODO: Deserialize the size field
        tc_descriptor.tc_num_bytes = deserialize_uint16(&frame_header[4]);

        // TODO: Read the telecommand and store it into the descriptor
        read(uart_fd, tc_descriptor.p_tc_bytes, tc_descriptor.tc_num_bytes);

        // TODO: Deliver the telecommand through the message queue
        rtems_message_queue_send(tc_message_queue_id, &tc_descriptor,
        	            sizeof(tc_descriptor_t));


    }

    rtems_shutdown_executive(1);

}
