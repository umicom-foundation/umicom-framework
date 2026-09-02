/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/session.h
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

/**
 * Represent the debug session snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the debug session registry data shared with callers of this public contract.
 */
typedef struct UmiDebugSessionRegistry UmiDebugSessionRegistry;

/**
 * Initialise debug session registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_session_registry_create(UmiDebugSessionRegistry **out_registry);
/**
 * Release or reset state held by debug session registry so the same storage can be reused
 * safely.
 */
void umi_debug_session_registry_destroy(UmiDebugSessionRegistry *registry);
/**
 * Provide the debug session registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_session_registry_upsert(UmiDebugSessionRegistry *registry, const UmiDebugSessionSnapshot *item);
/**
 * Remove debug session registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_session_registry_remove(UmiDebugSessionRegistry *registry, const char *id);
/**
 * Find debug session registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_session_registry_find(const UmiDebugSessionRegistry *registry, const char *id, UmiDebugSessionSnapshot *out_item);
/**
 * Find debug session registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_session_registry_at(const UmiDebugSessionRegistry *registry, size_t index, UmiDebugSessionSnapshot *out_item);
/**
 * Return the number of records represented by debug session registry without changing
 * their state.
 */
size_t umi_debug_session_registry_count(const UmiDebugSessionRegistry *registry);
/**
 * Provide the debug session registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_debug_session_registry_revision(const UmiDebugSessionRegistry *registry);
/**
 * Release or reset state held by debug session registry so the same storage can be reused
 * safely.
 */
void umi_debug_session_registry_clear(UmiDebugSessionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
