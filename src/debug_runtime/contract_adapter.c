/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/contract_adapter.c
 *
 * PURPOSE:
 *   Implement the existing stable debugger adapter ABI on top of a real DAP
 *   process. This is the key boundary that lets advanced inspection and future
 *   thin IDE applications use DAP without knowing protocol details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/contract_adapter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/debug_runtime/requests/requests.h"
#include "umicom/language_runtime/json_writer.h"

struct UmiDebugRuntimeContractAdapter {
    UmiDebugRuntimeAdapter *adapter;
    char id[UMI_DEBUG_ADAPTER_ID_CAPACITY];
    uint64_t capabilities;
    uint32_t timeout_ms;
    uint64_t sequence;
    uint64_t active_contract_request_id;
    uint64_t active_dap_sequence;
};

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

static uint64_t parse_u64(const char *text)
{
    char *end = NULL;
    unsigned long long value;

    if (text == NULL || text[0] == '\0') return 0U;
    value = strtoull(text, &end, 0);
    return end != text && *end == '\0'
        ? (uint64_t)value
        : 0U;
}

static const char *operation_command(UmiDebugAdapterOperation operation)
{
    switch (operation) {
        case UMI_DEBUG_ADAPTER_INITIALIZE: return "initialize";
        case UMI_DEBUG_ADAPTER_LAUNCH: return "launch";
        case UMI_DEBUG_ADAPTER_ATTACH: return "attach";
        case UMI_DEBUG_ADAPTER_CONFIGURATION_DONE: return "configurationDone";
        case UMI_DEBUG_ADAPTER_DISCONNECT: return "disconnect";
        case UMI_DEBUG_ADAPTER_CONTINUE: return "continue";
        case UMI_DEBUG_ADAPTER_PAUSE: return "pause";
        case UMI_DEBUG_ADAPTER_STEP_OVER: return "next";
        case UMI_DEBUG_ADAPTER_STEP_INTO: return "stepIn";
        case UMI_DEBUG_ADAPTER_STEP_OUT: return "stepOut";
        case UMI_DEBUG_ADAPTER_STEP_BACK: return "stepBack";
        case UMI_DEBUG_ADAPTER_REVERSE_CONTINUE: return "reverseContinue";
        case UMI_DEBUG_ADAPTER_RESTART_FRAME: return "restartFrame";
        case UMI_DEBUG_ADAPTER_THREADS: return "threads";
        case UMI_DEBUG_ADAPTER_STACK_TRACE: return "stackTrace";
        case UMI_DEBUG_ADAPTER_SCOPES: return "scopes";
        case UMI_DEBUG_ADAPTER_VARIABLES: return "variables";
        case UMI_DEBUG_ADAPTER_EVALUATE: return "evaluate";
        case UMI_DEBUG_ADAPTER_SET_VARIABLE: return "setVariable";
        case UMI_DEBUG_ADAPTER_SET_EXPRESSION: return "setExpression";
        case UMI_DEBUG_ADAPTER_SET_SOURCE_BREAKPOINTS: return "setBreakpoints";
        case UMI_DEBUG_ADAPTER_SET_FUNCTION_BREAKPOINTS:
            return "setFunctionBreakpoints";
        case UMI_DEBUG_ADAPTER_SET_DATA_BREAKPOINTS:
            return "setDataBreakpoints";
        case UMI_DEBUG_ADAPTER_SET_INSTRUCTION_BREAKPOINTS:
            return "setInstructionBreakpoints";
        case UMI_DEBUG_ADAPTER_SET_EXCEPTION_BREAKPOINTS:
            return "setExceptionBreakpoints";
        case UMI_DEBUG_ADAPTER_MODULES: return "modules";
        case UMI_DEBUG_ADAPTER_READ_MEMORY: return "readMemory";
        case UMI_DEBUG_ADAPTER_WRITE_MEMORY: return "writeMemory";
        case UMI_DEBUG_ADAPTER_DISASSEMBLE: return "disassemble";
        case UMI_DEBUG_ADAPTER_CANCEL: return "cancel";
        case UMI_DEBUG_ADAPTER_REGISTERS:
        default: return NULL;
    }
}

