/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_profile.h
 *
 * PURPOSE:
 *   Describe named context-channel profiles for Studio, Trader, TMS and Desk.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_PROFILE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_PROFILE_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_context_profile_init(UmiContextProfile *state);
UmiStatus umi_context_profile_set_field(UmiContextProfile *state,size_t field_index,const char *value);
const char *umi_context_profile_field(const UmiContextProfile *state,size_t field_index);
UmiStatus umi_context_profile_record_success(UmiContextProfile *state,uint64_t sequence);
UmiStatus umi_context_profile_record_failure(UmiContextProfile *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_profile_validate(const UmiContextProfile *state);
bool umi_context_profile_covers_sequence(const UmiContextProfile *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
