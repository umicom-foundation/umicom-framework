/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_transport.h
 *
 * PURPOSE:
 *   Describe transport-neutral context delivery endpoints and capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_TRANSPORT_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_TRANSPORT_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextTransport {
    uint32_t structure_size;
    char transport_id[UMI_CONTEXT_VALUE_CAPACITY];
    char endpoint_id[UMI_CONTEXT_VALUE_CAPACITY];
    char protocol[UMI_CONTEXT_VALUE_CAPACITY];
    char address[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextTransport;
void umi_context_transport_init(UmiContextTransport *state);
UmiStatus umi_context_transport_set_field(UmiContextTransport *state,size_t field_index,const char *value);
const char *umi_context_transport_field(const UmiContextTransport *state,size_t field_index);
UmiStatus umi_context_transport_record_success(UmiContextTransport *state,uint64_t sequence);
UmiStatus umi_context_transport_record_failure(UmiContextTransport *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_transport_validate(const UmiContextTransport *state);
bool umi_context_transport_covers_sequence(const UmiContextTransport *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
