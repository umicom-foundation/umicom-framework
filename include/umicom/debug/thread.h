/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/thread.h
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

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_DEBUG_THREAD_H
#define UMICOM_DEBUG_THREAD_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_THREAD_CAPACITY 2048U
#define UMI_DEBUG_THREAD_API_VERSION 1U

typedef struct UmiDebugThreadSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char name[256];
    char detail[512];
    uint64_t native_id;
    int stopped;
    int current;
    uint64_t revision;
} UmiDebugThreadSnapshot;

typedef struct UmiDebugThreadRegistry UmiDebugThreadRegistry;

UmiStatus umi_debug_thread_registry_create(UmiDebugThreadRegistry **out_registry);
void umi_debug_thread_registry_destroy(UmiDebugThreadRegistry *registry);
UmiStatus umi_debug_thread_registry_upsert(UmiDebugThreadRegistry *registry, const UmiDebugThreadSnapshot *item);
UmiStatus umi_debug_thread_registry_remove(UmiDebugThreadRegistry *registry, const char *id);
UmiStatus umi_debug_thread_registry_find(const UmiDebugThreadRegistry *registry, const char *id, UmiDebugThreadSnapshot *out_item);
UmiStatus umi_debug_thread_registry_at(const UmiDebugThreadRegistry *registry, size_t index, UmiDebugThreadSnapshot *out_item);
size_t umi_debug_thread_registry_count(const UmiDebugThreadRegistry *registry);
uint64_t umi_debug_thread_registry_revision(const UmiDebugThreadRegistry *registry);
void umi_debug_thread_registry_clear(UmiDebugThreadRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
