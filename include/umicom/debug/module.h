/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/module.h
 *
 * PURPOSE:
 *   Define a DAP-friendly but adapter-neutral debugger record for native and future Umicom runtimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the debug module snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the debug module registry data shared with callers of this public contract.
 */
typedef struct UmiDebugModuleRegistry UmiDebugModuleRegistry;

/**
 * Initialise debug module registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_debug_module_registry_create(UmiDebugModuleRegistry **out_registry);
/**
 * Release or reset state held by debug module registry so the same storage can be reused
 * safely.
 */
void umi_debug_module_registry_destroy(UmiDebugModuleRegistry *registry);
/**
 * Provide the debug module registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_module_registry_upsert(UmiDebugModuleRegistry *registry, const UmiDebugModuleSnapshot *item);
/**
 * Remove debug module registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_module_registry_remove(UmiDebugModuleRegistry *registry, const char *id);
/**
 * Find debug module registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_module_registry_find(const UmiDebugModuleRegistry *registry, const char *id, UmiDebugModuleSnapshot *out_item);
/**
 * Find debug module registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_module_registry_at(const UmiDebugModuleRegistry *registry, size_t index, UmiDebugModuleSnapshot *out_item);
/**
 * Return the number of records represented by debug module registry without changing their
 * state.
 */
size_t umi_debug_module_registry_count(const UmiDebugModuleRegistry *registry);
/**
 * Provide the debug module registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_debug_module_registry_revision(const UmiDebugModuleRegistry *registry);
/**
 * Release or reset state held by debug module registry so the same storage can be reused
 * safely.
 */
void umi_debug_module_registry_clear(UmiDebugModuleRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
