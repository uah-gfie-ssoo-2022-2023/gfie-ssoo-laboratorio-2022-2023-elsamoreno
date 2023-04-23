#ifndef INCLUDE_TC_DESCRIPTOR_H_
#define INCLUDE_TC_DESCRIPTOR_H_

/**
 * \brief Structure that stores a single TC descriptor. It contains a pointer
 *        to the allocated memory that stores the TC packet and the number of
 *        bytes that are actually in use by the TC packet.
 */
struct tc_descriptor {

    uint8_t * p_tc_bytes;
    uint16_t tc_num_bytes;

};

typedef struct tc_descriptor tc_descriptor_t;


#endif /* INCLUDE_TC_DESCRIPTOR_H_ */
