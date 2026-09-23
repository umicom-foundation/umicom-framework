/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_workbench_service.c
 *
 * PURPOSE:
 *   Compose canonical debug runtime state into one toolkit-neutral debugger workbench service.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/workbench/debug_workbench_service.h"
#include "umicom/debug_runtime/decoders/breakpoints.h"
#include "umicom/debug_runtime/requests/set_instruction_breakpoints.h"
#include "umicom/debug_runtime/request_support.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Initialise debug workbench debug workbench service from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_workbench_service_init(UmiDebugWorkbenchDebugWorkbenchService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    memset(service, 0, sizeof *service);
    umi_debug_workbench_debug_session_registry_init(&service->sessions);
    umi_debug_workbench_breakpoint_collection_init(&service->breakpoints);
    umi_debug_workbench_thread_group_init(&service->threads);
    umi_debug_workbench_watch_expression_collection_init(&service->watches);
    service->revision = 1U;
}

/*
 * Provide the debug workbench debug workbench service open session operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workbench_service_open_session(UmiDebugWorkbenchDebugWorkbenchService *service, const UmiDebugWorkbenchEntry *session)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || !umi_debug_workbench_entry_valid(session)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_workbench_debug_session_registry_add(&service->sessions, session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_workbench_debug_session_registry_select(&service->sessions, session->id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_workbench_debug_status_model_init(&service->status, session->id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    service->status_initialized = true;
    service->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench debug workbench service select session operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workbench_service_select_session(UmiDebugWorkbenchDebugWorkbenchService *service, const char *session_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_workbench_debug_session_registry_select(&service->sessions, session_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_workbench_debug_status_model_init(&service->status, session_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    service->status_initialized = true;
    service->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench debug workbench service refresh status operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workbench_service_refresh_status(UmiDebugWorkbenchDebugWorkbenchService *service, UmiDebugWorkbenchSessionPhase phase, const char *stop_reason, uint32_t frame_count, uint32_t variable_count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || !service->status_initialized) return UMI_STATUS_INVALID_STATE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_debug_workbench_debug_status_model_update(&service->status, phase, stop_reason, (uint32_t)service->threads.count, frame_count, variable_count) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    service->revision++;
    return UMI_STATUS_OK;
}

/*
 * Check that debug workbench debug workbench service satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_workbench_service_valid(const UmiDebugWorkbenchDebugWorkbenchService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || service->revision == 0U) return 0;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_debug_workbench_debug_session_registry_valid(&service->sessions)) return 0;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_debug_workbench_breakpoint_collection_valid(&service->breakpoints)) return 0;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_debug_workbench_thread_group_valid(&service->threads)) return 0;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_debug_workbench_watch_expression_collection_valid(&service->watches)) return 0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (service->status_initialized && !umi_debug_workbench_debug_status_model_valid(&service->status)) return 0;
    return 1;
}

/* -------------------------------------------------------------------------
 * Low-level C and Assembly inspection
 *
 * The debugger runtime and adapter remain authoritative.  This workbench layer
 * adds reusable projection and orchestration only; applications do not parse
 * DAP JSON, inspect register scopes, or construct disassembly records.
 * ------------------------------------------------------------------------- */

static int ascii_equal_folded(const char *left, const char *right)
{
    if (left == NULL || right == NULL) return 0;
    while (*left != '\0' && *right != '\0') {
        char a = *left++;
        char b = *right++;
        if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
        if (b >= 'A' && b <= 'Z') b = (char)(b - 'A' + 'a');
        if (a != b) return 0;
    }
    return *left == '\0' && *right == '\0';
}

static int ascii_contains_folded(const char *text, const char *needle)
{
    const char *start;
    if (text == NULL || needle == NULL || needle[0] == '\0') return 0;
    for (start = text; *start != '\0'; ++start) {
        const char *left = start;
        const char *right = needle;
        while (*left != '\0' && *right != '\0') {
            char a = *left;
            char b = *right;
            if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
            if (b >= 'A' && b <= 'Z') b = (char)(b - 'A' + 'a');
            if (a != b) break;
            ++left;
            ++right;
        }
        if (*right == '\0') return 1;
    }
    return 0;
}


