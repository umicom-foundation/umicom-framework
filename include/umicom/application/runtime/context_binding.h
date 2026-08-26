/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/context_binding.h
 *
 * PURPOSE:
 *   Maintain bounded TWS-style context-link group values independently of frontend widgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_CONTEXT_BINDING_H
#define UMICOM_APPLICATION_RUNTIME_CONTEXT_BINDING_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationContextBindingEntry {
    char group_id[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    char value[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    uint64_t revision;
} UmiApplicationContextBindingEntry;

typedef struct UmiApplicationContextBindingStore {
    uint32_t structure_size;
    UmiApplicationContextBindingEntry entries[UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS];
    size_t entry_count;
    uint64_t revision;
} UmiApplicationContextBindingStore;

void umi_application_context_binding_store_init(
    UmiApplicationContextBindingStore *store);
UmiStatus umi_application_context_binding_set(
    UmiApplicationContextBindingStore *store,
    const char *group_id,
    const char *value);
const char *umi_application_context_binding_get(
    const UmiApplicationContextBindingStore *store,
    const char *group_id);
UmiStatus umi_application_context_binding_clear(
    UmiApplicationContextBindingStore *store,
    const char *group_id);

#ifdef __cplusplus
}
#endif

#endif
