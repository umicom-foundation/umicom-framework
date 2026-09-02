/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/breakpoint.h
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

/**
 * Represent the debug breakpoint snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the debug breakpoint registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugBreakpointRegistry UmiDebugBreakpointRegistry;

/**
 * Initialise debug breakpoint registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_breakpoint_registry_create(UmiDebugBreakpointRegistry **out_registry);
/**
 * Release or reset state held by debug breakpoint registry so the same storage can be
 * reused safely.
 */
void umi_debug_breakpoint_registry_destroy(UmiDebugBreakpointRegistry *registry);
/**
 * Provide the debug breakpoint registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_breakpoint_registry_upsert(UmiDebugBreakpointRegistry *registry, const UmiDebugBreakpointSnapshot *item);
/**
 * Remove debug breakpoint registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_breakpoint_registry_remove(UmiDebugBreakpointRegistry *registry, const char *id);
/**
 * Find debug breakpoint registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_breakpoint_registry_find(const UmiDebugBreakpointRegistry *registry, const char *id, UmiDebugBreakpointSnapshot *out_item);
/**
 * Find debug breakpoint registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_breakpoint_registry_at(const UmiDebugBreakpointRegistry *registry, size_t index, UmiDebugBreakpointSnapshot *out_item);
/**
 * Return the number of records represented by debug breakpoint registry without changing
 * their state.
 */
size_t umi_debug_breakpoint_registry_count(const UmiDebugBreakpointRegistry *registry);
/**
 * Provide the debug breakpoint registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_debug_breakpoint_registry_revision(const UmiDebugBreakpointRegistry *registry);
/**
 * Release or reset state held by debug breakpoint registry so the same storage can be
 * reused safely.
 */
void umi_debug_breakpoint_registry_clear(UmiDebugBreakpointRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
