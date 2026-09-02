/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/inspection_session.c
 *
 * PURPOSE:
 *   Implement one coherent advanced inspection session over the established
 *   debugger service and stable adapter contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/inspection_session.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugInspectionSession {
    UmiDebugService *service;
    UmiDebugAdapterRegistry *adapters;
    UmiDebugThreadInspector *threads;
    UmiDebugRegisterBank *registers;
    UmiDebugMemoryView *memory;
    UmiDebugDisassemblyView *disassembly;
    UmiDebugAdvancedBreakpointRegistry *advanced_breakpoints;
    UmiDebugCapabilitySet capabilities;
    char session_id[UMI_DEBUG_INSPECTION_SESSION_ID_CAPACITY];
    char adapter_id[UMI_DEBUG_INSPECTION_ADAPTER_ID_CAPACITY];
    char last_message[UMI_DEBUG_INSPECTION_MESSAGE_CAPACITY];
    UmiDebugInspectionState state;
    UmiStatus last_status;
    uint64_t next_request_id;
    uint64_t active_request_id;
    uint64_t last_completed_request_id;
    uint64_t revision;
};

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t destination_capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || destination_capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= destination_capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the valid state operation used by this module and its client applications. */
static int valid_state(UmiDebugInspectionState state)
{
    return state >= UMI_DEBUG_INSPECTION_UNBOUND &&
           state <= UMI_DEBUG_INSPECTION_FAILED;
}

/* Provide the is bound operation used by this module and its client applications. */
static int is_bound(const UmiDebugInspectionSession *session)
{
    return session != NULL && session->session_id[0] != '\0' &&
           session->adapter_id[0] != '\0';
}

/* Provide the is paused operation used by this module and its client applications. */
static int is_paused(const UmiDebugInspectionSession *session)
{
    return session != NULL && session->state == UMI_DEBUG_INSPECTION_PAUSED;
}

/* Provide the is busy operation used by this module and its client applications. */
static int is_busy(const UmiDebugInspectionSession *session)
{
    return session != NULL && session->state == UMI_DEBUG_INSPECTION_BUSY;
}

/* Provide the has capability operation used by this module and its client applications. */
static int has_capability(const UmiDebugInspectionSession *session,
                          uint64_t capability)
{
    return session != NULL &&
           umi_debug_capability_set_has(&session->capabilities, capability);
}

/*
 * Provide the reset capabilities operation used by this module and its client
 * applications.
 */
static void reset_capabilities(UmiDebugInspectionSession *session)
{
    umi_debug_capability_set_init(&session->capabilities);
    session->capabilities.advertised = 0U;
    session->capabilities.required = 0U;
}

/*
 * Provide the clear advanced models operation used by this module and its client
 * applications.
 */
static UmiStatus clear_advanced_models(UmiDebugInspectionSession *session)
{
    UmiStatus status;

    status = umi_debug_thread_inspector_clear(session->threads);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_register_bank_clear(session->registers);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_memory_view_clear(session->memory);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_disassembly_view_clear(session->disassembly);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_debug_advanced_breakpoint_registry_clear(
        session->advanced_breakpoints);
}

/*
 * Initialise debug inspection session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_inspection_session_create(
    UmiDebugService *service,
    UmiDebugAdapterRegistry *adapters,
    UmiDebugInspectionSession **out_session)
{
    UmiDebugInspectionSession *session;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || adapters == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_session = NULL;
    session = (UmiDebugInspectionSession *)calloc(1U, sizeof(*session));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->service = service;
    session->adapters = adapters;
    session->state = UMI_DEBUG_INSPECTION_UNBOUND;
    session->last_status = UMI_STATUS_OK;
    session->next_request_id = 1U;
    session->revision = 1U;
    reset_capabilities(session);

    status = umi_debug_thread_inspector_create(&session->threads);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_debug_register_bank_create(&session->registers);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_debug_memory_view_create(&session->memory);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_debug_disassembly_view_create(&session->disassembly);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_debug_advanced_breakpoint_registry_create(
            0U, &session->advanced_breakpoints);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_debug_inspection_session_destroy(session);
        return status;
    }
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by debug inspection session so the same storage can be
 * reused safely.
 */
