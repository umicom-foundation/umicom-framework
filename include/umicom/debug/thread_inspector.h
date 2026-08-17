/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/thread_inspector.h
 *
 * PURPOSE:
 *   Build a deterministic, toolkit-neutral Threads / Call Stack / Scopes /
 *   Variables inspection model from the established debugger registries.
 *
 * ARCHITECTURE:
 *   The inspector does not own or mutate the source registries. Refresh copies
 *   relevant records into one coherent snapshot, applies session and text
 *   filtering, preserves valid selections and derives sensible fallback
 *   selections. Frontends therefore render ordinary value snapshots rather
 *   than traversing debugger internals directly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_THREAD_INSPECTOR_H
#define UMICOM_DEBUG_THREAD_INSPECTOR_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/debug/scope.h"
#include "umicom/debug/stack_frame.h"
#include "umicom/debug/thread.h"
#include "umicom/debug/variable.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_THREAD_INSPECTOR_API_VERSION 1U
#define UMI_DEBUG_THREAD_INSPECTOR_FILTER_CAPACITY 256U
#define UMI_DEBUG_THREAD_INSPECTOR_SESSION_ID_CAPACITY 128U
#define UMI_DEBUG_THREAD_INSPECTOR_SELECTION_ID_CAPACITY 128U

typedef struct UmiDebugThreadInspectorFilter {
    uint32_t struct_size;
    uint32_t api_version;
    char session_id[UMI_DEBUG_THREAD_INSPECTOR_SESSION_ID_CAPACITY];
    char text[UMI_DEBUG_THREAD_INSPECTOR_FILTER_CAPACITY];
    int stopped_only;
    int current_only;
    int include_expensive_scopes;
} UmiDebugThreadInspectorFilter;

typedef struct UmiDebugThreadInspectorSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t thread_count;
    size_t stopped_thread_count;
    size_t frame_count;
    size_t scope_count;
    size_t variable_count;
    char selected_thread_id[UMI_DEBUG_THREAD_INSPECTOR_SELECTION_ID_CAPACITY];
    char selected_frame_id[UMI_DEBUG_THREAD_INSPECTOR_SELECTION_ID_CAPACITY];
    char selected_scope_id[UMI_DEBUG_THREAD_INSPECTOR_SELECTION_ID_CAPACITY];
    uint64_t thread_registry_revision;
    uint64_t frame_registry_revision;
    uint64_t scope_registry_revision;
    uint64_t variable_registry_revision;
    uint64_t revision;
    int follow_current_thread;
    int has_thread_selection;
    int has_frame_selection;
    int has_scope_selection;
} UmiDebugThreadInspectorSnapshot;

typedef struct UmiDebugThreadInspector UmiDebugThreadInspector;

UmiStatus umi_debug_thread_inspector_create(
    UmiDebugThreadInspector **out_inspector);
void umi_debug_thread_inspector_destroy(UmiDebugThreadInspector *inspector);
UmiStatus umi_debug_thread_inspector_clear(
    UmiDebugThreadInspector *inspector);
UmiStatus umi_debug_thread_inspector_set_filter(
    UmiDebugThreadInspector *inspector,
    const UmiDebugThreadInspectorFilter *filter);
UmiStatus umi_debug_thread_inspector_set_follow_current_thread(
    UmiDebugThreadInspector *inspector,
    int follow_current_thread);
UmiStatus umi_debug_thread_inspector_refresh(
    UmiDebugThreadInspector *inspector,
    const UmiDebugThreadRegistry *threads,
    const UmiDebugStackFrameRegistry *frames,
    const UmiDebugScopeRegistry *scopes,
    const UmiDebugVariableRegistry *variables);
UmiStatus umi_debug_thread_inspector_select_thread(
    UmiDebugThreadInspector *inspector,
    const char *thread_id);
UmiStatus umi_debug_thread_inspector_select_frame(
    UmiDebugThreadInspector *inspector,
    const char *frame_id);
UmiStatus umi_debug_thread_inspector_select_scope(
    UmiDebugThreadInspector *inspector,
    const char *scope_id);
UmiStatus umi_debug_thread_inspector_thread_at(
    const UmiDebugThreadInspector *inspector,
    size_t index,
    UmiDebugThreadSnapshot *out_thread);
UmiStatus umi_debug_thread_inspector_frame_at(
    const UmiDebugThreadInspector *inspector,
    size_t index,
    UmiDebugStackFrameSnapshot *out_frame);
UmiStatus umi_debug_thread_inspector_scope_at(
    const UmiDebugThreadInspector *inspector,
    size_t index,
    UmiDebugScopeSnapshot *out_scope);
UmiStatus umi_debug_thread_inspector_variable_at(
    const UmiDebugThreadInspector *inspector,
    size_t index,
    UmiDebugVariableSnapshot *out_variable);
UmiStatus umi_debug_thread_inspector_selected_thread(
    const UmiDebugThreadInspector *inspector,
    UmiDebugThreadSnapshot *out_thread);
UmiStatus umi_debug_thread_inspector_selected_frame(
    const UmiDebugThreadInspector *inspector,
    UmiDebugStackFrameSnapshot *out_frame);
UmiStatus umi_debug_thread_inspector_selected_scope(
    const UmiDebugThreadInspector *inspector,
    UmiDebugScopeSnapshot *out_scope);
UmiStatus umi_debug_thread_inspector_snapshot(
    const UmiDebugThreadInspector *inspector,
    UmiDebugThreadInspectorSnapshot *out_snapshot);
size_t umi_debug_thread_inspector_thread_count(
    const UmiDebugThreadInspector *inspector);
size_t umi_debug_thread_inspector_frame_count(
    const UmiDebugThreadInspector *inspector);
size_t umi_debug_thread_inspector_scope_count(
    const UmiDebugThreadInspector *inspector);
size_t umi_debug_thread_inspector_variable_count(
    const UmiDebugThreadInspector *inspector);
uint64_t umi_debug_thread_inspector_revision(
    const UmiDebugThreadInspector *inspector);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_THREAD_INSPECTOR_H */
