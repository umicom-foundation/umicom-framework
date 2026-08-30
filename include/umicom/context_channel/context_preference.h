/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_preference.h
 *
 * PURPOSE:
 *   Store user-facing context-link preferences independently of GTK widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_PREFERENCE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_PREFERENCE_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextPreference {
    uint32_t structure_size;
    char preference_id[UMI_CONTEXT_VALUE_CAPACITY];
    char user_id[UMI_CONTEXT_VALUE_CAPACITY];
    char default_colour[UMI_CONTEXT_VALUE_CAPACITY];
    char default_channel[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextPreference;
void umi_context_preference_init(UmiContextPreference *state);
UmiStatus umi_context_preference_set_field(UmiContextPreference *state,size_t field_index,const char *value);
const char *umi_context_preference_field(const UmiContextPreference *state,size_t field_index);
UmiStatus umi_context_preference_record_success(UmiContextPreference *state,uint64_t sequence);
UmiStatus umi_context_preference_record_failure(UmiContextPreference *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_preference_validate(const UmiContextPreference *state);
bool umi_context_preference_covers_sequence(const UmiContextPreference *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
