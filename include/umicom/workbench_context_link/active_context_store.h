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

typedef struct UmiWorkbenchContextLinkActiveSlot {
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    UmiContextPayload payload;
    uint64_t sequence;
    uint64_t published_at_ms;
    uint64_t revision;
    bool occupied;
} UmiWorkbenchContextLinkActiveSlot;

typedef struct UmiWorkbenchContextLinkActiveContextStore {
    UmiWorkbenchContextLinkActiveSlot *slots;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextLinkActiveContextStore;

void umi_workbench_context_link_active_context_store_init(
    UmiWorkbenchContextLinkActiveContextStore *store);
void umi_workbench_context_link_active_context_store_destroy(
    UmiWorkbenchContextLinkActiveContextStore *store);
UmiStatus umi_workbench_context_link_active_context_store_put(
    UmiWorkbenchContextLinkActiveContextStore *store,
    const char *group_id,
    const UmiContextPayload *payload,
    uint64_t sequence,
    uint64_t published_at_ms);
const UmiWorkbenchContextLinkActiveSlot *
umi_workbench_context_link_active_context_store_find(
    const UmiWorkbenchContextLinkActiveContextStore *store,
    const char *group_id);
UmiStatus umi_workbench_context_link_active_context_store_clear(
    UmiWorkbenchContextLinkActiveContextStore *store,
    const char *group_id);

#ifdef __cplusplus
}
#endif

#endif
