/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_sync.h
 *
 * PURPOSE:
 *   Track cross-process context synchronisation cursors and bounded lag.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_SYNC_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_SYNC_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextSync {
    uint32_t structure_size;
    char peer_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char last_context_id[UMI_CONTEXT_VALUE_CAPACITY];
    char error_text[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextSync;
void umi_context_sync_init(UmiContextSync *state);
UmiStatus umi_context_sync_set_field(UmiContextSync *state,size_t field_index,const char *value);
const char *umi_context_sync_field(const UmiContextSync *state,size_t field_index);
UmiStatus umi_context_sync_record_success(UmiContextSync *state,uint64_t sequence);
UmiStatus umi_context_sync_record_failure(UmiContextSync *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_sync_validate(const UmiContextSync *state);
bool umi_context_sync_covers_sequence(const UmiContextSync *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
