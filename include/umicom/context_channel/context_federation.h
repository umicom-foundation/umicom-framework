/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_federation.h
 *
 * PURPOSE:
 *   Describe local and remote application context peers without changing core contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_FEDERATION_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_FEDERATION_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context federation data shared with callers of this public contract.
 */
typedef struct UmiContextFederation {
    uint32_t structure_size;
    char federation_id[UMI_CONTEXT_VALUE_CAPACITY];
    char peer_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char transport_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextFederation;
/**
 * Initialise context federation from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_federation_init(UmiContextFederation *state);
/**
 * Provide the context federation set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_federation_set_field(UmiContextFederation *state,size_t field_index,const char *value);
/**
 * Provide the context federation field operation used by this module and its client
 * applications.
 */
const char *umi_context_federation_field(const UmiContextFederation *state,size_t field_index);
/**
 * Provide the context federation record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_federation_record_success(UmiContextFederation *state,uint64_t sequence);
/**
 * Provide the context federation record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_federation_record_failure(UmiContextFederation *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context federation satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_federation_validate(const UmiContextFederation *state);
/**
 * Provide the context federation covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_federation_covers_sequence(const UmiContextFederation *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
