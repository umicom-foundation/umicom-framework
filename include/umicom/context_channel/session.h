/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/session.h
 *
 * PURPOSE:
 *   Retain context navigation state across application sessions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_SESSION_H
#define UMICOM_CONTEXT_CHANNEL_SESSION_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_SESSION_MAX_ITEMS 128U
typedef struct UmiContextSession {
    uint32_t structure_size;
    char session_id[UMI_CONTEXT_TEXT_CAPACITY];
    char user_id[UMI_CONTEXT_TEXT_CAPACITY];
    char workspace_id[UMI_CONTEXT_TEXT_CAPACITY];
    char active_channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char active_context_id[UMI_CONTEXT_TEXT_CAPACITY];
    uint64_t last_sequence;
    bool clean_shutdown;
    uint64_t revision;
} UmiContextSession;
typedef struct UmiContextSessionStore {
    UmiContextSession items[UMI_CONTEXT_SESSION_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextSessionStore;
void umi_context_session_init(UmiContextSession *record);
UmiStatus umi_context_session_validate(const UmiContextSession *record);
void umi_context_session_store_init(UmiContextSessionStore *store);
UmiStatus umi_context_session_store_put(UmiContextSessionStore *store,const UmiContextSession *record);
UmiStatus umi_context_session_store_remove(UmiContextSessionStore *store,const char *identity);
UmiContextSession *umi_context_session_store_find(UmiContextSessionStore *store,const char *identity);
const UmiContextSession *umi_context_session_store_find_const(const UmiContextSessionStore *store,const char *identity);
size_t umi_context_session_store_count(const UmiContextSessionStore *store);
UmiStatus umi_context_session_store_snapshot(const UmiContextSessionStore *store,UmiContextSession *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
