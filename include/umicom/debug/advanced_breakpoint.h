/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/advanced_breakpoint.h
 *
 * PURPOSE:
 *   Model function, data, instruction and exception breakpoints independently
 *   of any debugger adapter, graphical toolkit or application shell.
 *
 * ARCHITECTURE:
 *   Source breakpoints remain in the established UmiDebugBreakpointRegistry.
 *   This module extends that foundation with breakpoint forms whose identity is
 *   supplied by a debugger or runtime rather than by a source line alone.
 *   Records are copied by value into bounded Framework-owned storage so Studio,
 *   a headless test runner and future Umicom applications can share one model.
 *
 * OWNERSHIP AND THREADING:
 *   The registry owns copied records. Returned records are caller-owned value
 *   snapshots. External synchronisation is required when a registry is mutated
 *   from more than one thread.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_ADVANCED_BREAKPOINT_H
#define UMICOM_DEBUG_ADVANCED_BREAKPOINT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_ADVANCED_BREAKPOINT_API_VERSION 1U
#define UMI_DEBUG_ADVANCED_BREAKPOINT_DEFAULT_CAPACITY 64U
#define UMI_DEBUG_ADVANCED_BREAKPOINT_MAXIMUM_CAPACITY 4096U
#define UMI_DEBUG_ADVANCED_BREAKPOINT_ID_CAPACITY 128U
#define UMI_DEBUG_ADVANCED_BREAKPOINT_SESSION_ID_CAPACITY 128U
#define UMI_DEBUG_ADVANCED_BREAKPOINT_NAME_CAPACITY 512U
#define UMI_DEBUG_ADVANCED_BREAKPOINT_REFERENCE_CAPACITY 512U
#define UMI_DEBUG_ADVANCED_BREAKPOINT_CONDITION_CAPACITY 512U
#define UMI_DEBUG_ADVANCED_BREAKPOINT_HIT_CONDITION_CAPACITY 128U
#define UMI_DEBUG_ADVANCED_BREAKPOINT_LOG_MESSAGE_CAPACITY 512U
#define UMI_DEBUG_ADVANCED_BREAKPOINT_MESSAGE_CAPACITY 512U

/**
 * Breakpoint families that cannot be represented solely by a source URI and
 * line number. Values are stable because they may be persisted by applications.
 */
typedef enum UmiDebugAdvancedBreakpointKind {
    UMI_DEBUG_ADVANCED_BREAKPOINT_FUNCTION = 1,
    UMI_DEBUG_ADVANCED_BREAKPOINT_DATA = 2,
    UMI_DEBUG_ADVANCED_BREAKPOINT_INSTRUCTION = 3,
    UMI_DEBUG_ADVANCED_BREAKPOINT_EXCEPTION = 4
} UmiDebugAdvancedBreakpointKind;

/** Access requested for a data breakpoint. */
typedef enum UmiDebugDataBreakpointAccess {
    UMI_DEBUG_DATA_BREAKPOINT_ACCESS_READ = 1,
    UMI_DEBUG_DATA_BREAKPOINT_ACCESS_WRITE = 2,
    UMI_DEBUG_DATA_BREAKPOINT_ACCESS_READ_WRITE = 3
} UmiDebugDataBreakpointAccess;

/**
 * A provider-neutral advanced breakpoint.
 *
 * Field use by kind:
 *   FUNCTION    name, condition, hit_condition
 *   DATA        name, reference, access, condition, hit_condition
 *   INSTRUCTION reference, address, instruction_offset
 *   EXCEPTION   name/filter in reference, condition where supported
 */