void umi_debug_inspection_session_destroy(UmiDebugInspectionSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    umi_debug_advanced_breakpoint_registry_destroy(
        session->advanced_breakpoints);
    umi_debug_disassembly_view_destroy(session->disassembly);
    umi_debug_memory_view_destroy(session->memory);
    umi_debug_register_bank_destroy(session->registers);
    umi_debug_thread_inspector_destroy(session->threads);
    (void)memset(session, 0, sizeof(*session));
    free(session);
}

/*
 * Provide the debug inspection session bind operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_inspection_session_bind(
    UmiDebugInspectionSession *session,
    const char *debug_session_id,
    const char *adapter_id,
    uint64_t advertised_capabilities)
{
    UmiDebugSessionSnapshot debug_session;
    UmiDebugAdapterDescriptor adapter;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || debug_session_id == NULL ||
        debug_session_id[0] == '\0' || adapter_id == NULL ||
        adapter_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_debug_session_registry_find(
        umi_debug_service_session(session->service), debug_session_id,
        &debug_session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_adapter_registry_find(session->adapters, adapter_id,
                                             &adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this operation only while the related capability or state is available. */
    if ((adapter.flags & UMI_DEBUG_ADAPTER_ENABLED) == 0U) {
        return UMI_STATUS_UNAVAILABLE;
    }

    status = clear_advanced_models(session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(session->session_id, sizeof(session->session_id),
                       debug_session.id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(session->adapter_id, sizeof(session->adapter_id),
                       adapter.id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        session->session_id[0] = '\0';
        return status;
    }
    reset_capabilities(session);
    session->capabilities.supported = adapter.capabilities;
    status = umi_debug_capability_set_advertise(
        &session->capabilities,
        advertised_capabilities & adapter.capabilities);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    session->state = UMI_DEBUG_INSPECTION_READY;
    session->last_status = UMI_STATUS_OK;
    session->active_request_id = 0U;
    session->last_completed_request_id = 0U;
    session->last_message[0] = '\0';
    session->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug inspection session unbind operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_inspection_session_unbind(
    UmiDebugInspectionSession *session)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = clear_advanced_models(session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    session->session_id[0] = '\0';
    session->adapter_id[0] = '\0';
    session->last_message[0] = '\0';
    session->state = UMI_DEBUG_INSPECTION_UNBOUND;
    session->last_status = UMI_STATUS_OK;
    session->active_request_id = 0U;
    session->last_completed_request_id = 0U;
    reset_capabilities(session);
    session->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug inspection session set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_inspection_session_set_state(
    UmiDebugInspectionSession *session,
    UmiDebugInspectionState state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || !valid_state(state)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!is_bound(session) && state != UMI_DEBUG_INSPECTION_UNBOUND) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (is_bound(session) && state == UMI_DEBUG_INSPECTION_UNBOUND) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->state != state) {
        session->state = state;
        session->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the debug inspection session refresh threads operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_inspection_session_refresh_threads(
    UmiDebugInspectionSession *session)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!is_bound(session)) return UMI_STATUS_INVALID_STATE;
    status = umi_debug_thread_inspector_refresh(
        session->threads,
        umi_debug_service_thread(session->service),
        umi_debug_service_stack_frame(session->service),
        umi_debug_service_scope(session->service),
        umi_debug_service_variable(session->service));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) session->revision += 1U;
    session->last_status = status;
    return status;
}

