/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/active_context_store.h
 *
 * PURPOSE:
 *   Retain current typed contexts using compact bounded dynamic storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_ACTIVE_CONTEXT_STORE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_ACTIVE_CONTEXT_STORE_H

#include "umicom/workbench_context_link/active_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link active slot data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkActiveSlot {
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    UmiContextPayload payload;
    uint64_t sequence;
    uint64_t published_at_ms;
    uint64_t revision;
    bool occupied;
} UmiWorkbenchContextLinkActiveSlot;

/**
 * Represent the workbench context link active context store data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextLinkActiveContextStore {
    UmiWorkbenchContextLinkActiveSlot *slots;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextLinkActiveContextStore;

/**
 * Initialise workbench context link active context store from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_link_active_context_store_init(
    UmiWorkbenchContextLinkActiveContextStore *store);
/**
 * Release or reset state held by workbench context link active context store so the same
 * storage can be reused safely.
 */
void umi_workbench_context_link_active_context_store_destroy(
    UmiWorkbenchContextLinkActiveContextStore *store);
/**
 * Provide the workbench context link active context store put operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_active_context_store_put(
    UmiWorkbenchContextLinkActiveContextStore *store,
    const char *group_id,
    const UmiContextPayload *payload,
    uint64_t sequence,
    uint64_t published_at_ms);
/**
 * Find workbench context link active context store while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiWorkbenchContextLinkActiveSlot *
umi_workbench_context_link_active_context_store_find(
    const UmiWorkbenchContextLinkActiveContextStore *store,
    const char *group_id);
/**
 * Release or reset state held by workbench context link active context store so the same
 * storage can be reused safely.
 */
UmiStatus umi_workbench_context_link_active_context_store_clear(
    UmiWorkbenchContextLinkActiveContextStore *store,
    const char *group_id);

#ifdef __cplusplus
}
#endif

#endif
