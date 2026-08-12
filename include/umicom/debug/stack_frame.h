/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/stack_frame.h
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

typedef struct UmiDebugStackFrameRegistry UmiDebugStackFrameRegistry;

UmiStatus umi_debug_stack_frame_registry_create(UmiDebugStackFrameRegistry **out_registry);
void umi_debug_stack_frame_registry_destroy(UmiDebugStackFrameRegistry *registry);
UmiStatus umi_debug_stack_frame_registry_upsert(UmiDebugStackFrameRegistry *registry, const UmiDebugStackFrameSnapshot *item);
UmiStatus umi_debug_stack_frame_registry_remove(UmiDebugStackFrameRegistry *registry, const char *id);
UmiStatus umi_debug_stack_frame_registry_find(const UmiDebugStackFrameRegistry *registry, const char *id, UmiDebugStackFrameSnapshot *out_item);
UmiStatus umi_debug_stack_frame_registry_at(const UmiDebugStackFrameRegistry *registry, size_t index, UmiDebugStackFrameSnapshot *out_item);
size_t umi_debug_stack_frame_registry_count(const UmiDebugStackFrameRegistry *registry);
uint64_t umi_debug_stack_frame_registry_revision(const UmiDebugStackFrameRegistry *registry);
void umi_debug_stack_frame_registry_clear(UmiDebugStackFrameRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
