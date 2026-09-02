/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/stack_frame.h
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
#ifndef UMICOM_DEBUG_STACK_FRAME_H
#define UMICOM_DEBUG_STACK_FRAME_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_STACK_FRAME_CAPACITY 2048U
#define UMI_DEBUG_STACK_FRAME_API_VERSION 1U

/**
 * Represent the debug stack frame snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugStackFrameSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char thread_id[128];
    char name[256];
    char source_uri[1024];
    uint32_t line;
    uint32_t column;
    int32_t order;
    uint64_t revision;
} UmiDebugStackFrameSnapshot;

/**
 * Represent the debug stack frame registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugStackFrameRegistry UmiDebugStackFrameRegistry;

/**
 * Initialise debug stack frame registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_stack_frame_registry_create(UmiDebugStackFrameRegistry **out_registry);
/**
 * Release or reset state held by debug stack frame registry so the same storage can be
 * reused safely.
 */
void umi_debug_stack_frame_registry_destroy(UmiDebugStackFrameRegistry *registry);
/**
 * Provide the debug stack frame registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_stack_frame_registry_upsert(UmiDebugStackFrameRegistry *registry, const UmiDebugStackFrameSnapshot *item);
/**
 * Remove debug stack frame registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_stack_frame_registry_remove(UmiDebugStackFrameRegistry *registry, const char *id);
/**
 * Find debug stack frame registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_stack_frame_registry_find(const UmiDebugStackFrameRegistry *registry, const char *id, UmiDebugStackFrameSnapshot *out_item);
/**
 * Find debug stack frame registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_stack_frame_registry_at(const UmiDebugStackFrameRegistry *registry, size_t index, UmiDebugStackFrameSnapshot *out_item);
/**
 * Return the number of records represented by debug stack frame registry without changing
 * their state.
 */
size_t umi_debug_stack_frame_registry_count(const UmiDebugStackFrameRegistry *registry);
/**
 * Provide the debug stack frame registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_debug_stack_frame_registry_revision(const UmiDebugStackFrameRegistry *registry);
/**
 * Release or reset state held by debug stack frame registry so the same storage can be
 * reused safely.
 */
void umi_debug_stack_frame_registry_clear(UmiDebugStackFrameRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
