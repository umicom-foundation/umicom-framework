/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/pin_store.h
 *
 * PURPOSE:
 *   Retain explicitly pinned typed contexts using bounded dynamically allocated storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PIN_STORE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PIN_STORE_H

#include "umicom/workbench_context_link/pinned_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link pin item data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkPinItem {
    char pin_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    UmiContextPayload payload;
    uint64_t pinned_at_ms;
} UmiWorkbenchContextLinkPinItem;

/**
 * Represent the workbench context link pin store data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkPinStore {
    UmiWorkbenchContextLinkPinItem *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextLinkPinStore;

/**
 * Initialise workbench context link pin store from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_pin_store_init(
    UmiWorkbenchContextLinkPinStore *store);
/**
 * Release or reset state held by workbench context link pin store so the same storage can
 * be reused safely.
 */
void umi_workbench_context_link_pin_store_destroy(
    UmiWorkbenchContextLinkPinStore *store);
/**
 * Provide the workbench context link pin store put operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_link_pin_store_put(
    UmiWorkbenchContextLinkPinStore *store,
    const char *pin_id,
    const char *group_id,
    const UmiContextPayload *payload,
    uint64_t pinned_at_ms);
/**
 * Remove workbench context link pin store while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_workbench_context_link_pin_store_remove(
    UmiWorkbenchContextLinkPinStore *store,
    const char *pin_id);
/**
 * Find workbench context link pin store while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchContextLinkPinItem *
umi_workbench_context_link_pin_store_find(
    const UmiWorkbenchContextLinkPinStore *store,
    const char *pin_id);

#ifdef __cplusplus
}
#endif

#endif
