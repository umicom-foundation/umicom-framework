/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_access.h
 *
 * PURPOSE:
 *   Evaluate user and workload access to channel operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_ACCESS_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_ACCESS_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextAccess {
    uint32_t structure_size;
    char access_id[UMI_CONTEXT_VALUE_CAPACITY];
    char subject_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char action_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextAccess;
void umi_context_access_init(UmiContextAccess *state);
UmiStatus umi_context_access_set_field(UmiContextAccess *state,size_t field_index,const char *value);
const char *umi_context_access_field(const UmiContextAccess *state,size_t field_index);
UmiStatus umi_context_access_record_success(UmiContextAccess *state,uint64_t sequence);
UmiStatus umi_context_access_record_failure(UmiContextAccess *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_access_validate(const UmiContextAccess *state);
bool umi_context_access_covers_sequence(const UmiContextAccess *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