typedef struct UmiDebugAdvancedBreakpoint {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DEBUG_ADVANCED_BREAKPOINT_ID_CAPACITY];
    char session_id[UMI_DEBUG_ADVANCED_BREAKPOINT_SESSION_ID_CAPACITY];
    UmiDebugAdvancedBreakpointKind kind;
    char name[UMI_DEBUG_ADVANCED_BREAKPOINT_NAME_CAPACITY];
    char reference[UMI_DEBUG_ADVANCED_BREAKPOINT_REFERENCE_CAPACITY];
    char condition[UMI_DEBUG_ADVANCED_BREAKPOINT_CONDITION_CAPACITY];
    char hit_condition[UMI_DEBUG_ADVANCED_BREAKPOINT_HIT_CONDITION_CAPACITY];
    char log_message[UMI_DEBUG_ADVANCED_BREAKPOINT_LOG_MESSAGE_CAPACITY];
    char message[UMI_DEBUG_ADVANCED_BREAKPOINT_MESSAGE_CAPACITY];
    uint64_t address;
    int64_t instruction_offset;
    UmiDebugDataBreakpointAccess access;
    uint64_t sequence;
    uint64_t revision;
    int enabled;
    int verified;
    int temporary;
    int stop_on_hit;
} UmiDebugAdvancedBreakpoint;

typedef struct UmiDebugAdvancedBreakpointFilter {
    uint32_t struct_size;
    uint32_t api_version;
    const char *session_id;
    const char *text;
    UmiDebugAdvancedBreakpointKind kind;
    int enabled_only;
    int verified_only;
} UmiDebugAdvancedBreakpointFilter;

typedef struct UmiDebugAdvancedBreakpointSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t breakpoint_count;
    size_t function_count;
    size_t data_count;
    size_t instruction_count;
    size_t exception_count;
    size_t enabled_count;
    size_t verified_count;
    uint64_t revision;
} UmiDebugAdvancedBreakpointSnapshot;

typedef struct UmiDebugAdvancedBreakpointRegistry
    UmiDebugAdvancedBreakpointRegistry;

UmiStatus umi_debug_advanced_breakpoint_registry_create(
    size_t initial_capacity,
    UmiDebugAdvancedBreakpointRegistry **out_registry);
void umi_debug_advanced_breakpoint_registry_destroy(
    UmiDebugAdvancedBreakpointRegistry *registry);
UmiStatus umi_debug_advanced_breakpoint_registry_clear(
    UmiDebugAdvancedBreakpointRegistry *registry);
UmiStatus umi_debug_advanced_breakpoint_registry_upsert(
    UmiDebugAdvancedBreakpointRegistry *registry,
    const UmiDebugAdvancedBreakpoint *breakpoint);
UmiStatus umi_debug_advanced_breakpoint_registry_remove(
    UmiDebugAdvancedBreakpointRegistry *registry,
    const char *breakpoint_id);
UmiStatus umi_debug_advanced_breakpoint_registry_remove_session(
    UmiDebugAdvancedBreakpointRegistry *registry,
    const char *session_id,
    size_t *out_removed_count);
UmiStatus umi_debug_advanced_breakpoint_registry_set_enabled(
    UmiDebugAdvancedBreakpointRegistry *registry,
    const char *breakpoint_id,
    int enabled);
UmiStatus umi_debug_advanced_breakpoint_registry_find(
    const UmiDebugAdvancedBreakpointRegistry *registry,
    const char *breakpoint_id,
    UmiDebugAdvancedBreakpoint *out_breakpoint);
UmiStatus umi_debug_advanced_breakpoint_registry_at(
    const UmiDebugAdvancedBreakpointRegistry *registry,
    size_t index,
    UmiDebugAdvancedBreakpoint *out_breakpoint);
UmiStatus umi_debug_advanced_breakpoint_registry_query(
    const UmiDebugAdvancedBreakpointRegistry *registry,
    const UmiDebugAdvancedBreakpointFilter *filter,
    UmiDebugAdvancedBreakpoint *out_breakpoints,
    size_t breakpoint_capacity,
    size_t *out_breakpoint_count);
UmiStatus umi_debug_advanced_breakpoint_registry_snapshot(
    const UmiDebugAdvancedBreakpointRegistry *registry,
    UmiDebugAdvancedBreakpointSnapshot *out_snapshot);
size_t umi_debug_advanced_breakpoint_registry_count(
    const UmiDebugAdvancedBreakpointRegistry *registry);
uint64_t umi_debug_advanced_breakpoint_registry_revision(
    const UmiDebugAdvancedBreakpointRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_ADVANCED_BREAKPOINT_H */
