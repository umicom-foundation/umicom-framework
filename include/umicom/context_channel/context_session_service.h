/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_session_service.h
 *
 * PURPOSE:
 *   Coordinate context session restoration and clean shutdown evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_SESSION_SERVICE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_SESSION_SERVICE_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context session service data shared with callers of this public contract.
 */
typedef struct UmiContextSessionService {
    uint32_t structure_size;
    char operation_id[UMI_CONTEXT_VALUE_CAPACITY];
    char session_id[UMI_CONTEXT_VALUE_CAPACITY];
    char workspace_id[UMI_CONTEXT_VALUE_CAPACITY];
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextSessionService;
/**
 * Initialise context session service from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_session_service_init(UmiContextSessionService *state);
/**
 * Provide the context session service set field operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_session_service_set_field(UmiContextSessionService *state,size_t field_index,const char *value);
/**
 * Provide the context session service field operation used by this module and its client
 * applications.
 */
const char *umi_context_session_service_field(const UmiContextSessionService *state,size_t field_index);
/**
 * Provide the context session service record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_session_service_record_success(UmiContextSessionService *state,uint64_t sequence);
/**
 * Provide the context session service record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_session_service_record_failure(UmiContextSessionService *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context session service satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_context_session_service_validate(const UmiContextSessionService *state);
/**
 * Provide the context session service covers sequence operation used by this module and
 * its client applications.
 */
bool umi_context_session_service_covers_sequence(const UmiContextSessionService *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
