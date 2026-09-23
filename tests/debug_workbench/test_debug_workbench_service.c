/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_workbench_service.c
 *
 * PURPOSE:
 *   Verify compose canonical debug runtime state into one toolkit-neutral debugger workbench service.
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
#include "umicom/ui/value.h"

#include <string.h>

#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

static int low_level_projection_check(void)
{
    UmiDebugRegisterBank *bank = NULL;
    UmiDebugDisassemblyView *disassembly = NULL;
    UmiUiViewModel *register_view = NULL;
    UmiUiViewModel *disassembly_view = NULL;
    UmiDebugRegisterGroup group = {0};
    UmiDebugRegisterValue pc = {0};
    UmiDebugRegisterValue accumulator = {0};
    UmiDebugInstruction instruction = {0};
    UmiUiValue value;
    char reference[64];

    UMI_TEST_CHECK(umi_debug_register_bank_create(&bank) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_disassembly_view_create(&disassembly) == UMI_STATUS_OK);

    group.struct_size = (uint32_t)sizeof(group);
    group.api_version = UMI_DEBUG_REGISTER_BANK_API_VERSION;
    (void)strcpy(group.id, "register-group.general");
    (void)strcpy(group.session_id, "notes.debug");
    (void)strcpy(group.frame_id, "0");
    (void)strcpy(group.name, "General Registers");
    group.available = 1;
    group.expanded = 1;
    UMI_TEST_CHECK(umi_debug_register_bank_upsert_group(bank, &group) ==
                   UMI_STATUS_OK);

    pc.struct_size = (uint32_t)sizeof(pc);
    pc.api_version = UMI_DEBUG_REGISTER_BANK_API_VERSION;
    (void)strcpy(pc.id, "register.pc");
    (void)strcpy(pc.group_id, group.id);
    (void)strcpy(pc.name, "RIP");
    (void)strcpy(pc.value, "0x401020 <UmiNotesCountByte+4>");
    (void)strcpy(pc.raw_value, "0x401020");
    (void)strcpy(pc.type, "uint64");
    pc.available = 1;
    pc.read_only = 1;
    UMI_TEST_CHECK(umi_debug_register_bank_upsert_register(bank, &pc) ==
                   UMI_STATUS_OK);

    accumulator = pc;
    (void)strcpy(accumulator.id, "register.rax");
    (void)strcpy(accumulator.name, "RAX");
    (void)strcpy(accumulator.value, "0x0000000000000005");
    (void)strcpy(accumulator.raw_value, "0x5");
    accumulator.changed = 1;
    UMI_TEST_CHECK(umi_debug_register_bank_upsert_register(bank, &accumulator) ==
                   UMI_STATUS_OK);

    UMI_TEST_CHECK(umi_debug_workbench_program_counter_reference(
                       bank, reference, sizeof(reference)) == UMI_STATUS_OK);
    UMI_TEST_CHECK(strcmp(reference, "0x401020") == 0);

    UMI_TEST_CHECK(umi_debug_workbench_registers_view_create(
                       "test.registers", bank, &register_view) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_ui_view_model_get_property(
                       register_view, "debug.register.row-count", &value) ==
                   UMI_STATUS_OK);
    UMI_TEST_CHECK(value.kind == UMI_UI_VALUE_INTEGER);
    UMI_TEST_CHECK(value.integer_value == 2);
    UMI_TEST_CHECK(umi_ui_view_model_get_property(
                       register_view, "debug.register.row.01", &value) ==
                   UMI_STATUS_OK);
    UMI_TEST_CHECK(value.kind == UMI_UI_VALUE_STRING);
    UMI_TEST_CHECK(strstr(value.string_value, "RAX") != NULL);

    instruction.struct_size = (uint32_t)sizeof(instruction);
    instruction.api_version = UMI_DEBUG_DISASSEMBLY_VIEW_API_VERSION;
    (void)strcpy(instruction.id, "instruction.0");
    (void)strcpy(instruction.session_id, "notes.debug");
    (void)strcpy(instruction.memory_reference, "0x401020");
    (void)strcpy(instruction.instruction_bytes, "48 83 c0 01");
    (void)strcpy(instruction.mnemonic, "add");
    (void)strcpy(instruction.operands, "$0x1, %rax");
    instruction.address = 0x401020U;
    instruction.valid = 1;
    instruction.current = 1;
    instruction.can_breakpoint = 1;
    UMI_TEST_CHECK(umi_debug_disassembly_view_upsert(
                       disassembly, &instruction) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_disassembly_view_set_current(
                       disassembly, instruction.id) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_disassembly_view_select(
                       disassembly, instruction.id) == UMI_STATUS_OK);

    UMI_TEST_CHECK(umi_debug_workbench_disassembly_view_create(
                       "test.disassembly", disassembly,
                       &disassembly_view) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_ui_view_model_get_property(
                       disassembly_view, "debug.disassembly.row.00", &value) ==
                   UMI_STATUS_OK);
    UMI_TEST_CHECK(value.kind == UMI_UI_VALUE_STRING);
    UMI_TEST_CHECK(strstr(value.string_value, "add") != NULL);
    UMI_TEST_CHECK(strstr(value.string_value, "%rax") != NULL);

    umi_ui_view_model_destroy(disassembly_view);
    umi_ui_view_model_destroy(register_view);
    umi_debug_disassembly_view_destroy(disassembly);
    umi_debug_register_bank_destroy(bank);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugWorkbenchDebugWorkbenchService service;
    UmiDebugWorkbenchEntry session;
    UmiDebugWorkbenchRange range = {{0U, 0U}, {0U, 0U}};
    umi_debug_workbench_debug_workbench_service_init(&service);
    UMI_TEST_CHECK(umi_debug_workbench_entry_init(&session, "session-main", "Main debug session", "gdb-dap", NULL, range) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workbench_service_open_session(&service, &session) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workbench_service_refresh_status(&service, UMI_DEBUG_WORKBENCH_SESSION_PAUSED, "breakpoint", 3U, 12U) == UMI_STATUS_OK);
    UMI_TEST_CHECK(service.status.phase == UMI_DEBUG_WORKBENCH_SESSION_PAUSED);
    UMI_TEST_CHECK(service.status.frame_count == 3U);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workbench_service_valid(&service));
    UMI_TEST_CHECK(low_level_projection_check() == 0);
    return 0;
}
