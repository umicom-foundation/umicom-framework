/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/source.h
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
#ifndef UMICOM_DEBUG_SOURCE_H
#define UMICOM_DEBUG_SOURCE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_SOURCE_CAPACITY 2048U
#define UMI_DEBUG_SOURCE_API_VERSION 1U

typedef struct UmiDebugSourceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char uri[1024];
    char name[256];
    uint64_t source_reference;
    int available;
    uint64_t revision;
} UmiDebugSourceSnapshot;

typedef struct UmiDebugSourceRegistry UmiDebugSourceRegistry;

UmiStatus umi_debug_source_registry_create(UmiDebugSourceRegistry **out_registry);
void umi_debug_source_registry_destroy(UmiDebugSourceRegistry *registry);
UmiStatus umi_debug_source_registry_upsert(UmiDebugSourceRegistry *registry, const UmiDebugSourceSnapshot *item);
UmiStatus umi_debug_source_registry_remove(UmiDebugSourceRegistry *registry, const char *id);
UmiStatus umi_debug_source_registry_find(const UmiDebugSourceRegistry *registry, const char *id, UmiDebugSourceSnapshot *out_item);
UmiStatus umi_debug_source_registry_at(const UmiDebugSourceRegistry *registry, size_t index, UmiDebugSourceSnapshot *out_item);
size_t umi_debug_source_registry_count(const UmiDebugSourceRegistry *registry);
uint64_t umi_debug_source_registry_revision(const UmiDebugSourceRegistry *registry);
void umi_debug_source_registry_clear(UmiDebugSourceRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
