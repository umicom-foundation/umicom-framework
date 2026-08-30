/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/exception.h
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
#ifndef UMICOM_DEBUG_EXCEPTION_H
#define UMICOM_DEBUG_EXCEPTION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_EXCEPTION_CAPACITY 2048U
#define UMI_DEBUG_EXCEPTION_API_VERSION 1U

typedef struct UmiDebugExceptionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char type[256];
    char message[1024];
    char break_mode[64];
    int caught;
    uint64_t revision;
} UmiDebugExceptionSnapshot;

typedef struct UmiDebugExceptionRegistry UmiDebugExceptionRegistry;

UmiStatus umi_debug_exception_registry_create(UmiDebugExceptionRegistry **out_registry);
void umi_debug_exception_registry_destroy(UmiDebugExceptionRegistry *registry);
UmiStatus umi_debug_exception_registry_upsert(UmiDebugExceptionRegistry *registry, const UmiDebugExceptionSnapshot *item);
UmiStatus umi_debug_exception_registry_remove(UmiDebugExceptionRegistry *registry, const char *id);
UmiStatus umi_debug_exception_registry_find(const UmiDebugExceptionRegistry *registry, const char *id, UmiDebugExceptionSnapshot *out_item);
UmiStatus umi_debug_exception_registry_at(const UmiDebugExceptionRegistry *registry, size_t index, UmiDebugExceptionSnapshot *out_item);
size_t umi_debug_exception_registry_count(const UmiDebugExceptionRegistry *registry);
uint64_t umi_debug_exception_registry_revision(const UmiDebugExceptionRegistry *registry);
void umi_debug_exception_registry_clear(UmiDebugExceptionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
