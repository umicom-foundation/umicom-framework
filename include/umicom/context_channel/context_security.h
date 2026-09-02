/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_security.h
 *
 * PURPOSE:
 *   Record security decisions for context publication and cross-application sharing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_SECURITY_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_SECURITY_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context security data shared with callers of this public contract.
 */
typedef struct UmiContextSecurity {
    uint32_t structure_size;
    char decision_id[UMI_CONTEXT_VALUE_CAPACITY];
    char subject_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextSecurity;
/**
 * Initialise context security from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_security_init(UmiContextSecurity *state);
/**
 * Provide the context security set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_security_set_field(UmiContextSecurity *state,size_t field_index,const char *value);
/**
 * Provide the context security field operation used by this module and its client
 * applications.
 */
const char *umi_context_security_field(const UmiContextSecurity *state,size_t field_index);
/**
 * Provide the context security record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_security_record_success(UmiContextSecurity *state,uint64_t sequence);
/**
 * Provide the context security record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_security_record_failure(UmiContextSecurity *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context security satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_security_validate(const UmiContextSecurity *state);
/**
 * Provide the context security covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_security_covers_sequence(const UmiContextSecurity *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
