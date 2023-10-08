#include "basic_types.h"
#include "ccsds_pus_format.h"
#include "serialize.h"


void ccsds_pus_tc_get_fields(uint8_t tc_bytes[256],
                             ccsds_pus_tmtc_packet_header_t * p_tc_packet_header,
                             ccsds_pus_tc_df_header_t * p_tc_df_header,
                             uint16_t * p_tc_packet_err_ctrl) {

    (*p_tc_packet_header).packet_id = deserialize_uint16(&tc_bytes[0]);

    (*p_tc_packet_header).packet_seq_ctrl = deserialize_uint16(&tc_bytes[2]);

    (*p_tc_packet_header).packet_length = deserialize_uint16(&tc_bytes[4]);

    (*p_tc_df_header).flag_ver_ack = tc_bytes[6];

    (*p_tc_df_header).type = tc_bytes[7];

    (*p_tc_df_header).subtype = tc_bytes[8];

    (*p_tc_df_header).sourceID = tc_bytes[9];

    uint8_t packet_error_ctrl[2];

    {
        for (uint32_t __i0 = 0; __i0 < 2; __i0 = __i0 + 1) {
            packet_error_ctrl[__i0] = 0;
        }
    }

    packet_error_ctrl[0] = tc_bytes[(uint32_t)((*p_tc_packet_header).packet_length + 5)];

    packet_error_ctrl[1] = tc_bytes[(uint32_t)((*p_tc_packet_header).packet_length + 6)];

    *p_tc_packet_err_ctrl = deserialize_uint16(packet_error_ctrl);

    return;

}




void ccsds_pus_tm_set_fields(uint8_t tm_bytes[256],
                             ccsds_pus_tmtc_packet_header_t * p_tm_packet_header,
                             ccsds_pus_tm_df_header_t * p_tm_df_header) {

    serialize_uint16((*p_tm_packet_header).packet_id, &tm_bytes[0]);

    serialize_uint16((*p_tm_packet_header).packet_seq_ctrl, &tm_bytes[2]);

    serialize_uint16((*p_tm_packet_header).packet_length, &tm_bytes[4]);

    tm_bytes[6] = (*p_tm_df_header).version;

    tm_bytes[7] = (*p_tm_df_header).type;

    tm_bytes[8] = (*p_tm_df_header).subtype;

    tm_bytes[9] = (*p_tm_df_header).destinationID;

    return;

}