static UmiStatus request_payload_json(
    const UmiDebugAdapterRequest *request,
    char *out_json,
    size_t capacity,
    const char **out_arguments)
{
    UmiLanguageRuntimeJsonWriter writer;
    uint64_t id;

    if (request->payload_length > 0U) {
        if (request->payload_length + 1U > capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)memcpy(out_json, request->payload, request->payload_length);
        out_json[request->payload_length] = '\0';
        *out_arguments = out_json;
        return UMI_STATUS_OK;
    }

    *out_arguments = NULL;

    switch (request->operation) {
        case UMI_DEBUG_ADAPTER_CONTINUE:
        case UMI_DEBUG_ADAPTER_PAUSE:
        case UMI_DEBUG_ADAPTER_STEP_OVER:
        case UMI_DEBUG_ADAPTER_STEP_INTO:
        case UMI_DEBUG_ADAPTER_STEP_OUT:
        case UMI_DEBUG_ADAPTER_STEP_BACK:
        case UMI_DEBUG_ADAPTER_REVERSE_CONTINUE:
        case UMI_DEBUG_ADAPTER_STACK_TRACE:
            id = parse_u64(request->thread_id);
            if (id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
            umi_language_runtime_json_writer_init(
                &writer, out_json, capacity);
            (void)umi_language_runtime_json_writer_raw(
                &writer, "{\"threadId\":");
            (void)umi_language_runtime_json_writer_uint64(&writer, id);
            if (request->operation == UMI_DEBUG_ADAPTER_STACK_TRACE &&
                request->count > 0U) {
                (void)umi_language_runtime_json_writer_raw(
                    &writer, ",\"levels\":");
                (void)umi_language_runtime_json_writer_uint64(
                    &writer, request->count);
            }
            (void)umi_language_runtime_json_writer_raw(&writer, "}");
            if (writer.status != UMI_STATUS_OK) return writer.status;
            *out_arguments = out_json;
            return UMI_STATUS_OK;

        case UMI_DEBUG_ADAPTER_SCOPES:
        case UMI_DEBUG_ADAPTER_RESTART_FRAME:
            id = parse_u64(request->frame_id);
            if (id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
            umi_language_runtime_json_writer_init(
                &writer, out_json, capacity);
            (void)umi_language_runtime_json_writer_raw(
                &writer, "{\"frameId\":");
            (void)umi_language_runtime_json_writer_uint64(&writer, id);
            (void)umi_language_runtime_json_writer_raw(&writer, "}");
            if (writer.status != UMI_STATUS_OK) return writer.status;
            *out_arguments = out_json;
            return UMI_STATUS_OK;

        case UMI_DEBUG_ADAPTER_VARIABLES:
            id = parse_u64(request->reference);
            if (id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
            umi_language_runtime_json_writer_init(
                &writer, out_json, capacity);
            (void)umi_language_runtime_json_writer_raw(
                &writer, "{\"variablesReference\":");
            (void)umi_language_runtime_json_writer_uint64(&writer, id);
            (void)umi_language_runtime_json_writer_raw(&writer, "}");
            if (writer.status != UMI_STATUS_OK) return writer.status;
            *out_arguments = out_json;
            return UMI_STATUS_OK;

        case UMI_DEBUG_ADAPTER_EVALUATE:
            if (request->expression[0] == '\0') {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            umi_language_runtime_json_writer_init(
                &writer, out_json, capacity);
            (void)umi_language_runtime_json_writer_raw(
                &writer, "{\"expression\":");
            (void)umi_language_runtime_json_writer_string(
                &writer, request->expression);
            id = parse_u64(request->frame_id);
            if (id != 0U) {
                (void)umi_language_runtime_json_writer_raw(
                    &writer, ",\"frameId\":");
                (void)umi_language_runtime_json_writer_uint64(&writer, id);
            }
            (void)umi_language_runtime_json_writer_raw(
                &writer, ",\"context\":\"watch\"}");
            if (writer.status != UMI_STATUS_OK) return writer.status;
            *out_arguments = out_json;
            return UMI_STATUS_OK;

        case UMI_DEBUG_ADAPTER_READ_MEMORY:
        case UMI_DEBUG_ADAPTER_DISASSEMBLE:
            if (request->reference[0] == '\0') {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            umi_language_runtime_json_writer_init(
                &writer, out_json, capacity);
            (void)umi_language_runtime_json_writer_raw(
                &writer, "{\"memoryReference\":");
            (void)umi_language_runtime_json_writer_string(
                &writer, request->reference);
            (void)umi_language_runtime_json_writer_raw(
                &writer, ",\"offset\":");
            (void)umi_language_runtime_json_writer_int64(
                &writer, request->offset);

            if (request->operation == UMI_DEBUG_ADAPTER_READ_MEMORY) {
                (void)umi_language_runtime_json_writer_raw(
                    &writer, ",\"count\":");
                (void)umi_language_runtime_json_writer_uint64(
                    &writer, request->count);
            } else {
                (void)umi_language_runtime_json_writer_raw(
                    &writer, ",\"instructionCount\":");
                (void)umi_language_runtime_json_writer_uint64(
                    &writer, request->count);
                (void)umi_language_runtime_json_writer_raw(
                    &writer, ",\"resolveSymbols\":true");
            }

            (void)umi_language_runtime_json_writer_raw(&writer, "}");
            if (writer.status != UMI_STATUS_OK) return writer.status;
            *out_arguments = out_json;
            return UMI_STATUS_OK;

        case UMI_DEBUG_ADAPTER_CONFIGURATION_DONE:
        case UMI_DEBUG_ADAPTER_THREADS:
        case UMI_DEBUG_ADAPTER_MODULES:
        case UMI_DEBUG_ADAPTER_DISCONNECT:
        case UMI_DEBUG_ADAPTER_CANCEL:
            return UMI_STATUS_OK;

        default:
            /*
             * Complex mutation operations must supply a typed/validated JSON
             * payload from their higher-level request builder.
             */
            return UMI_STATUS_INVALID_ARGUMENT;
    }
}

static UmiStatus contract_invoke(
    void *instance,
    const UmiDebugAdapterRequest *request,
    const UmiDebugAdapterCancellation *cancellation,
    UmiDebugAdapterResponse *out_response)
{
    UmiDebugRuntimeContractAdapter *owner =
        (UmiDebugRuntimeContractAdapter *)instance;
    UmiDebugRuntimeEnvelope response;
    char arguments[UMI_DEBUG_ADAPTER_PAYLOAD_CAPACITY];
    const char *arguments_json = NULL;
    const char *command;
    UmiStatus status;
    size_t length;

    if (owner == NULL || request == NULL || out_response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (cancellation != NULL &&
        cancellation->is_cancelled != NULL &&
        cancellation->is_cancelled(cancellation->user_data)) {
        return UMI_STATUS_CANCELLED;
    }

    command = operation_command(request->operation);
    if (command == NULL) return UMI_STATUS_NOT_IMPLEMENTED;

    status = request_payload_json(
        request,
        arguments,
        sizeof(arguments),
        &arguments_json);
    if (status != UMI_STATUS_OK) return status;

    {
        uint64_t dap_sequence = 0U;

        status = umi_debug_runtime_adapter_send_request(
            owner->adapter,
            command,
            arguments_json,
            request->session_id,
            &dap_sequence);

        if (status == UMI_STATUS_OK) {
            owner->active_contract_request_id = request->request_id;
            owner->active_dap_sequence = dap_sequence;

            status = umi_debug_runtime_adapter_wait_response(
                owner->adapter,
                dap_sequence,
                owner->timeout_ms,
                &response);

            owner->active_contract_request_id = 0U;
            owner->active_dap_sequence = 0U;
        }
    }

    (void)memset(out_response, 0, sizeof(*out_response));
    out_response->struct_size = (uint32_t)sizeof(*out_response);
    out_response->api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    out_response->request_id = request->request_id;
    out_response->operation = request->operation;
    out_response->status = status;
    out_response->sequence = ++owner->sequence;
    out_response->revision = out_response->sequence;
    out_response->complete = 1;
    out_response->cancelled = status == UMI_STATUS_CANCELLED;

    if (response.message[0] != '\0') {
        copy_bounded(
            out_response->message,
            sizeof(out_response->message),
            response.message);
    } else {
        (void)snprintf(
            out_response->message,
            sizeof(out_response->message),
            "%s",
            status == UMI_STATUS_OK ? "DAP request completed." :
            umi_status_text(status));
    }

    length = strlen(response.json);
    if (length > sizeof(out_response->payload)) {
        out_response->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    if (length > 0U) {
        (void)memcpy(out_response->payload, response.json, length);
        out_response->payload_length = length;
    }

    return status;
}

static UmiStatus contract_cancel(void *instance, uint64_t request_id)
{
    UmiDebugRuntimeContractAdapter *owner =
        (UmiDebugRuntimeContractAdapter *)instance;
    uint64_t sequence = 0U;

    if (owner == NULL || request_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (owner->active_contract_request_id != request_id ||
        owner->active_dap_sequence == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }

    return umi_debug_runtime_request_cancel(
        owner->adapter,
        owner->active_dap_sequence,
        0U,
        &sequence);
}

static UmiStatus contract_health(
    void *instance,
    char *out_message,
    size_t message_capacity)
{
    UmiDebugRuntimeContractAdapter *owner =
        (UmiDebugRuntimeContractAdapter *)instance;

    if (owner == NULL || out_message == NULL || message_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)snprintf(
        out_message,
        message_capacity,
        "%s",
        umi_debug_runtime_adapter_is_running(owner->adapter)
            ? "DAP adapter process is running."
            : "DAP adapter process is not running.");

    return umi_debug_runtime_adapter_is_running(owner->adapter)
        ? UMI_STATUS_OK
        : UMI_STATUS_UNAVAILABLE;
}

UmiStatus umi_debug_runtime_contract_adapter_create(
    UmiDebugRuntimeAdapter *adapter,
    const char *descriptor_id,
    const char *label,
    const char *debugger_kind,
    uint64_t capabilities,
    uint32_t timeout_ms,
    UmiDebugRuntimeContractAdapter **out_owner,
    UmiDebugAdapterDescriptor *out_descriptor)
{
    UmiDebugRuntimeContractAdapter *owner;

    if (adapter == NULL || descriptor_id == NULL ||
        label == NULL || debugger_kind == NULL ||
        out_owner == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_owner = NULL;
    (void)memset(out_descriptor, 0, sizeof(*out_descriptor));

    owner = (UmiDebugRuntimeContractAdapter *)calloc(1U, sizeof(*owner));
    if (owner == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    owner->adapter = adapter;
    owner->capabilities = capabilities;
    owner->timeout_ms = timeout_ms > 0U ? timeout_ms : 1000U;
    (void)snprintf(owner->id, sizeof(owner->id), "%s", descriptor_id);

    out_descriptor->struct_size = (uint32_t)sizeof(*out_descriptor);
    out_descriptor->api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    (void)snprintf(out_descriptor->id, sizeof(out_descriptor->id),
                   "%s", descriptor_id);
    (void)snprintf(out_descriptor->label, sizeof(out_descriptor->label),
                   "%s", label);
    (void)snprintf(
        out_descriptor->debugger_kind,
        sizeof(out_descriptor->debugger_kind),
        "%s",
        debugger_kind);
    (void)snprintf(
        out_descriptor->implementation_version,
        sizeof(out_descriptor->implementation_version),
        "%s",
        "dap-runtime");
    out_descriptor->capabilities = capabilities;
    out_descriptor->flags =
        UMI_DEBUG_ADAPTER_ENABLED |
        UMI_DEBUG_ADAPTER_LOCAL |
        UMI_DEBUG_ADAPTER_SUPERVISED_PROCESS |
        UMI_DEBUG_ADAPTER_REQUIRES_WORKSPACE_TRUST;
    out_descriptor->priority = 100;
    out_descriptor->sequence = 1U;
    out_descriptor->revision = 1U;
    out_descriptor->instance = owner;

    out_descriptor->functions.struct_size =
        (uint32_t)sizeof(out_descriptor->functions);
    out_descriptor->functions.api_version =
        UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    out_descriptor->functions.invoke = contract_invoke;
    out_descriptor->functions.cancel = contract_cancel;
    out_descriptor->functions.health = contract_health;

    *out_owner = owner;
    return UMI_STATUS_OK;
}

void umi_debug_runtime_contract_adapter_destroy(
    UmiDebugRuntimeContractAdapter *owner)
{
    if (owner == NULL) return;
    umi_debug_runtime_adapter_destroy(owner->adapter);
    owner->adapter = NULL;
    free(owner);
}

UmiDebugRuntimeAdapter *umi_debug_runtime_contract_adapter_connection(
    UmiDebugRuntimeContractAdapter *owner)
{
    return owner != NULL ? owner->adapter : NULL;
}