/*
 * Provide the debug inspection session invoke operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_inspection_session_invoke(
    UmiDebugInspectionSession *session,
    const UmiDebugAdapterRequest *request,
    const UmiDebugAdapterCancellation *cancellation,
    UmiDebugAdapterResponse *out_response)
{
    UmiDebugAdapterRequest effective_request;
    UmiDebugInspectionState previous_state;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || request == NULL || out_response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!is_bound(session)) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (is_busy(session)) return UMI_STATUS_BUSY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (request->session_id[0] != '\0' &&
        strcmp(request->session_id, session->session_id) != 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    effective_request = *request;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (effective_request.request_id == 0U) {
        effective_request.request_id = session->next_request_id;
        session->next_request_id += 1U;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (effective_request.session_id[0] == '\0') {
        status = copy_text(effective_request.session_id,
                           sizeof(effective_request.session_id),
                           session->session_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    (void)memset(out_response, 0, sizeof(*out_response));
    out_response->struct_size = (uint32_t)sizeof(*out_response);
    out_response->api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    out_response->status = UMI_STATUS_INVALID_STATE;
    previous_state = session->state;
    session->state = UMI_DEBUG_INSPECTION_BUSY;
    session->active_request_id = effective_request.request_id;
    session->revision += 1U;

    status = umi_debug_adapter_registry_invoke(
        session->adapters, session->adapter_id, &effective_request,
        cancellation, out_response);
    session->last_status = status == UMI_STATUS_OK
                               ? out_response->status
                               : status;
    (void)copy_text(session->last_message, sizeof(session->last_message),
                    out_response->message);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_BUSY ||
        (status == UMI_STATUS_OK && !out_response->complete)) {
        session->state = UMI_DEBUG_INSPECTION_BUSY;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        session->active_request_id = 0U;
        session->last_completed_request_id = effective_request.request_id;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_CANCELLED || out_response->cancelled) {
            session->state = UMI_DEBUG_INSPECTION_CANCELLED;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK ||
                   out_response->status != UMI_STATUS_OK) {
            session->state = UMI_DEBUG_INSPECTION_FAILED;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            session->state = previous_state == UMI_DEBUG_INSPECTION_BUSY
                                 ? UMI_DEBUG_INSPECTION_READY
                                 : previous_state;
        }
    }
    session->revision += 1U;
    return status;
}

/*
 * Provide the debug inspection session cancel active operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_inspection_session_cancel_active(
    UmiDebugInspectionSession *session)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!is_bound(session) || session->active_request_id == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_debug_adapter_registry_cancel(
        session->adapters, session->adapter_id, session->active_request_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        session->last_completed_request_id = session->active_request_id;
        session->active_request_id = 0U;
        session->state = UMI_DEBUG_INSPECTION_CANCELLED;
        session->last_status = UMI_STATUS_CANCELLED;
        session->revision += 1U;
    }
    return status;
}

/*
 * Provide the debug inspection session write memory operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_inspection_session_write_memory(
    UmiDebugInspectionSession *session,
    const UmiDebugMemoryWritePlan *plan,
    const UmiDebugAdapterCancellation *cancellation,
    UmiDebugAdapterResponse *out_response)
{
    UmiDebugAdapterRequest request;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || plan == NULL || out_response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!is_paused(session) ||
        !has_capability(session, UMI_DEBUG_CAP_WRITE_MEMORY)) {
        return UMI_STATUS_UNAVAILABLE;
    }
    status = umi_debug_memory_view_validate_write(session->memory, plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&request, 0, sizeof(request));
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    request.operation = UMI_DEBUG_ADAPTER_WRITE_MEMORY;
    status = copy_text(request.reference, sizeof(request.reference),
                       plan->memory_reference);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    request.address = plan->absolute_address;
    request.offset = plan->reference_offset;
    request.count = plan->length;
    request.payload_length = plan->length;
    (void)memcpy(request.payload, plan->replacement, plan->length);

    status = umi_debug_inspection_session_invoke(
        session, &request, cancellation, out_response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && out_response->complete &&
        out_response->status == UMI_STATUS_OK) {
        status = umi_debug_memory_view_commit_write(session->memory, plan);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) session->revision += 1U;
    }
    return status;
}

/*
 * Provide the debug inspection session command enabled operation used by this module and
 * its client applications.
 */
