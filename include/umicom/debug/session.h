/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/session.h
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
#ifndef UMICOM_DEBUG_SESSION_H
#define UMICOM_DEBUG_SESSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_SESSION_CAPACITY 2048U
#define UMI_DEBUG_SESSION_API_VERSION 1U

typedef struct UmiDebugSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char configuration_id[128];
    char adapter[128];
    char state_text[128];
    uint64_t started_at;
    int state;
    int attached;
    int supports_restart;
    uint64_t revision;
} UmiDebugSessionSnapshot;

typedef struct UmiDebugSessionRegistry UmiDebugSessionRegistry;

UmiStatus umi_debug_session_registry_create(UmiDebugSessionRegistry **out_registry);
void umi_debug_session_registry_destroy(UmiDebugSessionRegistry *registry);
UmiStatus umi_debug_session_registry_upsert(UmiDebugSessionRegistry *registry, const UmiDebugSessionSnapshot *item);
UmiStatus umi_debug_session_registry_remove(UmiDebugSessionRegistry *registry, const char *id);
UmiStatus umi_debug_session_registry_find(const UmiDebugSessionRegistry *registry, const char *id, UmiDebugSessionSnapshot *out_item);
UmiStatus umi_debug_session_registry_at(const UmiDebugSessionRegistry *registry, size_t index, UmiDebugSessionSnapshot *out_item);
size_t umi_debug_session_registry_count(const UmiDebugSessionRegistry *registry);
uint64_t umi_debug_session_registry_revision(const UmiDebugSessionRegistry *registry);
void umi_debug_session_registry_clear(UmiDebugSessionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
