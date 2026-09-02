/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/console_entry.h
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
#ifndef UMICOM_DEBUG_CONSOLE_ENTRY_H
#define UMICOM_DEBUG_CONSOLE_ENTRY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_CONSOLE_ENTRY_CAPACITY 2048U
#define UMI_DEBUG_CONSOLE_ENTRY_API_VERSION 1U

/**
 * Represent the debug console entry snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugConsoleEntrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char category[64];
    char text[2048];
    uint64_t timestamp;
    int severity;
    uint64_t revision;
} UmiDebugConsoleEntrySnapshot;

/**
 * Represent the debug console entry registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugConsoleEntryRegistry UmiDebugConsoleEntryRegistry;

/**
 * Initialise debug console entry registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_console_entry_registry_create(UmiDebugConsoleEntryRegistry **out_registry);
/**
 * Release or reset state held by debug console entry registry so the same storage can be
 * reused safely.
 */
void umi_debug_console_entry_registry_destroy(UmiDebugConsoleEntryRegistry *registry);
/**
 * Provide the debug console entry registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_console_entry_registry_upsert(UmiDebugConsoleEntryRegistry *registry, const UmiDebugConsoleEntrySnapshot *item);
/**
 * Remove debug console entry registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_console_entry_registry_remove(UmiDebugConsoleEntryRegistry *registry, const char *id);
/**
 * Find debug console entry registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_debug_console_entry_registry_find(const UmiDebugConsoleEntryRegistry *registry, const char *id, UmiDebugConsoleEntrySnapshot *out_item);
/**
 * Find debug console entry registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_debug_console_entry_registry_at(const UmiDebugConsoleEntryRegistry *registry, size_t index, UmiDebugConsoleEntrySnapshot *out_item);
/**
 * Return the number of records represented by debug console entry registry without
 * changing their state.
 */
size_t umi_debug_console_entry_registry_count(const UmiDebugConsoleEntryRegistry *registry);
/**
 * Provide the debug console entry registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_debug_console_entry_registry_revision(const UmiDebugConsoleEntryRegistry *registry);
/**
 * Release or reset state held by debug console entry registry so the same storage can be
 * reused safely.
 */
void umi_debug_console_entry_registry_clear(UmiDebugConsoleEntryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