int umi_debug_inspection_session_command_enabled(
    const UmiDebugInspectionSession *session,
    UmiDebugCommandKind command_kind)
{
    const UmiDebugCommandDescriptor *descriptor;
    int bound;
    int paused;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return 0;
    bound = is_bound(session);
    paused = is_paused(session);
    /* Apply this branch only when its contract condition is satisfied. */
    if (is_busy(session)) return 0;

    /* Select the behaviour associated with the requested command or state value. */
    switch (command_kind) {
        case UMI_DEBUG_COMMAND_ADD_FUNCTION_BREAKPOINT:
            return bound && has_capability(
                                session, UMI_DEBUG_CAP_FUNCTION_BREAKPOINTS);
        case UMI_DEBUG_COMMAND_ADD_DATA_BREAKPOINT:
            return bound && has_capability(session,
                                           UMI_DEBUG_CAP_DATA_BREAKPOINTS);
        case UMI_DEBUG_COMMAND_ADD_INSTRUCTION_BREAKPOINT:
        case UMI_DEBUG_COMMAND_TOGGLE_INSTRUCTION_BREAKPOINT:
            return paused && has_capability(
                                 session,
                                 UMI_DEBUG_CAP_INSTRUCTION_BREAKPOINTS);
        case UMI_DEBUG_COMMAND_CONFIGURE_EXCEPTION_BREAKPOINTS:
            return bound &&
                   has_capability(session, UMI_DEBUG_CAP_EXCEPTION_FILTERS);
        case UMI_DEBUG_COMMAND_OPEN_ADVANCED_BREAKPOINTS:
        case UMI_DEBUG_COMMAND_OPEN_ADAPTER_CENTRE:
            return 1;
        case UMI_DEBUG_COMMAND_OPEN_THREADS:
            return 1;
        case UMI_DEBUG_COMMAND_SELECT_THREAD:
        case UMI_DEBUG_COMMAND_SELECT_FRAME:
        case UMI_DEBUG_COMMAND_REFRESH_VARIABLES:
            return paused;
        case UMI_DEBUG_COMMAND_REFRESH_THREADS:
            return bound;
        case UMI_DEBUG_COMMAND_SET_VARIABLE:
            return paused &&
                   has_capability(session, UMI_DEBUG_CAP_SET_VARIABLE);
        case UMI_DEBUG_COMMAND_SET_EXPRESSION:
            return paused &&
                   has_capability(session, UMI_DEBUG_CAP_SET_EXPRESSION);
        case UMI_DEBUG_COMMAND_STEP_BACK:
            return paused && has_capability(session, UMI_DEBUG_CAP_STEP_BACK);
        case UMI_DEBUG_COMMAND_REVERSE_CONTINUE:
            return paused &&
                   has_capability(session, UMI_DEBUG_CAP_REVERSE_CONTINUE);
        case UMI_DEBUG_COMMAND_RESTART_FRAME:
            return paused &&
                   has_capability(session, UMI_DEBUG_CAP_RESTART_FRAME);
        case UMI_DEBUG_COMMAND_OPEN_MODULES:
            return bound && has_capability(session, UMI_DEBUG_CAP_MODULES);
        case UMI_DEBUG_COMMAND_OPEN_REGISTERS:
        case UMI_DEBUG_COMMAND_REFRESH_REGISTERS:
            return paused && has_capability(session, UMI_DEBUG_CAP_REGISTERS);
        case UMI_DEBUG_COMMAND_REFRESH_MEMORY:
            return paused &&
                   has_capability(session, UMI_DEBUG_CAP_READ_MEMORY);
        case UMI_DEBUG_COMMAND_WRITE_MEMORY:
            return paused &&
                   has_capability(session, UMI_DEBUG_CAP_WRITE_MEMORY) &&
                   umi_debug_memory_view_count(session->memory) != 0U;
        case UMI_DEBUG_COMMAND_REFRESH_DISASSEMBLY:
            return paused &&
                   has_capability(session, UMI_DEBUG_CAP_DISASSEMBLE);
        case UMI_DEBUG_COMMAND_COPY_MEMORY_ADDRESS:
            return paused &&
                   umi_debug_memory_view_count(session->memory) != 0U;
        default:
            break;
    }

    descriptor = umi_debug_command_for_kind(command_kind);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL) return 0;
    /* Apply this operation only while the related capability or state is available. */
    if (descriptor->requires_active_session && !bound) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->requires_paused_session && !paused) return 0;
    return 1;
}

/*
 * Provide the debug inspection session threads operation used by this module and its
 * client applications.
 */
UmiDebugThreadInspector *umi_debug_inspection_session_threads(
    UmiDebugInspectionSession *session)
{
    return session != NULL ? session->threads : NULL;
}

/*
 * Provide the debug inspection session registers operation used by this module and its
 * client applications.
 */
UmiDebugRegisterBank *umi_debug_inspection_session_registers(
    UmiDebugInspectionSession *session)
{
    return session != NULL ? session->registers : NULL;
}

/*
 * Provide the debug inspection session memory operation used by this module and its client
 * applications.
 */
