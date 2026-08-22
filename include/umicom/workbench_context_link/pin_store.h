/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/pin_store.h
 *
 * PURPOSE:
 *   Retain explicitly pinned typed contexts using bounded dynamically allocated storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PIN_STORE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PIN_STORE_H

#include "umicom/workbench_context_link/pinned_context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkPinItem {
    char pin_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    UmiContextPayload payload;
    uint64_t pinned_at_ms;
} UmiWorkbenchContextLinkPinItem;

typedef struct UmiWorkbenchContextLinkPinStore {
    UmiWorkbenchContextLinkPinItem *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextLinkPinStore;

void umi_workbench_context_link_pin_store_init(
    UmiWorkbenchContextLinkPinStore *store);
void umi_workbench_context_link_pin_store_destroy(
    UmiWorkbenchContextLinkPinStore *store);
UmiStatus umi_workbench_context_link_pin_store_put(
    UmiWorkbenchContextLinkPinStore *store,
    const char *pin_id,
    const char *group_id,
    const UmiContextPayload *payload,
    uint64_t pinned_at_ms);
UmiStatus umi_workbench_context_link_pin_store_remove(
    UmiWorkbenchContextLinkPinStore *store,
    const char *pin_id);
const UmiWorkbenchContextLinkPinItem *
umi_workbench_context_link_pin_store_find(
    const UmiWorkbenchContextLinkPinStore *store,
    const char *pin_id);

#ifdef __cplusplus
}
#endif

#endif
