/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_bookmark_service.h
 *
 * PURPOSE:
 *   Coordinate bookmark creation, removal and user ownership.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_BOOKMARK_SERVICE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_BOOKMARK_SERVICE_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextBookmarkService {
    uint32_t structure_size;
    char operation_id[UMI_CONTEXT_VALUE_CAPACITY];
    char bookmark_id[UMI_CONTEXT_VALUE_CAPACITY];
    char user_id[UMI_CONTEXT_VALUE_CAPACITY];
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextBookmarkService;
void umi_context_bookmark_service_init(UmiContextBookmarkService *state);
UmiStatus umi_context_bookmark_service_set_field(UmiContextBookmarkService *state,size_t field_index,const char *value);
const char *umi_context_bookmark_service_field(const UmiContextBookmarkService *state,size_t field_index);
UmiStatus umi_context_bookmark_service_record_success(UmiContextBookmarkService *state,uint64_t sequence);
UmiStatus umi_context_bookmark_service_record_failure(UmiContextBookmarkService *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_bookmark_service_validate(const UmiContextBookmarkService *state);
bool umi_context_bookmark_service_covers_sequence(const UmiContextBookmarkService *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