UmiDebugMemoryView *umi_debug_inspection_session_memory(
    UmiDebugInspectionSession *session)
{
    return session != NULL ? session->memory : NULL;
}

/*
 * Provide the debug inspection session disassembly operation used by this module and its
 * client applications.
 */
UmiDebugDisassemblyView *umi_debug_inspection_session_disassembly(
    UmiDebugInspectionSession *session)
{
    return session != NULL ? session->disassembly : NULL;
}

/*
 * Provide the debug inspection session advanced breakpoints operation used by this module
 * and its client applications.
 */
UmiDebugAdvancedBreakpointRegistry *
umi_debug_inspection_session_advanced_breakpoints(
    UmiDebugInspectionSession *session)
{
    return session != NULL ? session->advanced_breakpoints : NULL;
}

/*
 * Provide the debug inspection session capabilities operation used by this module and its
 * client applications.
 */
const UmiDebugCapabilitySet *umi_debug_inspection_session_capabilities(
    const UmiDebugInspectionSession *session)
{
    return session != NULL ? &session->capabilities : NULL;
}

/*
 * Provide the debug inspection session snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_inspection_session_snapshot(
    const UmiDebugInspectionSession *session,
    UmiDebugInspectionSessionSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEBUG_INSPECTION_SESSION_API_VERSION;
    (void)copy_text(out_snapshot->session_id, sizeof(out_snapshot->session_id),
                    session->session_id);
    (void)copy_text(out_snapshot->adapter_id, sizeof(out_snapshot->adapter_id),
                    session->adapter_id);
    (void)copy_text(out_snapshot->last_message,
                    sizeof(out_snapshot->last_message),
                    session->last_message);
    out_snapshot->state = session->state;
    out_snapshot->last_status = session->last_status;
    out_snapshot->advertised_capabilities =
        session->capabilities.advertised;
    out_snapshot->active_request_id = session->active_request_id;
    out_snapshot->last_completed_request_id =
        session->last_completed_request_id;
    out_snapshot->thread_count =
        umi_debug_thread_inspector_thread_count(session->threads);
    out_snapshot->frame_count =
        umi_debug_thread_inspector_frame_count(session->threads);
    out_snapshot->scope_count =
        umi_debug_thread_inspector_scope_count(session->threads);
    out_snapshot->variable_count =
        umi_debug_thread_inspector_variable_count(session->threads);
    out_snapshot->register_count =
        umi_debug_register_bank_register_count(session->registers);
    out_snapshot->memory_block_count =
        umi_debug_memory_view_count(session->memory);
    out_snapshot->instruction_count =
        umi_debug_disassembly_view_count(session->disassembly);
    out_snapshot->advanced_breakpoint_count =
        umi_debug_advanced_breakpoint_registry_count(
            session->advanced_breakpoints);
    out_snapshot->revision = session->revision;
    out_snapshot->bound = is_bound(session);
    out_snapshot->can_read_memory =
        has_capability(session, UMI_DEBUG_CAP_READ_MEMORY);
    out_snapshot->can_write_memory =
        has_capability(session, UMI_DEBUG_CAP_WRITE_MEMORY);
    out_snapshot->can_read_registers =
        has_capability(session, UMI_DEBUG_CAP_REGISTERS);
    out_snapshot->can_disassemble =
        has_capability(session, UMI_DEBUG_CAP_DISASSEMBLE);
    out_snapshot->can_step_back =
        has_capability(session, UMI_DEBUG_CAP_STEP_BACK);
    out_snapshot->can_reverse_continue =
        has_capability(session, UMI_DEBUG_CAP_REVERSE_CONTINUE);
    out_snapshot->can_data_breakpoint =
        has_capability(session, UMI_DEBUG_CAP_DATA_BREAKPOINTS);
    out_snapshot->can_instruction_breakpoint =
        has_capability(session, UMI_DEBUG_CAP_INSTRUCTION_BREAKPOINTS);
    return UMI_STATUS_OK;
}

/*
 * Provide the debug inspection session revision operation used by this module and its
 * client applications.
 */
uint64_t umi_debug_inspection_session_revision(
    const UmiDebugInspectionSession *session)
{
    return session != NULL ? session->revision : 0U;
}
