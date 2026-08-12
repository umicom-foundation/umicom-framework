/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/console_entry.h
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

typedef struct UmiDebugConsoleEntryRegistry UmiDebugConsoleEntryRegistry;

UmiStatus umi_debug_console_entry_registry_create(UmiDebugConsoleEntryRegistry **out_registry);
void umi_debug_console_entry_registry_destroy(UmiDebugConsoleEntryRegistry *registry);
UmiStatus umi_debug_console_entry_registry_upsert(UmiDebugConsoleEntryRegistry *registry, const UmiDebugConsoleEntrySnapshot *item);
UmiStatus umi_debug_console_entry_registry_remove(UmiDebugConsoleEntryRegistry *registry, const char *id);
UmiStatus umi_debug_console_entry_registry_find(const UmiDebugConsoleEntryRegistry *registry, const char *id, UmiDebugConsoleEntrySnapshot *out_item);
UmiStatus umi_debug_console_entry_registry_at(const UmiDebugConsoleEntryRegistry *registry, size_t index, UmiDebugConsoleEntrySnapshot *out_item);
size_t umi_debug_console_entry_registry_count(const UmiDebugConsoleEntryRegistry *registry);
uint64_t umi_debug_console_entry_registry_revision(const UmiDebugConsoleEntryRegistry *registry);
void umi_debug_console_entry_registry_clear(UmiDebugConsoleEntryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
