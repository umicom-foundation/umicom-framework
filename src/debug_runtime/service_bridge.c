/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/service_bridge.c
 *
 * PURPOSE:
 *   Implement DAP -> existing Debug Service projection without global clears
 *   that would destroy another concurrent debug session's state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/service_bridge.h"

#include <stdio.h>
#include <string.h>

static void copy_bounded(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";

    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;

    if (length > 0U) {
        (void)memcpy(destination, source, length);
    }
    destination[length] = '\0';
}

static uint64_t hash_text(const char *text)
{
    const unsigned char *cursor = (const unsigned char *)text;
    uint64_t hash = 1469598103934665603ULL;

    if (text == NULL) return 0U;

    while (*cursor != '\0') {
        hash ^= (uint64_t)*cursor++;
        hash *= 1099511628211ULL;
    }

    return hash;
}

UmiStatus umi_debug_runtime_service_bridge_init(
    UmiDebugRuntimeServiceBridge *bridge,
    UmiDebugService *service,
    const char *session_id)
{
    int written;

    if (bridge == NULL || service == NULL ||
        session_id == NULL || session_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(bridge, 0, sizeof(*bridge));
    bridge->service = service;
    written = snprintf(
        bridge->session_id,
        sizeof(bridge->session_id),
        "%s",
        session_id);
    if (written < 0 || (size_t)written >= sizeof(bridge->session_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    bridge->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_runtime_publish_session(
    UmiDebugRuntimeServiceBridge *bridge,
    const char *configuration_id,
    const char *adapter_id,
    const char *state_text,
    int state,
    int attached,
    int supports_restart)
{
    UmiDebugSessionSnapshot item;

    if (bridge == NULL || bridge->service == NULL ||
        configuration_id == NULL || adapter_id == NULL ||
        state_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_DEBUG_SESSION_API_VERSION;
    copy_bounded(item.id, sizeof(item.id), bridge->session_id);
    copy_bounded(
        item.configuration_id, sizeof(item.configuration_id),
        configuration_id);
    copy_bounded(item.adapter, sizeof(item.adapter), adapter_id);
    copy_bounded(
        item.state_text, sizeof(item.state_text), state_text);
    item.state = state;
    item.attached = attached;
    item.supports_restart = supports_restart;
    item.started_at = bridge->revision;
    item.revision = ++bridge->revision;

    return umi_debug_session_registry_upsert(
        umi_debug_service_session(bridge->service),
        &item);
}

static void remove_session_threads(
    UmiDebugThreadRegistry *registry,
    const char *session_id)
{
    size_t index = umi_debug_thread_registry_count(registry);

    while (index > 0U) {
        UmiDebugThreadSnapshot item;
        index -= 1U;
        if (umi_debug_thread_registry_at(
                registry, index, &item) == UMI_STATUS_OK &&
            strcmp(item.session_id, session_id) == 0) {
            (void)umi_debug_thread_registry_remove(registry, item.id);
        }
    }
}

UmiStatus umi_debug_runtime_publish_threads(
    UmiDebugRuntimeServiceBridge *bridge,
    const UmiDebugRuntimeThreadList *result,
    uint64_t current_thread_id,
    int stopped)
{
    UmiDebugThreadRegistry *registry;
    size_t index;

    if (bridge == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    registry = umi_debug_service_thread(bridge->service);
    remove_session_threads(registry, bridge->session_id);

    for (index = 0U; index < result->count; ++index) {
        UmiDebugThreadSnapshot item;
        UmiStatus status;

        (void)memset(&item, 0, sizeof(item));
        item.struct_size = (uint32_t)sizeof(item);
        item.api_version = UMI_DEBUG_THREAD_API_VERSION;
        (void)snprintf(
            item.id, sizeof(item.id),
            "thread.%016llx.%llu",
            (unsigned long long)hash_text(bridge->session_id),
            (unsigned long long)result->items[index].id);
        copy_bounded(
            item.session_id, sizeof(item.session_id),
            bridge->session_id);
        copy_bounded(
            item.name, sizeof(item.name),
            result->items[index].name);
        item.native_id = result->items[index].id;
        item.stopped = stopped != 0;
        item.current = result->items[index].id == current_thread_id;
        item.revision = bridge->revision + 1U;

        status = umi_debug_thread_registry_upsert(registry, &item);
        if (status != UMI_STATUS_OK) return status;
    }

    bridge->revision += 1U;
    return UMI_STATUS_OK;
}

static void remove_thread_frames(
    UmiDebugStackFrameRegistry *registry,
    const char *thread_id)
{
    size_t index = umi_debug_stack_frame_registry_count(registry);

    while (index > 0U) {
        UmiDebugStackFrameSnapshot item;
        index -= 1U;
        if (umi_debug_stack_frame_registry_at(
                registry, index, &item) == UMI_STATUS_OK &&
            strcmp(item.thread_id, thread_id) == 0) {
            (void)umi_debug_stack_frame_registry_remove(registry, item.id);
        }
    }
}

UmiStatus umi_debug_runtime_publish_stack(
    UmiDebugRuntimeServiceBridge *bridge,
    uint64_t thread_id,
    const UmiDebugRuntimeStackTrace *result)
{
    UmiDebugStackFrameRegistry *registry;
    char framework_thread_id[128];
    size_t index;

    if (bridge == NULL || result == NULL || thread_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)snprintf(
        framework_thread_id,
        sizeof(framework_thread_id),
        "thread.%016llx.%llu",
        (unsigned long long)hash_text(bridge->session_id),
        (unsigned long long)thread_id);

    registry = umi_debug_service_stack_frame(bridge->service);
    remove_thread_frames(registry, framework_thread_id);

    for (index = 0U; index < result->count; ++index) {
        UmiDebugStackFrameSnapshot item;
        UmiStatus status;

        (void)memset(&item, 0, sizeof(item));
        item.struct_size = (uint32_t)sizeof(item);
        item.api_version = UMI_DEBUG_STACK_FRAME_API_VERSION;
        (void)snprintf(
            item.id, sizeof(item.id),
            "%llu",
            (unsigned long long)result->items[index].id);
        copy_bounded(
            item.thread_id, sizeof(item.thread_id),
            framework_thread_id);
        copy_bounded(
            item.name, sizeof(item.name),
            result->items[index].name);
        copy_bounded(
            item.source_uri, sizeof(item.source_uri),
            result->items[index].source.path);
        item.line = result->items[index].line;
        item.column = result->items[index].column;
        item.order = (int32_t)index;
        item.revision = bridge->revision + 1U;

        status = umi_debug_stack_frame_registry_upsert(registry, &item);
        if (status != UMI_STATUS_OK) return status;
    }

    bridge->revision += 1U;
    return UMI_STATUS_OK;
}

static void remove_frame_scopes(
    UmiDebugScopeRegistry *registry,
    const char *frame_id)
{
    size_t index = umi_debug_scope_registry_count(registry);

    while (index > 0U) {
        UmiDebugScopeSnapshot item;
        index -= 1U;
        if (umi_debug_scope_registry_at(
                registry, index, &item) == UMI_STATUS_OK &&
            strcmp(item.frame_id, frame_id) == 0) {
            (void)umi_debug_scope_registry_remove(registry, item.id);
        }
    }
}

UmiStatus umi_debug_runtime_publish_scopes(
    UmiDebugRuntimeServiceBridge *bridge,
    uint64_t frame_id,
    const UmiDebugRuntimeScopeList *result)
{
    UmiDebugScopeRegistry *registry;
    char frame_text[128];
    size_t index;

    if (bridge == NULL || result == NULL || frame_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)snprintf(
        frame_text, sizeof(frame_text),
        "%llu", (unsigned long long)frame_id);

    registry = umi_debug_service_scope(bridge->service);
    remove_frame_scopes(registry, frame_text);

    for (index = 0U; index < result->count; ++index) {
        UmiDebugScopeSnapshot item;
        UmiStatus status;

        (void)memset(&item, 0, sizeof(item));
        item.struct_size = (uint32_t)sizeof(item);
        item.api_version = UMI_DEBUG_SCOPE_API_VERSION;
        (void)snprintf(
            item.id, sizeof(item.id),
            "scope.%016llx.%llu.%zu",
            (unsigned long long)hash_text(bridge->session_id),
            (unsigned long long)frame_id,
            index);
        copy_bounded(
            item.frame_id, sizeof(item.frame_id),
            frame_text);
        copy_bounded(
            item.name, sizeof(item.name),
            result->items[index].name);
        item.variables_reference =
            result->items[index].variables_reference;
        item.expensive = result->items[index].expensive;
        item.order = (int32_t)index;
        item.revision = bridge->revision + 1U;

        status = umi_debug_scope_registry_upsert(registry, &item);
        if (status != UMI_STATUS_OK) return status;
    }

    bridge->revision += 1U;
    return UMI_STATUS_OK;
}


static void remove_scope_variables(
    UmiDebugVariableRegistry *registry,
    const char *scope_id)
{
    size_t index = umi_debug_variable_registry_count(registry);

    while (index > 0U) {
        UmiDebugVariableSnapshot item;
        index -= 1U;
        if (umi_debug_variable_registry_at(
                registry, index, &item) == UMI_STATUS_OK &&
            strcmp(item.scope_id, scope_id) == 0) {
            (void)umi_debug_variable_registry_remove(registry, item.id);
        }
    }
}

UmiStatus umi_debug_runtime_publish_variables(
    UmiDebugRuntimeServiceBridge *bridge,
    const char *scope_id,
    const UmiDebugRuntimeVariableList *result)
{
    UmiDebugVariableRegistry *registry;
    size_t index;

    if (bridge == NULL || scope_id == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    registry = umi_debug_service_variable(bridge->service);
    remove_scope_variables(registry, scope_id);

    for (index = 0U; index < result->count; ++index) {
        UmiDebugVariableSnapshot item;
        UmiStatus status;

        (void)memset(&item, 0, sizeof(item));
        item.struct_size = (uint32_t)sizeof(item);
        item.api_version = UMI_DEBUG_VARIABLE_API_VERSION;
        (void)snprintf(
            item.id, sizeof(item.id),
            "variable.%016llx.%zu",
            (unsigned long long)hash_text(scope_id),
            index);
        copy_bounded(
            item.scope_id, sizeof(item.scope_id),
            scope_id);
        copy_bounded(
            item.name, sizeof(item.name),
            result->items[index].name);
        copy_bounded(
            item.value, sizeof(item.value),
            result->items[index].value);
        copy_bounded(
            item.type, sizeof(item.type),
            result->items[index].type);
        copy_bounded(
            item.evaluate_name, sizeof(item.evaluate_name),
            result->items[index].evaluate_name);
        item.variables_reference =
            result->items[index].variables_reference;
        item.changed = 0;
        item.revision = bridge->revision + 1U;

        status = umi_debug_variable_registry_upsert(registry, &item);
        if (status != UMI_STATUS_OK) return status;
    }

    bridge->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_runtime_publish_watch(
    UmiDebugRuntimeServiceBridge *bridge,
    const char *watch_id,
    const char *expression,
    const UmiDebugRuntimeEvaluateResult *result)
{
    UmiDebugWatchSnapshot item;

    if (bridge == NULL || watch_id == NULL ||
        expression == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_DEBUG_WATCH_API_VERSION;
    copy_bounded(item.id, sizeof(item.id), watch_id);
    copy_bounded(
        item.session_id, sizeof(item.session_id),
        bridge->session_id);
    copy_bounded(
        item.expression, sizeof(item.expression),
        expression);
    copy_bounded(
        item.value, sizeof(item.value),
        result->result);
    copy_bounded(
        item.type, sizeof(item.type),
        result->type);
    item.enabled = 1;
    item.valid = 1;
    item.revision = ++bridge->revision;

    return umi_debug_watch_registry_upsert(
        umi_debug_service_watch(bridge->service),
        &item);
}

static void remove_session_modules(
    UmiDebugModuleRegistry *registry,
    const char *session_id)
{
    size_t index = umi_debug_module_registry_count(registry);

    while (index > 0U) {
        UmiDebugModuleSnapshot item;
        index -= 1U;
        if (umi_debug_module_registry_at(
                registry, index, &item) == UMI_STATUS_OK &&
            strcmp(item.session_id, session_id) == 0) {
            (void)umi_debug_module_registry_remove(registry, item.id);
        }
    }
}

UmiStatus umi_debug_runtime_publish_modules(
    UmiDebugRuntimeServiceBridge *bridge,
    const UmiDebugRuntimeModuleList *result)
{
    UmiDebugModuleRegistry *registry;
    size_t index;

    if (bridge == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    registry = umi_debug_service_module(bridge->service);
    remove_session_modules(registry, bridge->session_id);

    for (index = 0U; index < result->count; ++index) {
        UmiDebugModuleSnapshot item;
        UmiStatus status;

        (void)memset(&item, 0, sizeof(item));
        item.struct_size = (uint32_t)sizeof(item);
        item.api_version = UMI_DEBUG_MODULE_API_VERSION;
        (void)snprintf(
            item.id, sizeof(item.id),
            "module.%016llx.%zu",
            (unsigned long long)hash_text(bridge->session_id),
            index);
        copy_bounded(
            item.session_id, sizeof(item.session_id),
            bridge->session_id);
        copy_bounded(
            item.name, sizeof(item.name),
            result->items[index].name);
        copy_bounded(
            item.path, sizeof(item.path),
            result->items[index].path);
        copy_bounded(
            item.version, sizeof(item.version),
            result->items[index].version);
        (void)snprintf(
            item.symbol_status, sizeof(item.symbol_status),
            "%s", result->items[index].symbol_status);
        item.optimised = result->items[index].optimized;
        item.revision = bridge->revision + 1U;

        status = umi_debug_module_registry_upsert(registry, &item);
        if (status != UMI_STATUS_OK) return status;
    }

    bridge->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_runtime_publish_source(
    UmiDebugRuntimeServiceBridge *bridge,
    const char *source_id,
    const char *uri,
    const char *name,
    uint64_t source_reference,
    int available)
{
    UmiDebugSourceSnapshot item;

    if (bridge == NULL || source_id == NULL ||
        uri == NULL || name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_DEBUG_SOURCE_API_VERSION;
    copy_bounded(item.id, sizeof(item.id), source_id);
    copy_bounded(
        item.session_id, sizeof(item.session_id),
        bridge->session_id);
    copy_bounded(item.uri, sizeof(item.uri), uri);
    copy_bounded(item.name, sizeof(item.name), name);
    item.source_reference = source_reference;
    item.available = available != 0;
    item.revision = ++bridge->revision;

    return umi_debug_source_registry_upsert(
        umi_debug_service_source(bridge->service),
        &item);
}

UmiStatus umi_debug_runtime_publish_exception(
    UmiDebugRuntimeServiceBridge *bridge,
    const UmiDebugRuntimeExceptionInfo *result)
{
    UmiDebugExceptionSnapshot item;

    if (bridge == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_DEBUG_EXCEPTION_API_VERSION;
    (void)snprintf(
        item.id, sizeof(item.id),
        "exception.%016llx.%llu",
        (unsigned long long)hash_text(bridge->session_id),
        (unsigned long long)(bridge->revision + 1U));
    copy_bounded(
        item.session_id, sizeof(item.session_id),
        bridge->session_id);
    copy_bounded(
        item.type, sizeof(item.type),
        result->exception_id);
    copy_bounded(
        item.message, sizeof(item.message),
        result->description[0] != '\0'
            ? result->description
            : result->details);
    copy_bounded(
        item.break_mode, sizeof(item.break_mode),
        result->break_mode);
    item.caught = 0;
    item.revision = ++bridge->revision;

    return umi_debug_exception_registry_upsert(
        umi_debug_service_exception(bridge->service),
        &item);
}

UmiStatus umi_debug_runtime_publish_breakpoints(
    UmiDebugRuntimeServiceBridge *bridge,
    const char *source_uri,
    const UmiDebugRuntimeBreakpointList *result)
{
    UmiDebugBreakpointRegistry *registry;
    size_t index;

    if (bridge == NULL || source_uri == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    registry = umi_debug_service_breakpoint(bridge->service);

    for (index = 0U; index < result->count; ++index) {
        UmiDebugBreakpointSnapshot item;
        UmiStatus status;

        (void)memset(&item, 0, sizeof(item));
        item.struct_size = (uint32_t)sizeof(item);
        item.api_version = UMI_DEBUG_BREAKPOINT_API_VERSION;
        (void)snprintf(
            item.id, sizeof(item.id),
            "breakpoint.%016llx.%zu",
            (unsigned long long)hash_text(bridge->session_id),
            index);
        copy_bounded(
            item.session_id, sizeof(item.session_id),
            bridge->session_id);
        copy_bounded(
            item.uri, sizeof(item.uri),
            result->items[index].source.path[0] != '\0'
                ? result->items[index].source.path
                : source_uri);
        item.line = result->items[index].line;
        item.column = result->items[index].column;
        item.enabled = 1;
        item.verified = result->items[index].verified;
        item.revision = bridge->revision + 1U;

        status = umi_debug_breakpoint_registry_upsert(registry, &item);
        if (status != UMI_STATUS_OK) return status;
    }

    bridge->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_runtime_publish_event(
    UmiDebugRuntimeServiceBridge *bridge,
    const UmiDebugRuntimeEvent *event)
{
    UmiDebugEventSnapshot item;
    UmiStatus status;

    if (bridge == NULL || event == NULL || event->event[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_DEBUG_EVENT_API_VERSION;
    (void)snprintf(
        item.id, sizeof(item.id),
        "event.%016llx.%llu",
        (unsigned long long)hash_text(bridge->session_id),
        (unsigned long long)(bridge->revision + 1U));
    copy_bounded(
        item.session_id, sizeof(item.session_id),
        bridge->session_id);
    copy_bounded(
        item.kind, sizeof(item.kind),
        event->event);
    copy_bounded(
        item.detail, sizeof(item.detail),
        event->description[0] != '\0'
            ? event->description
            : event->reason);
    item.timestamp = bridge->revision + 1U;
    item.important =
        strcmp(event->event, "stopped") == 0 ||
        strcmp(event->event, "terminated") == 0 ||
        strcmp(event->event, "exited") == 0;
    item.revision = ++bridge->revision;

    status = umi_debug_event_registry_upsert(
        umi_debug_service_event(bridge->service),
        &item);
    if (status != UMI_STATUS_OK) return status;

    if (strcmp(event->event, "output") == 0 && event->text[0] != '\0') {
        UmiDebugConsoleEntrySnapshot console;

        (void)memset(&console, 0, sizeof(console));
        console.struct_size = (uint32_t)sizeof(console);
        console.api_version = UMI_DEBUG_CONSOLE_ENTRY_API_VERSION;
        (void)snprintf(
            console.id, sizeof(console.id),
            "console.%016llx.%llu",
            (unsigned long long)hash_text(bridge->session_id),
            (unsigned long long)(bridge->revision + 1U));
        copy_bounded(
            console.session_id, sizeof(console.session_id),
            bridge->session_id);
        copy_bounded(
            console.category, sizeof(console.category),
            event->category[0] != '\0' ? event->category : "console");
        copy_bounded(
            console.text, sizeof(console.text),
            event->text);
        console.timestamp = bridge->revision + 1U;
        console.revision = ++bridge->revision;

        status = umi_debug_console_entry_registry_upsert(
            umi_debug_service_console_entry(bridge->service),
            &console);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
