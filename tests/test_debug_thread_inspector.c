/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_thread_inspector.c
 *
 * PURPOSE:
 *   Implement the test debug thread inspector behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework thread inspector tests.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/debug/thread_inspector.h"

static UmiDebugThreadSnapshot make_thread(
    const char *id,
    const char *session_id,
    const char *name,
    uint64_t native_id,
    int stopped,
    int current)
{
    UmiDebugThreadSnapshot item;
    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_DEBUG_THREAD_API_VERSION;
    (void)snprintf(item.id, sizeof(item.id), "%s", id);
    (void)snprintf(item.session_id, sizeof(item.session_id), "%s", session_id);
    (void)snprintf(item.name, sizeof(item.name), "%s", name);
    item.native_id = native_id;
    item.stopped = stopped;
    item.current = current;
    return item;
}

static UmiDebugStackFrameSnapshot make_frame(
    const char *id,
    const char *thread_id,
    int32_t order)
{
    UmiDebugStackFrameSnapshot item;
    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_DEBUG_STACK_FRAME_API_VERSION;
    (void)snprintf(item.id, sizeof(item.id), "%s", id);
    (void)snprintf(item.thread_id, sizeof(item.thread_id), "%s", thread_id);
    (void)snprintf(item.name, sizeof(item.name), "%s", id);
    (void)snprintf(item.source_uri, sizeof(item.source_uri), "%s", "file:///main.c");
    item.order = order;
    return item;
}

static UmiDebugScopeSnapshot make_scope(
    const char *id,
    const char *frame_id,
    int32_t order)
{
    UmiDebugScopeSnapshot item;
    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_DEBUG_SCOPE_API_VERSION;
    (void)snprintf(item.id, sizeof(item.id), "%s", id);
    (void)snprintf(item.frame_id, sizeof(item.frame_id), "%s", frame_id);
    (void)snprintf(item.name, sizeof(item.name), "%s", id);
    item.order = order;
    return item;
}

static UmiDebugVariableSnapshot make_variable(
    const char *id,
    const char *scope_id,
    const char *name)
{
    UmiDebugVariableSnapshot item;
    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_DEBUG_VARIABLE_API_VERSION;
    (void)snprintf(item.id, sizeof(item.id), "%s", id);
    (void)snprintf(item.scope_id, sizeof(item.scope_id), "%s", scope_id);
    (void)snprintf(item.name, sizeof(item.name), "%s", name);
    (void)snprintf(item.value, sizeof(item.value), "%s", "42");
    return item;
}

