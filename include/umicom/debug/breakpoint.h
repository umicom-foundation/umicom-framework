/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/breakpoint.h
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
#ifndef UMICOM_DEBUG_BREAKPOINT_H
#define UMICOM_DEBUG_BREAKPOINT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_BREAKPOINT_CAPACITY 2048U
#define UMI_DEBUG_BREAKPOINT_API_VERSION 1U

typedef struct UmiDebugBreakpointSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char uri[1024];
    char condition[512];
    char log_message[512];
    uint32_t line;
    uint32_t column;
    int enabled;
    int verified;
    uint64_t revision;
} UmiDebugBreakpointSnapshot;

typedef struct UmiDebugBreakpointRegistry UmiDebugBreakpointRegistry;

UmiStatus umi_debug_breakpoint_registry_create(UmiDebugBreakpointRegistry **out_registry);
void umi_debug_breakpoint_registry_destroy(UmiDebugBreakpointRegistry *registry);
UmiStatus umi_debug_breakpoint_registry_upsert(UmiDebugBreakpointRegistry *registry, const UmiDebugBreakpointSnapshot *item);
UmiStatus umi_debug_breakpoint_registry_remove(UmiDebugBreakpointRegistry *registry, const char *id);
UmiStatus umi_debug_breakpoint_registry_find(const UmiDebugBreakpointRegistry *registry, const char *id, UmiDebugBreakpointSnapshot *out_item);
UmiStatus umi_debug_breakpoint_registry_at(const UmiDebugBreakpointRegistry *registry, size_t index, UmiDebugBreakpointSnapshot *out_item);
size_t umi_debug_breakpoint_registry_count(const UmiDebugBreakpointRegistry *registry);
uint64_t umi_debug_breakpoint_registry_revision(const UmiDebugBreakpointRegistry *registry);
void umi_debug_breakpoint_registry_clear(UmiDebugBreakpointRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
