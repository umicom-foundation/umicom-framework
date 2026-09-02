/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_profile.h
 *
 * PURPOSE:
 *   Describe named context-channel profiles for Studio, Trader, TMS and Desk.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_PROFILE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_PROFILE_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context profile data shared with callers of this public contract.
 */
typedef struct UmiContextProfile {
    uint32_t structure_size;
    char profile_id[UMI_CONTEXT_VALUE_CAPACITY];
    char application_id[UMI_CONTEXT_VALUE_CAPACITY];
    char default_channel[UMI_CONTEXT_VALUE_CAPACITY];
    char default_schema[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextProfile;
/**
 * Initialise context profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_profile_init(UmiContextProfile *state);
/**
 * Provide the context profile set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_profile_set_field(UmiContextProfile *state,size_t field_index,const char *value);
/**
 * Provide the context profile field operation used by this module and its client
 * applications.
 */
const char *umi_context_profile_field(const UmiContextProfile *state,size_t field_index);
/**
 * Provide the context profile record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_profile_record_success(UmiContextProfile *state,uint64_t sequence);
/**
 * Provide the context profile record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_profile_record_failure(UmiContextProfile *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context profile satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_profile_validate(const UmiContextProfile *state);
/**
 * Provide the context profile covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_profile_covers_sequence(const UmiContextProfile *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
