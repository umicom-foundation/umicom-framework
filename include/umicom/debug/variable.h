/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/variable.h
 *
 * PURPOSE:
 *   Define a DAP-friendly but adapter-neutral debugger record for native and future Umicom runtimes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_DEBUG_VARIABLE_H
#define UMICOM_DEBUG_VARIABLE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_VARIABLE_CAPACITY 2048U
#define UMI_DEBUG_VARIABLE_API_VERSION 1U

typedef struct UmiDebugVariableSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char scope_id[128];
    char name[256];
    char value[1024];
    char type[256];
    char evaluate_name[512];
    uint64_t variables_reference;
    int changed;
    uint64_t revision;
} UmiDebugVariableSnapshot;

typedef struct UmiDebugVariableRegistry UmiDebugVariableRegistry;

UmiStatus umi_debug_variable_registry_create(UmiDebugVariableRegistry **out_registry);
void umi_debug_variable_registry_destroy(UmiDebugVariableRegistry *registry);
UmiStatus umi_debug_variable_registry_upsert(UmiDebugVariableRegistry *registry, const UmiDebugVariableSnapshot *item);
UmiStatus umi_debug_variable_registry_remove(UmiDebugVariableRegistry *registry, const char *id);
UmiStatus umi_debug_variable_registry_find(const UmiDebugVariableRegistry *registry, const char *id, UmiDebugVariableSnapshot *out_item);
UmiStatus umi_debug_variable_registry_at(const UmiDebugVariableRegistry *registry, size_t index, UmiDebugVariableSnapshot *out_item);
size_t umi_debug_variable_registry_count(const UmiDebugVariableRegistry *registry);
uint64_t umi_debug_variable_registry_revision(const UmiDebugVariableRegistry *registry);
void umi_debug_variable_registry_clear(UmiDebugVariableRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
