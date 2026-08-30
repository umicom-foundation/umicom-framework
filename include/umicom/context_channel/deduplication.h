/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/deduplication.h
 *
 * PURPOSE:
 *   Prevent feedback loops and duplicate propagation across linked panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_DEDUPLICATION_H
#define UMICOM_CONTEXT_CHANNEL_DEDUPLICATION_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextDeduplicationEntry { char context_id[UMI_CONTEXT_TEXT_CAPACITY]; uint64_t content_hash; uint64_t sequence; } UmiContextDeduplicationEntry;
typedef struct UmiContextDeduplication { UmiContextDeduplicationEntry entries[UMI_CONTEXT_MAX_RECENT_IDS]; size_t count; size_t next_index; uint64_t duplicate_count; } UmiContextDeduplication;
void umi_context_deduplication_init(UmiContextDeduplication *state);
bool umi_context_deduplication_seen(const UmiContextDeduplication *state,const UmiContextPayload *payload);
UmiStatus umi_context_deduplication_record(UmiContextDeduplication *state,const UmiContextPayload *payload);
#ifdef __cplusplus
}
#endif
#endif