int main(void)
{
    UmiDebugThreadRegistry *threads = NULL;
    UmiDebugStackFrameRegistry *frames = NULL;
    UmiDebugScopeRegistry *scopes = NULL;
    UmiDebugVariableRegistry *variables = NULL;
    UmiDebugThreadInspector *inspector = NULL;
    UmiDebugThreadInspectorFilter filter;
    UmiDebugThreadInspectorSnapshot snapshot;
    UmiDebugThreadSnapshot thread;
    UmiDebugStackFrameSnapshot frame;
    UmiDebugScopeSnapshot scope;
    UmiDebugVariableSnapshot variable;

    assert(umi_debug_thread_registry_create(&threads) == UMI_STATUS_OK);
    assert(umi_debug_stack_frame_registry_create(&frames) == UMI_STATUS_OK);
    assert(umi_debug_scope_registry_create(&scopes) == UMI_STATUS_OK);
    assert(umi_debug_variable_registry_create(&variables) == UMI_STATUS_OK);
    assert(umi_debug_thread_inspector_create(&inspector) == UMI_STATUS_OK);

    thread = make_thread("thread-2", "session-1", "worker", 2U, 1, 0);
    assert(umi_debug_thread_registry_upsert(threads, &thread) == UMI_STATUS_OK);
    thread = make_thread("thread-1", "session-1", "main", 1U, 1, 1);
    assert(umi_debug_thread_registry_upsert(threads, &thread) == UMI_STATUS_OK);
    thread = make_thread("thread-other", "session-2", "other", 3U, 0, 0);
    assert(umi_debug_thread_registry_upsert(threads, &thread) == UMI_STATUS_OK);

    frame = make_frame("frame-1b", "thread-1", 1);
    assert(umi_debug_stack_frame_registry_upsert(frames, &frame) == UMI_STATUS_OK);
    frame = make_frame("frame-1a", "thread-1", 0);
    assert(umi_debug_stack_frame_registry_upsert(frames, &frame) == UMI_STATUS_OK);
    frame = make_frame("frame-2a", "thread-2", 0);
    assert(umi_debug_stack_frame_registry_upsert(frames, &frame) == UMI_STATUS_OK);

    scope = make_scope("locals-1", "frame-1a", 0);
    assert(umi_debug_scope_registry_upsert(scopes, &scope) == UMI_STATUS_OK);
    scope = make_scope("globals-1", "frame-1a", 1);
    assert(umi_debug_scope_registry_upsert(scopes, &scope) == UMI_STATUS_OK);
    scope = make_scope("locals-2", "frame-2a", 0);
    assert(umi_debug_scope_registry_upsert(scopes, &scope) == UMI_STATUS_OK);

    variable = make_variable("var-b", "locals-1", "beta");
    assert(umi_debug_variable_registry_upsert(variables, &variable) ==
           UMI_STATUS_OK);
    variable = make_variable("var-a", "locals-1", "alpha");
    variable.changed = 1;
    assert(umi_debug_variable_registry_upsert(variables, &variable) ==
           UMI_STATUS_OK);

    (void)memset(&filter, 0, sizeof(filter));
    filter.struct_size = (uint32_t)sizeof(filter);
    filter.api_version = UMI_DEBUG_THREAD_INSPECTOR_API_VERSION;
    (void)snprintf(filter.session_id, sizeof(filter.session_id), "%s", "session-1");
    filter.include_expensive_scopes = 1;
    assert(umi_debug_thread_inspector_set_filter(inspector, &filter) ==
           UMI_STATUS_OK);
    assert(umi_debug_thread_inspector_refresh(
               inspector, threads, frames, scopes, variables) == UMI_STATUS_OK);
    assert(umi_debug_thread_inspector_snapshot(inspector, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.thread_count == 2U);
    assert(snapshot.frame_count == 3U);
    assert(snapshot.scope_count == 3U);
    assert(snapshot.variable_count == 2U);
    assert(strcmp(snapshot.selected_thread_id, "thread-1") == 0);
    assert(strcmp(snapshot.selected_frame_id, "frame-1a") == 0);
    assert(strcmp(snapshot.selected_scope_id, "locals-1") == 0);

    assert(umi_debug_thread_inspector_thread_at(inspector, 0U, &thread) ==
           UMI_STATUS_OK);
    assert(thread.current);
    assert(umi_debug_thread_inspector_frame_at(inspector, 0U, &frame) ==
           UMI_STATUS_OK);
    assert(strcmp(frame.id, "frame-1a") == 0);
    assert(umi_debug_thread_inspector_variable_at(inspector, 0U, &variable) ==
           UMI_STATUS_OK);
    assert(strcmp(variable.name, "alpha") == 0);

    assert(umi_debug_thread_inspector_select_thread(inspector, "thread-2") ==
           UMI_STATUS_OK);
    assert(umi_debug_thread_inspector_selected_thread(inspector, &thread) ==
           UMI_STATUS_OK);
    assert(strcmp(thread.id, "thread-2") == 0);
    assert(umi_debug_thread_inspector_selected_frame(inspector, &frame) ==
           UMI_STATUS_OK);
    assert(strcmp(frame.id, "frame-2a") == 0);
    assert(umi_debug_thread_inspector_selected_scope(inspector, &scope) ==
           UMI_STATUS_OK);
    assert(strcmp(scope.id, "locals-2") == 0);

    umi_debug_thread_inspector_destroy(inspector);
    umi_debug_variable_registry_destroy(variables);
    umi_debug_scope_registry_destroy(scopes);
    umi_debug_stack_frame_registry_destroy(frames);
    umi_debug_thread_registry_destroy(threads);
    return 0;
}
