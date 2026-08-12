/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/module.h
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
#ifndef UMICOM_DEBUG_MODULE_H
#define UMICOM_DEBUG_MODULE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_MODULE_CAPACITY 2048U
#define UMI_DEBUG_MODULE_API_VERSION 1U

typedef struct UmiDebugModuleSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char name[256];
    char path[1024];
    char version[128];
    char symbol_status[256];
    int optimised;
    uint64_t revision;
} UmiDebugModuleSnapshot;

typedef struct UmiDebugModuleRegistry UmiDebugModuleRegistry;

UmiStatus umi_debug_module_registry_create(UmiDebugModuleRegistry **out_registry);
void umi_debug_module_registry_destroy(UmiDebugModuleRegistry *registry);
UmiStatus umi_debug_module_registry_upsert(UmiDebugModuleRegistry *registry, const UmiDebugModuleSnapshot *item);
UmiStatus umi_debug_module_registry_remove(UmiDebugModuleRegistry *registry, const char *id);
UmiStatus umi_debug_module_registry_find(const UmiDebugModuleRegistry *registry, const char *id, UmiDebugModuleSnapshot *out_item);
UmiStatus umi_debug_module_registry_at(const UmiDebugModuleRegistry *registry, size_t index, UmiDebugModuleSnapshot *out_item);
size_t umi_debug_module_registry_count(const UmiDebugModuleRegistry *registry);
uint64_t umi_debug_module_registry_revision(const UmiDebugModuleRegistry *registry);
void umi_debug_module_registry_clear(UmiDebugModuleRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
