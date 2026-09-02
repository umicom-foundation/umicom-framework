/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/context_binding.h
 *
 * PURPOSE:
 *   Maintain bounded colour-linked context groups independently of frontend widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_CONTEXT_BINDING_H
#define UMICOM_APPLICATION_RUNTIME_CONTEXT_BINDING_H

#include "umicom/application/runtime/types.h"
#include "umicom/ui/context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application context binding entry data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationContextBindingEntry {
    char group_id[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    char value[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    uint64_t revision;
} UmiApplicationContextBindingEntry;

/**
 * Represent the application context binding store data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationContextBindingStore {
    uint32_t structure_size;
    UmiApplicationContextBindingEntry entries[UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS];
    size_t entry_count;
    uint64_t revision;
} UmiApplicationContextBindingStore;

/**
 * Initialise application context binding store from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_context_binding_store_init(
    UmiApplicationContextBindingStore *store);
/**
 * Copy application context binding into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_application_context_binding_set(
    UmiApplicationContextBindingStore *store,
    const char *group_id,
    const char *value);
/**
 * Provide the application context binding get operation used by this module and its client
 * applications.
 */
const char *umi_application_context_binding_get(
    const UmiApplicationContextBindingStore *store,
    const char *group_id);
/**
 * Release or reset state held by application context binding so the same storage can be
 * reused safely.
 */
UmiStatus umi_application_context_binding_clear(
    UmiApplicationContextBindingStore *store,
    const char *group_id);
/* Copy current context-link values into the canonical UI context store.
 * This keeps command/menu expressions and linked application panels on one context authority. */
UmiStatus umi_application_context_binding_apply_to_ui(
    const UmiApplicationContextBindingStore *store,
    UmiUiContextStore *ui_context);

#ifdef __cplusplus
}
#endif

#endif
