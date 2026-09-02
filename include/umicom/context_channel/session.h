/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/session.h
 *
 * PURPOSE:
 *   Retain context navigation state across application sessions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_SESSION_H
#define UMICOM_CONTEXT_CHANNEL_SESSION_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_SESSION_MAX_ITEMS 128U
/**
 * Represent the context session data shared with callers of this public contract.
 */
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
/**
 * Represent the context session store data shared with callers of this public contract.
 */
typedef struct UmiContextSessionStore {
    UmiContextSession items[UMI_CONTEXT_SESSION_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextSessionStore;
/**
 * Initialise context session from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_session_init(UmiContextSession *record);
/**
 * Check that context session satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_session_validate(const UmiContextSession *record);
/**
 * Initialise context session store from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_session_store_init(UmiContextSessionStore *store);
/**
 * Provide the context session store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_session_store_put(UmiContextSessionStore *store,const UmiContextSession *record);
/**
 * Remove context session store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_session_store_remove(UmiContextSessionStore *store,const char *identity);
/**
 * Find context session store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiContextSession *umi_context_session_store_find(UmiContextSessionStore *store,const char *identity);
/**
 * Provide the context session store find const operation used by this module and its
 * client applications.
 */
const UmiContextSession *umi_context_session_store_find_const(const UmiContextSessionStore *store,const char *identity);
/**
 * Return the number of records represented by context session store without changing their
 * state.
 */
size_t umi_context_session_store_count(const UmiContextSessionStore *store);
/**
 * Provide the context session store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_session_store_snapshot(const UmiContextSessionStore *store,UmiContextSession *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
