/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/adapter_contract.c
 *
 * PURPOSE:
 *   Implement the stable debugger-adapter registry, capability selection and
 *   bounded request dispatch used by the advanced debugging platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/adapter_contract.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugAdapterRegistry {
    UmiDebugAdapterDescriptor *items;
    size_t count;
    size_t capacity;
    uint64_t next_sequence;
    uint64_t next_response_sequence;
    uint64_t revision;
};

static int has_terminator(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static int valid_operation(UmiDebugAdapterOperation operation)
{
    return operation >= UMI_DEBUG_ADAPTER_INITIALIZE &&
           operation <= UMI_DEBUG_ADAPTER_CANCEL;
}

static int valid_cancellation(
    const UmiDebugAdapterCancellation *cancellation)
{
    return cancellation == NULL ||
           (cancellation->struct_size ==
                (uint32_t)sizeof(*cancellation) &&
            cancellation->api_version ==
                UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION &&
            cancellation->is_cancelled != NULL);
}

static int valid_request(const UmiDebugAdapterRequest *request)
{
    if (request == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION ||
        !valid_operation(request->operation) ||
        request->payload_length > sizeof(request->payload) ||
        !has_terminator(request->session_id,
                        sizeof(request->session_id)) ||
        !has_terminator(request->thread_id,
                        sizeof(request->thread_id)) ||
        !has_terminator(request->frame_id, sizeof(request->frame_id)) ||
        !has_terminator(request->reference,
                        sizeof(request->reference)) ||
        !has_terminator(request->expression,
                        sizeof(request->expression))) {
        return 0;
    }
    return 1;
}

static int valid_descriptor(const UmiDebugAdapterDescriptor *descriptor)
{
    if (descriptor == NULL ||
        descriptor->struct_size != (uint32_t)sizeof(*descriptor) ||
        descriptor->api_version != UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION ||
        !has_terminator(descriptor->id, sizeof(descriptor->id)) ||
        descriptor->id[0] == '\0' ||
        !has_terminator(descriptor->label, sizeof(descriptor->label)) ||
        descriptor->label[0] == '\0' ||
        !has_terminator(descriptor->debugger_kind,
                        sizeof(descriptor->debugger_kind)) ||
        descriptor->debugger_kind[0] == '\0' ||
        !has_terminator(descriptor->implementation_version,
                        sizeof(descriptor->implementation_version)) ||
        (descriptor->flags &
         (UmiDebugAdapterFlags)(~(uint32_t)UMI_DEBUG_ADAPTER_ALL_FLAGS)) != 0U ||
        descriptor->functions.struct_size !=
            (uint32_t)sizeof(descriptor->functions) ||
        descriptor->functions.api_version !=
            UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION ||
        descriptor->functions.invoke == NULL) {
        return 0;
    }
    return 1;
}

static size_t find_index(const UmiDebugAdapterRegistry *registry,
                         const char *adapter_id)
{
    size_t position;

    if (registry == NULL || adapter_id == NULL) return SIZE_MAX;
    for (position = 0U; position < registry->count; ++position) {
        if (strcmp(registry->items[position].id, adapter_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

static UmiStatus reserve(UmiDebugAdapterRegistry *registry,
                         size_t required_capacity)
{
    UmiDebugAdapterDescriptor *replacement;
    size_t new_capacity;

    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (required_capacity <= registry->capacity) return UMI_STATUS_OK;
    if (required_capacity > UMI_DEBUG_ADAPTER_MAXIMUM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = registry->capacity;
    if (new_capacity == 0U) new_capacity = UMI_DEBUG_ADAPTER_DEFAULT_CAPACITY;
    while (new_capacity < required_capacity) {
        if (new_capacity > UMI_DEBUG_ADAPTER_MAXIMUM_CAPACITY / 2U) {
            new_capacity = UMI_DEBUG_ADAPTER_MAXIMUM_CAPACITY;
        } else {
            new_capacity *= 2U;
        }
    }
    if (new_capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiDebugAdapterDescriptor *)realloc(
        registry->items, new_capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (new_capacity > registry->capacity) {
        (void)memset(&replacement[registry->capacity], 0,
                     (new_capacity - registry->capacity) *
                         sizeof(*replacement));
    }
    registry->items = replacement;
    registry->capacity = new_capacity;
    return UMI_STATUS_OK;
}

static int descriptor_is_better(
    const UmiDebugAdapterDescriptor *candidate,
    const UmiDebugAdapterDescriptor *current)
{
    int candidate_local;
    int current_local;
    int identifier_order;

    if (current == NULL) return 1;
    if (candidate->priority > current->priority) return 1;
    if (candidate->priority < current->priority) return 0;

    candidate_local = (candidate->flags & UMI_DEBUG_ADAPTER_LOCAL) != 0U;
    current_local = (current->flags & UMI_DEBUG_ADAPTER_LOCAL) != 0U;
    if (candidate_local != current_local) return candidate_local;
    if (candidate->sequence < current->sequence) return 1;
    if (candidate->sequence > current->sequence) return 0;
    identifier_order = strcmp(candidate->id, current->id);
    return identifier_order < 0;
}

uint64_t umi_debug_adapter_operation_required_capability(
    UmiDebugAdapterOperation operation)
{
    switch (operation) {
        case UMI_DEBUG_ADAPTER_CONFIGURATION_DONE:
            return UMI_DEBUG_CAP_CONFIGURATION_DONE;
        case UMI_DEBUG_ADAPTER_STEP_BACK:
            return UMI_DEBUG_CAP_STEP_BACK;
        case UMI_DEBUG_ADAPTER_REVERSE_CONTINUE:
            return UMI_DEBUG_CAP_REVERSE_CONTINUE;
        case UMI_DEBUG_ADAPTER_RESTART_FRAME:
            return UMI_DEBUG_CAP_RESTART_FRAME;
        case UMI_DEBUG_ADAPTER_SET_VARIABLE:
            return UMI_DEBUG_CAP_SET_VARIABLE;
        case UMI_DEBUG_ADAPTER_SET_EXPRESSION:
            return UMI_DEBUG_CAP_SET_EXPRESSION;
        case UMI_DEBUG_ADAPTER_SET_FUNCTION_BREAKPOINTS:
            return UMI_DEBUG_CAP_FUNCTION_BREAKPOINTS;
        case UMI_DEBUG_ADAPTER_SET_DATA_BREAKPOINTS:
            return UMI_DEBUG_CAP_DATA_BREAKPOINTS;
        case UMI_DEBUG_ADAPTER_SET_INSTRUCTION_BREAKPOINTS:
            return UMI_DEBUG_CAP_INSTRUCTION_BREAKPOINTS;
        case UMI_DEBUG_ADAPTER_SET_EXCEPTION_BREAKPOINTS:
            return UMI_DEBUG_CAP_EXCEPTION_FILTERS;
        case UMI_DEBUG_ADAPTER_MODULES:
            return UMI_DEBUG_CAP_MODULES;
        case UMI_DEBUG_ADAPTER_READ_MEMORY:
            return UMI_DEBUG_CAP_READ_MEMORY;
        case UMI_DEBUG_ADAPTER_WRITE_MEMORY:
            return UMI_DEBUG_CAP_WRITE_MEMORY;
        case UMI_DEBUG_ADAPTER_DISASSEMBLE:
            return UMI_DEBUG_CAP_DISASSEMBLE;
        case UMI_DEBUG_ADAPTER_REGISTERS:
            return UMI_DEBUG_CAP_REGISTERS;
        default:
            return 0U;
    }
}

UmiStatus umi_debug_adapter_registry_create(
    size_t initial_capacity,
    UmiDebugAdapterRegistry **out_registry)
{
    UmiDebugAdapterRegistry *registry;
    UmiStatus status;

    if (out_registry == NULL ||
        initial_capacity > UMI_DEBUG_ADAPTER_MAXIMUM_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_registry = NULL;
    registry = (UmiDebugAdapterRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    registry->next_sequence = 1U;
    registry->next_response_sequence = 1U;
    if (initial_capacity != 0U) {
        status = reserve(registry, initial_capacity);
        if (status != UMI_STATUS_OK) {
            free(registry);
            return status;
        }
    }
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_debug_adapter_registry_destroy(UmiDebugAdapterRegistry *registry)
{
    if (registry == NULL) return;
    free(registry->items);
    (void)memset(registry, 0, sizeof(*registry));
    free(registry);
}

UmiStatus umi_debug_adapter_registry_clear(UmiDebugAdapterRegistry *registry)
{
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (registry->items != NULL) {
        (void)memset(registry->items, 0,
                     registry->capacity * sizeof(registry->items[0]));
    }
    registry->count = 0U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_adapter_registry_register(
    UmiDebugAdapterRegistry *registry,
    const UmiDebugAdapterDescriptor *descriptor)
{
    UmiDebugAdapterDescriptor copy;
    UmiStatus status;

    if (registry == NULL || !valid_descriptor(descriptor)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_index(registry, descriptor->id) != SIZE_MAX) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    status = reserve(registry, registry->count + 1U);
    if (status != UMI_STATUS_OK) return status;

    copy = *descriptor;
    copy.struct_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    copy.id[sizeof(copy.id) - 1U] = '\0';
    copy.label[sizeof(copy.label) - 1U] = '\0';
    copy.debugger_kind[sizeof(copy.debugger_kind) - 1U] = '\0';
    copy.implementation_version[sizeof(copy.implementation_version) - 1U] =
        '\0';
    copy.functions.struct_size = (uint32_t)sizeof(copy.functions);
    copy.functions.api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    copy.sequence = registry->next_sequence;
    registry->next_sequence += 1U;
    registry->revision += 1U;
    copy.revision = registry->revision;
    registry->items[registry->count] = copy;
    registry->count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_adapter_registry_unregister(
    UmiDebugAdapterRegistry *registry,
    const char *adapter_id)
{
    size_t position;

    if (registry == NULL || adapter_id == NULL || adapter_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(registry, adapter_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (position + 1U < registry->count) {
        (void)memmove(&registry->items[position],
                      &registry->items[position + 1U],
                      (registry->count - position - 1U) *
                          sizeof(registry->items[0]));
    }
    registry->count -= 1U;
    (void)memset(&registry->items[registry->count], 0,
                 sizeof(registry->items[registry->count]));
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_adapter_registry_set_enabled(
    UmiDebugAdapterRegistry *registry,
    const char *adapter_id,
    int enabled)
{
    size_t position;
    UmiDebugAdapterFlags new_flags;

    if (registry == NULL || adapter_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(registry, adapter_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    new_flags = registry->items[position].flags;
    if (enabled != 0) {
        new_flags |= UMI_DEBUG_ADAPTER_ENABLED;
    } else {
        new_flags &=
            (UmiDebugAdapterFlags)(~(uint32_t)UMI_DEBUG_ADAPTER_ENABLED);
    }
    if (new_flags != registry->items[position].flags) {
        registry->items[position].flags = new_flags;
        registry->revision += 1U;
        registry->items[position].revision = registry->revision;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_adapter_registry_find(
    const UmiDebugAdapterRegistry *registry,
    const char *adapter_id,
    UmiDebugAdapterDescriptor *out_descriptor)
{
    size_t position;

    if (registry == NULL || adapter_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(registry, adapter_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = registry->items[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_adapter_registry_at(
    const UmiDebugAdapterRegistry *registry,
    size_t index,
    UmiDebugAdapterDescriptor *out_descriptor)
{
    if (registry == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_adapter_registry_select(
    const UmiDebugAdapterRegistry *registry,
    const char *debugger_kind,
    uint64_t required_capabilities,
    int allow_remote,
    UmiDebugAdapterDescriptor *out_descriptor)
{
    const UmiDebugAdapterDescriptor *best = NULL;
    size_t position;

    if (registry == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (position = 0U; position < registry->count; ++position) {
        const UmiDebugAdapterDescriptor *candidate =
            &registry->items[position];
        if ((candidate->flags & UMI_DEBUG_ADAPTER_ENABLED) == 0U) continue;
        if (!allow_remote &&
            (candidate->flags & UMI_DEBUG_ADAPTER_REMOTE) != 0U) {
            continue;
        }
        if (debugger_kind != NULL && debugger_kind[0] != '\0' &&
            strcmp(candidate->debugger_kind, debugger_kind) != 0) {
            continue;
        }
        if ((candidate->capabilities & required_capabilities) !=
            required_capabilities) {
            continue;
        }
        if (descriptor_is_better(candidate, best)) best = candidate;
    }
    if (best == NULL) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = *best;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_adapter_registry_invoke(
    UmiDebugAdapterRegistry *registry,
    const char *adapter_id,
    const UmiDebugAdapterRequest *request,
    const UmiDebugAdapterCancellation *cancellation,
    UmiDebugAdapterResponse *out_response)
{
    UmiDebugAdapterDescriptor *descriptor;
    uint64_t required_capability;
    size_t position;
    UmiStatus status;

    if (registry == NULL || adapter_id == NULL ||
        !valid_request(request) || !valid_cancellation(cancellation) ||
        out_response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(registry, adapter_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    descriptor = &registry->items[position];
    if ((descriptor->flags & UMI_DEBUG_ADAPTER_ENABLED) == 0U) {
        return UMI_STATUS_UNAVAILABLE;
    }
    required_capability =
        umi_debug_adapter_operation_required_capability(request->operation);
    if (required_capability != 0U &&
        (descriptor->capabilities & required_capability) !=
            required_capability) {
        return UMI_STATUS_UNAVAILABLE;
    }
    if (cancellation != NULL &&
        cancellation->is_cancelled(cancellation->user_data) != 0) {
        (void)memset(out_response, 0, sizeof(*out_response));
        out_response->struct_size = (uint32_t)sizeof(*out_response);
        out_response->api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
        out_response->request_id = request->request_id;
        out_response->operation = request->operation;
        out_response->status = UMI_STATUS_CANCELLED;
        out_response->cancelled = 1;
        out_response->complete = 1;
        return UMI_STATUS_CANCELLED;
    }

    (void)memset(out_response, 0, sizeof(*out_response));
    out_response->struct_size = (uint32_t)sizeof(*out_response);
    out_response->api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    out_response->request_id = request->request_id;
    out_response->operation = request->operation;
    out_response->status = UMI_STATUS_BUSY;
    out_response->sequence = registry->next_response_sequence;
    registry->next_response_sequence += 1U;
    out_response->revision = registry->revision;

    status = descriptor->functions.invoke(
        descriptor->instance, request, cancellation, out_response);
    out_response->message[sizeof(out_response->message) - 1U] = '\0';
    if (out_response->payload_length > sizeof(out_response->payload)) {
        out_response->payload_length = 0U;
        out_response->status = UMI_STATUS_CAPACITY_EXCEEDED;
        out_response->complete = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (status != UMI_STATUS_OK) {
        out_response->status = status;
        out_response->complete = 1;
    } else if (out_response->status == UMI_STATUS_BUSY) {
        out_response->status = UMI_STATUS_OK;
    }
    return status;
}

UmiStatus umi_debug_adapter_registry_cancel(
    UmiDebugAdapterRegistry *registry,
    const char *adapter_id,
    uint64_t request_id)
{
    size_t position;

    if (registry == NULL || adapter_id == NULL || request_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(registry, adapter_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (registry->items[position].functions.cancel == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return registry->items[position].functions.cancel(
        registry->items[position].instance, request_id);
}

UmiStatus umi_debug_adapter_registry_snapshot(
    const UmiDebugAdapterRegistry *registry,
    UmiDebugAdapterRegistrySnapshot *out_snapshot)
{
    size_t position;

    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    out_snapshot->adapter_count = registry->count;
    out_snapshot->revision = registry->revision;
    for (position = 0U; position < registry->count; ++position) {
        UmiDebugAdapterFlags flags = registry->items[position].flags;
        if ((flags & UMI_DEBUG_ADAPTER_ENABLED) != 0U) {
            out_snapshot->enabled_count += 1U;
        }
        if ((flags & UMI_DEBUG_ADAPTER_LOCAL) != 0U) {
            out_snapshot->local_count += 1U;
        }
        if ((flags & UMI_DEBUG_ADAPTER_REMOTE) != 0U) {
            out_snapshot->remote_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_debug_adapter_registry_count(
    const UmiDebugAdapterRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_debug_adapter_registry_revision(
    const UmiDebugAdapterRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