static UmiStatus copy_bounded_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || source == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static int program_counter_name(const char *name)
{
    static const char *const NAMES[] = {
        "pc", "$pc", "rip", "$rip", "eip", "$eip", "ip", "$ip",
        "program counter", "instruction pointer"
    };
    size_t index;
    if (name == NULL) return 0;
    for (index = 0U; index < sizeof(NAMES) / sizeof(NAMES[0]); ++index) {
        if (ascii_equal_folded(name, NAMES[index])) return 1;
    }
    return 0;
}

static UmiStatus copy_reference_token(const char *text,
                                      char *out_reference,
                                      size_t capacity)
{
    const char *start;
    size_t length = 0U;
    if (text == NULL || out_reference == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    start = text;
    while (*start == ' ' || *start == '\t') ++start;
    if (*start == '\0') return UMI_STATUS_NOT_FOUND;
    while (start[length] != '\0' && start[length] != ' ' &&
           start[length] != '\t' && start[length] != '\r' &&
           start[length] != '\n') {
        ++length;
    }
    if (length == 0U) return UMI_STATUS_NOT_FOUND;
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_reference, start, length);
    out_reference[length] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_program_counter_reference(
    const UmiDebugRegisterBank *bank,
    char *out_reference,
    size_t capacity)
{
    size_t index;
    UmiDebugRegisterValue item;
    if (bank == NULL || out_reference == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    out_reference[0] = '\0';
    for (index = 0U; index < umi_debug_register_bank_register_count(bank); ++index) {
        if (umi_debug_register_bank_register_at(bank, index, &item) != UMI_STATUS_OK)
            continue;
        if (!program_counter_name(item.name) &&
            !program_counter_name(item.alternate_name)) {
            continue;
        }
        if (item.raw_value[0] != '\0')
            return copy_reference_token(item.raw_value, out_reference, capacity);
        return copy_reference_token(item.value, out_reference, capacity);
    }
    return UMI_STATUS_NOT_FOUND;
}

static UmiStatus copy_register_bank(
    UmiDebugRegisterBank *destination,
    const UmiDebugRegisterBank *source)
{
    size_t index;
    UmiStatus status;
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_register_bank_clear(destination);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < umi_debug_register_bank_group_count(source); ++index) {
        UmiDebugRegisterGroup group;
        status = umi_debug_register_bank_group_at(source, index, &group);
        if (status != UMI_STATUS_OK) return status;
        status = umi_debug_register_bank_upsert_group(destination, &group);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < umi_debug_register_bank_register_count(source); ++index) {
        UmiDebugRegisterValue value;
        status = umi_debug_register_bank_register_at(source, index, &value);
        if (status != UMI_STATUS_OK) return status;
        status = umi_debug_register_bank_upsert_register(destination, &value);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

static UmiStatus refresh_register_bank(
    UmiDebugRuntimePlatform *platform,
    uint64_t frame_id,
    uint32_t timeout_ms,
    UmiDebugRegisterBank *destination)
{
    UmiDebugRuntimeAdapter *adapter;
    UmiDebugRuntimeEnvelope response;
    UmiDebugRuntimeScopeList scopes;
    UmiDebugRegisterBank *staged = NULL;
    UmiDebugRuntimePlatformSnapshot platform_snapshot;
    uint64_t sequence = 0U;
    size_t scope_index;
    size_t group_order = 0U;
    size_t register_scope_count = 0U;
    UmiStatus status;

    if (platform == NULL || destination == NULL || timeout_ms == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    adapter = umi_debug_runtime_platform_adapter(platform);
    if (adapter == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_debug_runtime_platform_snapshot(platform, &platform_snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (!platform_snapshot.active || !platform_snapshot.paused)
        return UMI_STATUS_INVALID_STATE;

    status = umi_debug_register_bank_create(&staged);
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_runtime_request_scopes(adapter, frame_id, &sequence);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_adapter_wait_response(
            adapter, sequence, timeout_ms, &response);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_decode_scopes(response.json, &scopes);

    for (scope_index = 0U;
         status == UMI_STATUS_OK && scope_index < scopes.count;
         ++scope_index) {
        UmiDebugRuntimeVariableList variables;
        UmiDebugRegisterGroup group;
        UmiDebugRuntimeEnvelope variable_response;
        uint64_t variable_sequence = 0U;
        size_t variable_index;

        if (!ascii_contains_folded(scopes.items[scope_index].name, "register") ||
            scopes.items[scope_index].variables_reference == 0U) {
            continue;
        }
        ++register_scope_count;
        status = umi_debug_runtime_request_variables(
            adapter, scopes.items[scope_index].variables_reference,
            &variable_sequence);
        if (status == UMI_STATUS_OK)
            status = umi_debug_runtime_adapter_wait_response(
                adapter, variable_sequence, timeout_ms, &variable_response);
        if (status == UMI_STATUS_OK)
            status = umi_debug_runtime_decode_variables(
                variable_response.json, &variables);
        if (status != UMI_STATUS_OK) break;

        (void)memset(&group, 0, sizeof(group));
        group.struct_size = (uint32_t)sizeof(group);
        group.api_version = UMI_DEBUG_REGISTER_BANK_API_VERSION;
        (void)snprintf(group.id, sizeof(group.id),
                       "register-group.%zu", scope_index);
        (void)snprintf(group.session_id, sizeof(group.session_id), "%s",
                       platform_snapshot.active_session_id);
        (void)snprintf(group.frame_id, sizeof(group.frame_id), "%llu",
                       (unsigned long long)frame_id);
        status = copy_bounded_text(
            group.name, sizeof(group.name), scopes.items[scope_index].name);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(group.description, sizeof(group.description), "%s",
                       "Register scope exposed by the active debug adapter.");
        group.order = (int32_t)group_order++;
        group.sequence = platform_snapshot.revision + scope_index + 1U;
        group.revision = group.sequence;
        group.expanded = 1;
        group.available = 1;
        status = umi_debug_register_bank_upsert_group(staged, &group);
        if (status != UMI_STATUS_OK) break;

        for (variable_index = 0U;
             status == UMI_STATUS_OK && variable_index < variables.count;
             ++variable_index) {
            UmiDebugRegisterValue register_value;
            UmiDebugRegisterValue previous;
            (void)memset(&register_value, 0, sizeof(register_value));
            register_value.struct_size = (uint32_t)sizeof(register_value);
            register_value.api_version = UMI_DEBUG_REGISTER_BANK_API_VERSION;
            (void)snprintf(register_value.id, sizeof(register_value.id),
                           "register.%zu.%zu", scope_index, variable_index);
            (void)snprintf(register_value.group_id,
                           sizeof(register_value.group_id), "%s", group.id);
            status = copy_bounded_text(
                register_value.name, sizeof(register_value.name),
                variables.items[variable_index].name);
            if (status != UMI_STATUS_OK) break;
            status = copy_bounded_text(
                register_value.value, sizeof(register_value.value),
                variables.items[variable_index].value);
            if (status != UMI_STATUS_OK) break;
            status = copy_bounded_text(
                register_value.raw_value, sizeof(register_value.raw_value),
                variables.items[variable_index].value);
            if (status != UMI_STATUS_OK) break;
            status = copy_bounded_text(
                register_value.type, sizeof(register_value.type),
                variables.items[variable_index].type);
            if (status != UMI_STATUS_OK) break;
            (void)snprintf(register_value.format,
                           sizeof(register_value.format), "%s", "adapter");
            register_value.order = (int32_t)variable_index;
            register_value.sequence =
                platform_snapshot.revision + variable_index + 1U;
            register_value.revision = register_value.sequence;
            register_value.changed =
                umi_debug_register_bank_find_register(
                    destination, register_value.id, &previous) == UMI_STATUS_OK &&
                strcmp(previous.value, register_value.value) != 0;
            register_value.read_only = 1;
            register_value.available = 1;
            status = umi_debug_register_bank_upsert_register(
                staged, &register_value);
        }
    }

    if (status == UMI_STATUS_OK && register_scope_count == 0U)
        status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK)
        status = copy_register_bank(destination, staged);
    umi_debug_register_bank_destroy(staged);
    return status;
}

UmiStatus umi_debug_workbench_refresh_low_level(
    UmiDebugRuntimePlatform *platform,
    uint32_t timeout_ms,
    uint32_t instruction_count)
{
    UmiDebugRuntimePlatformSnapshot snapshot;
    UmiDebugAdvancedPlatform *advanced;
    UmiDebugInspectionSession *inspection;
    UmiDebugRegisterBank *bank;
    UmiDebugDisassemblyView *disassembly;
    UmiDebugRuntimeDisassembly result;
    char memory_reference[UMI_DEBUG_REGISTER_VALUE_CAPACITY];
    UmiStatus status;

    if (platform == NULL || timeout_ms == 0U || instruction_count == 0U ||
        instruction_count > UMI_DEBUG_WORKBENCH_LOW_LEVEL_INSTRUCTION_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_runtime_platform_snapshot(platform, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (!snapshot.active || !snapshot.paused) return UMI_STATUS_INVALID_STATE;

    advanced = umi_debug_runtime_platform_advanced(platform);
    inspection = advanced != NULL
        ? umi_debug_advanced_platform_inspection(advanced) : NULL;
    bank = inspection != NULL
        ? umi_debug_inspection_session_registers(inspection) : NULL;
    disassembly = inspection != NULL
        ? umi_debug_inspection_session_disassembly(inspection) : NULL;
    if (bank == NULL || disassembly == NULL) return UMI_STATUS_INVALID_STATE;

    /* The workbench path accepts DAP frame identifier zero.  Existing platform
     * refresh APIs remain untouched for compatibility; this orchestration uses
     * the public request/decoder contracts directly and publishes only after a
     * complete register snapshot has been staged. */
    status = refresh_register_bank(
        platform, snapshot.active_frame_id, timeout_ms, bank);
    if (status != UMI_STATUS_OK) return status;

    if (!snapshot.capabilities.supports_disassemble_request) {
        (void)umi_debug_disassembly_view_clear(disassembly);
        return UMI_STATUS_OK;
    }
    status = umi_debug_workbench_program_counter_reference(
        bank, memory_reference, sizeof(memory_reference));
    if (status == UMI_STATUS_NOT_FOUND) {
        (void)umi_debug_disassembly_view_clear(disassembly);
        return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK) return status;

    return umi_debug_runtime_platform_disassemble(
        platform, memory_reference, 0, 0, instruction_count,
        timeout_ms, &result);
}

UmiStatus umi_debug_workbench_instruction_reference(
    const UmiDebugDisassemblyView *disassembly,
    char *out_reference,
    size_t capacity)
{
    UmiDebugInstruction instruction;
    UmiStatus status;

    if (disassembly == NULL || out_reference == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    out_reference[0] = '\0';
    status = umi_debug_disassembly_view_selected(disassembly, &instruction);
    if (status == UMI_STATUS_NOT_FOUND)
        status = umi_debug_disassembly_view_current(disassembly, &instruction);
    if (status != UMI_STATUS_OK) return status;
    if (!instruction.valid || instruction.memory_reference[0] == '\0')
        return UMI_STATUS_NOT_FOUND;
    return copy_bounded_text(out_reference, capacity,
                             instruction.memory_reference);
}

UmiStatus umi_debug_workbench_step_instruction(
    UmiDebugRuntimePlatform *platform,
    int step_into,
    uint32_t timeout_ms)
{
    UmiDebugRuntimePlatformSnapshot snapshot;
    UmiDebugRuntimeAdapter *adapter;
    UmiDebugRuntimeEnvelope response;
    UmiDebugAdvancedPlatform *advanced;
    UmiDebugInspectionSession *inspection;
    UmiDebugDisassemblyView *disassembly;
    char arguments[192];
    uint64_t sequence = 0U;
    int written;
    UmiStatus status;

    if (platform == NULL || timeout_ms == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_runtime_platform_snapshot(platform, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (!snapshot.active || !snapshot.paused ||
        snapshot.adapter.state != UMI_DEBUG_RUNTIME_ADAPTER_PAUSED ||
        snapshot.active_thread_id == 0U)
        return UMI_STATUS_INVALID_STATE;
    adapter = umi_debug_runtime_platform_adapter(platform);
    if (adapter == NULL) return UMI_STATUS_INVALID_STATE;

    written = snprintf(arguments, sizeof(arguments),
        "{\"threadId\":%llu,\"granularity\":\"instruction\"}",
        (unsigned long long)snapshot.active_thread_id);
    if (written < 0 || (size_t)written >= sizeof(arguments))
        return UMI_STATUS_CAPACITY_EXCEEDED;

    status = umi_debug_runtime_request_raw(
        adapter, step_into ? "stepIn" : "next", arguments,
        "instruction", &sequence);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_adapter_wait_response(
            adapter, sequence, timeout_ms, &response);
    if (status != UMI_STATUS_OK) return status;
    if (!response.success) return UMI_STATUS_UNAVAILABLE;

    /* The stopped/continued event pump remains authoritative for the platform's
     * eventual pause state.  Mark the public adapter/inspection models running
     * immediately so frontends cannot issue a second instruction step while the
     * first one is in flight. */
    (void)umi_debug_runtime_adapter_set_state(
        adapter, UMI_DEBUG_RUNTIME_ADAPTER_RUNNING);
    advanced = umi_debug_runtime_platform_advanced(platform);
    inspection = advanced != NULL
        ? umi_debug_advanced_platform_inspection(advanced) : NULL;
    if (inspection != NULL) {
        (void)umi_debug_inspection_session_set_state(
            inspection, UMI_DEBUG_INSPECTION_RUNNING);
        disassembly = umi_debug_inspection_session_disassembly(inspection);
        if (disassembly != NULL) (void)umi_debug_disassembly_view_clear(disassembly);
    }
    return UMI_STATUS_OK;
}

static uint64_t instruction_reference_hash(const char *text)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    if (text == NULL) return hash;
    while (*text != '\0') {
        hash ^= (unsigned char)*text++;
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

static int same_instruction_breakpoint(
    const UmiDebugAdvancedBreakpoint *breakpoint,
    const UmiDebugInstruction *instruction)
{
    if (breakpoint == NULL || instruction == NULL ||
        breakpoint->kind != UMI_DEBUG_ADVANCED_BREAKPOINT_INSTRUCTION)
        return 0;
    if (instruction->memory_reference[0] != '\0' &&
        strcmp(breakpoint->reference, instruction->memory_reference) == 0 &&
        breakpoint->instruction_offset == instruction->instruction_offset)
        return 1;
    return instruction->address != 0U && breakpoint->address == instruction->address;
}

UmiStatus umi_debug_workbench_toggle_instruction_breakpoint(
    UmiDebugRuntimePlatform *platform,
    uint32_t timeout_ms,
    int *out_enabled)
{
    UmiDebugRuntimePlatformSnapshot snapshot;
    UmiDebugRuntimeAdapter *adapter;
    UmiDebugAdvancedPlatform *advanced;
    UmiDebugInspectionSession *inspection;
    UmiDebugDisassemblyView *disassembly;
    UmiDebugAdvancedBreakpointRegistry *registry;
    UmiDebugInstruction instruction;
    UmiDebugAdvancedBreakpoint existing;
    UmiDebugRuntimeInstructionBreakpoint *requests = NULL;
    UmiDebugRuntimeBreakpointList decoded;
    UmiDebugRuntimeEnvelope response;
    size_t registry_count;
    size_t index;
    size_t request_count = 0U;
    size_t target_response_index = SIZE_MAX;
    int found = 0;
    uint64_t sequence = 0U;
    UmiStatus status;

    if (platform == NULL || timeout_ms == 0U || out_enabled == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_enabled = 0;
    status = umi_debug_runtime_platform_snapshot(platform, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (!snapshot.active || !snapshot.paused ||
        snapshot.adapter.state != UMI_DEBUG_RUNTIME_ADAPTER_PAUSED)
        return UMI_STATUS_INVALID_STATE;
    if (!snapshot.capabilities.supports_instruction_breakpoints)
        return UMI_STATUS_NOT_IMPLEMENTED;
    adapter = umi_debug_runtime_platform_adapter(platform);
    advanced = umi_debug_runtime_platform_advanced(platform);
    inspection = advanced != NULL
        ? umi_debug_advanced_platform_inspection(advanced) : NULL;
    disassembly = inspection != NULL
        ? umi_debug_inspection_session_disassembly(inspection) : NULL;
    registry = inspection != NULL
        ? umi_debug_inspection_session_advanced_breakpoints(inspection) : NULL;
    if (adapter == NULL || disassembly == NULL || registry == NULL)
        return UMI_STATUS_INVALID_STATE;

    status = umi_debug_disassembly_view_selected(disassembly, &instruction);
    if (status == UMI_STATUS_NOT_FOUND)
        status = umi_debug_disassembly_view_current(disassembly, &instruction);
    if (status != UMI_STATUS_OK) return status;
    if (!instruction.valid || !instruction.can_breakpoint ||
        instruction.memory_reference[0] == '\0')
        return UMI_STATUS_NOT_IMPLEMENTED;

    registry_count = umi_debug_advanced_breakpoint_registry_count(registry);
    requests = (UmiDebugRuntimeInstructionBreakpoint *)calloc(
        registry_count + 1U, sizeof(*requests));
    if (requests == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    for (index = 0U; index < registry_count; ++index) {
        UmiDebugAdvancedBreakpoint item;
        status = umi_debug_advanced_breakpoint_registry_at(registry, index, &item);
        if (status != UMI_STATUS_OK) break;
        if (item.kind != UMI_DEBUG_ADVANCED_BREAKPOINT_INSTRUCTION)
            continue;
        if (same_instruction_breakpoint(&item, &instruction)) {
            existing = item;
            found = 1;
            continue; /* toggle-off request omits the selected breakpoint */
        }
        if (!item.enabled) continue;
        status = copy_bounded_text(
            requests[request_count].instruction_reference,
            sizeof(requests[request_count].instruction_reference),
            item.reference);
        if (status != UMI_STATUS_OK) break;
        requests[request_count].offset = item.instruction_offset;
        status = copy_bounded_text(requests[request_count].condition,
            sizeof(requests[request_count].condition), item.condition);
        if (status != UMI_STATUS_OK) break;
        status = copy_bounded_text(requests[request_count].hit_condition,
            sizeof(requests[request_count].hit_condition), item.hit_condition);
        if (status != UMI_STATUS_OK) break;
        ++request_count;
    }

    if (status == UMI_STATUS_OK && !found) {
        target_response_index = request_count;
        status = copy_bounded_text(
            requests[request_count].instruction_reference,
            sizeof(requests[request_count].instruction_reference),
            instruction.memory_reference);
        if (status == UMI_STATUS_OK) {
            requests[request_count].offset = instruction.instruction_offset;
            ++request_count;
        }
    }

    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_request_set_instruction_breakpoints(
            adapter, requests, request_count, &sequence);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_adapter_wait_response(
            adapter, sequence, timeout_ms, &response);
    if (status == UMI_STATUS_OK && !response.success)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_decode_breakpoints(response.json, &decoded);
    if (status == UMI_STATUS_OK && decoded.count != request_count)
        status = UMI_STATUS_PARSE_ERROR;

    if (status == UMI_STATUS_OK && found) {
        status = umi_debug_advanced_breakpoint_registry_remove(
            registry, existing.id);
        if (status == UMI_STATUS_OK)
            status = umi_debug_disassembly_view_set_breakpoint(
                disassembly, instruction.id, 0);
        *out_enabled = 0;
    } else if (status == UMI_STATUS_OK) {
        UmiDebugAdvancedBreakpoint item;
        (void)memset(&item, 0, sizeof(item));
        item.struct_size = (uint32_t)sizeof(item);
        item.api_version = UMI_DEBUG_ADVANCED_BREAKPOINT_API_VERSION;
        (void)snprintf(item.id, sizeof(item.id), "instruction.%016llx",
            (unsigned long long)(instruction.address != 0U
                ? instruction.address
                : instruction_reference_hash(instruction.memory_reference)));
        (void)snprintf(item.session_id, sizeof(item.session_id), "%s",
            snapshot.active_session_id);
        item.kind = UMI_DEBUG_ADVANCED_BREAKPOINT_INSTRUCTION;
        (void)snprintf(item.name, sizeof(item.name), "%.120s %.300s",
            instruction.mnemonic, instruction.operands);
        status = copy_bounded_text(item.reference, sizeof(item.reference),
                                   instruction.memory_reference);
        item.address = instruction.address;
        item.instruction_offset = instruction.instruction_offset;
        item.sequence = snapshot.revision + 1U;
        item.revision = item.sequence;
        item.enabled = 1;
        item.verified = target_response_index < decoded.count
            ? decoded.items[target_response_index].verified : 0;
        item.stop_on_hit = 1;
        if (status == UMI_STATUS_OK)
            status = umi_debug_advanced_breakpoint_registry_upsert(
                registry, &item);
        if (status == UMI_STATUS_OK)
            status = umi_debug_disassembly_view_set_breakpoint(
                disassembly, instruction.id, 1);
        *out_enabled = status == UMI_STATUS_OK;
    }

    free(requests);
    return status;
}

static UmiStatus set_string(UmiUiViewModel *view,
                            const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value,
                                               text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_integer(UmiUiViewModel *view,
                             const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus create_low_level_view(const char *view_id,
                                       const char *view_type,
                                       const char *title,
                                       const char *summary,
                                       UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (view_id == NULL || view_type == NULL || out_view == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    status = umi_ui_view_model_create(view_id, view_type,
                                      UMI_UI_ROLE_PANE, out_view);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "title", title);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "summary", summary);
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

UmiStatus umi_debug_workbench_registers_view_create(
    const char *view_id,
    const UmiDebugRegisterBank *bank,
    UmiUiViewModel **out_view)
{
    UmiDebugRegisterBankSnapshot snapshot;
    size_t count;
    size_t index;
    UmiStatus status = create_low_level_view(
        view_id, "umicom.debug-registers", "Registers",
        "Architecture registers for the selected stack frame.", out_view);
    if (status != UMI_STATUS_OK) return status;
    if (bank == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_register_bank_snapshot(bank, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "debug.register.group-count",
                             (int64_t)snapshot.group_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "debug.register.count",
                             (int64_t)snapshot.register_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "debug.register.changed-count",
                             (int64_t)snapshot.changed_register_count);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "debug.register.selected",
                            snapshot.selected_register_id);
    count = snapshot.visible_register_count;
    if (count > UMI_DEBUG_WORKBENCH_LOW_LEVEL_ROW_LIMIT)
        count = UMI_DEBUG_WORKBENCH_LOW_LEVEL_ROW_LIMIT;
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "debug.register.row-count",
                             (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiDebugRegisterValue value;
        char key[64];
        char text[UMI_UI_VALUE_STRING_CAPACITY];
        status = umi_debug_register_bank_visible_register_at(bank, index, &value);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "debug.register.row.%02zu", index);
        (void)snprintf(text, sizeof(text), "%s%.120s = %.300s%s%.64s",
                       value.changed ? "* " : "", value.name, value.value,
                       value.type[0] != '\0' ? " : " : "", value.type);
        status = set_string(*out_view, key, text);
    }
    return status;
}

UmiStatus umi_debug_workbench_disassembly_view_create(
    const char *view_id,
    const UmiDebugDisassemblyView *disassembly,
    UmiUiViewModel **out_view)
{
    UmiDebugDisassemblySnapshot snapshot;
    size_t count;
    size_t index;
    UmiStatus status = create_low_level_view(
        view_id, "umicom.debug-disassembly", "Disassembly",
        "Machine instructions from the active program counter.", out_view);
    if (status != UMI_STATUS_OK) return status;
    if (disassembly == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_disassembly_view_snapshot(disassembly, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "debug.disassembly.instruction-count",
                             (int64_t)snapshot.instruction_count);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "debug.disassembly.current",
                            snapshot.current_instruction_id);
    count = snapshot.instruction_count;
    if (count > UMI_DEBUG_WORKBENCH_LOW_LEVEL_ROW_LIMIT)
        count = UMI_DEBUG_WORKBENCH_LOW_LEVEL_ROW_LIMIT;
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "debug.disassembly.row-count",
                             (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiDebugInstruction instruction;
        char key[64];
        char text[UMI_UI_VALUE_STRING_CAPACITY];
        status = umi_debug_disassembly_view_at(disassembly, index, &instruction);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "debug.disassembly.row.%02zu", index);
        (void)snprintf(text, sizeof(text),
                       "%s0x%llx  %.82s  %.100s %.190s",
                       instruction.current ? "> " : "  ",
                       (unsigned long long)instruction.address,
                       instruction.instruction_bytes,
                       instruction.mnemonic,
                       instruction.operands);
        status = set_string(*out_view, key, text);
    }
    return status;
}
