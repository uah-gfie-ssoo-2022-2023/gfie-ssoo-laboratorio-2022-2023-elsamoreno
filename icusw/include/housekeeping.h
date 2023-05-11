#ifndef INCLUDE_HOUSEKEEPING_H_
#define INCLUDE_HOUSEKEEPING_H_

/**
 * \brief Housekeeping telemetry generation interval
 */
extern uint32_t interval;

/**
 * \brief Control variable for telemetry generation
 */
extern uint32_t interval_control;

/**
 * \brief Initialize housekeeping parameters
 */
void init_housekeeping(void);

/**
 * \brief Perform housekeeping and telemetry generation
 */
void do_housekeeping(void);

/**
 * \brief Identifier of the housekeeping task's message queue.
 */
extern rtems_id hk_message_queue_id;

#endif /* INCLUDE_HOUSEKEEPING__H_ */
