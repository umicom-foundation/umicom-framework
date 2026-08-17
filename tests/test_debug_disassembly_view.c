/*-----------------------------------------------------------------------------
 * Umicom Framework disassembly view tests.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/debug/disassembly_view.h"

static UmiDebugInstruction make_instruction(
    const char *id,
    uint64_t address,
    const char *mnemonic,
    int current)
{
    UmiDebugInstruction instruction;
    (void)memset(&instruction, 0, sizeof(instruction));
    instruction.struct_size = (uint32_t)sizeof(instruction);
    instruction.api_version = UMI_DEBUG_DISASSEMBLY_VIEW_API_VERSION;
    (void)snprintf(instruction.id, sizeof(instruction.id), "%s", id);
    (void)snprintf(instruction.session_id,
                   sizeof(instruction.session_id), "%s", "session-1");
    (void)snprintf(instruction.memory_reference,
                   sizeof(instruction.memory_reference), "%s", "code");
    (void)snprintf(instruction.instruction_bytes,
                   sizeof(instruction.instruction_bytes), "%s", "90");
    (void)snprintf(instruction.mnemonic,
                   sizeof(instruction.mnemonic), "%s", mnemonic);
    instruction.address = address;
    instruction.valid = 1;
    instruction.current = current;
    instruction.can_breakpoint = 1;
    return instruction;
}

int main(void)
{
    UmiDebugDisassemblyView *view = NULL;
    UmiDebugInstruction instructions[3];
    UmiDebugInstruction instruction;
    UmiDebugDisassemblySnapshot snapshot;

    assert(umi_debug_disassembly_view_create(&view) == UMI_STATUS_OK);
    instructions[0] = make_instruction("i2", 0x1002U, "ret", 0);
    instructions[1] = make_instruction("i0", 0x1000U, "push", 1);
    instructions[1].has_source = 1;
    (void)snprintf(instructions[1].source_uri,
                   sizeof(instructions[1].source_uri), "%s", "file:///main.c");
    instructions[1].source_line = 10U;
    instructions[2] = make_instruction("i1", 0x1001U, "nop", 0);
    assert(umi_debug_disassembly_view_load(view, instructions, 3U) ==
           UMI_STATUS_OK);
    assert(umi_debug_disassembly_view_at(view, 0U, &instruction) ==
           UMI_STATUS_OK);
    assert(strcmp(instruction.id, "i0") == 0);
    assert(umi_debug_disassembly_view_current(view, &instruction) ==
           UMI_STATUS_OK);
    assert(strcmp(instruction.id, "i0") == 0);
    {
        const uint64_t instruction_sequence = instruction.sequence;
        instruction.sequence = 0U;
        (void)snprintf(instruction.comment, sizeof(instruction.comment), "%s",
                       "function prologue");
        assert(umi_debug_disassembly_view_upsert(view, &instruction) ==
               UMI_STATUS_OK);
        assert(umi_debug_disassembly_view_find(view, "i0", &instruction) ==
               UMI_STATUS_OK);
        assert(instruction.sequence == instruction_sequence);
    }

    assert(umi_debug_disassembly_view_select_address(view, 0x1001U) ==
           UMI_STATUS_OK);
    assert(umi_debug_disassembly_view_selected(view, &instruction) ==
           UMI_STATUS_OK);
    assert(strcmp(instruction.id, "i1") == 0);
    assert(umi_debug_disassembly_view_set_breakpoint(view, "i1", 1) ==
           UMI_STATUS_OK);
    assert(umi_debug_disassembly_view_set_current(view, "i2") ==
           UMI_STATUS_OK);

    assert(umi_debug_disassembly_view_snapshot(view, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.instruction_count == 3U);
    assert(snapshot.source_correlated_count == 1U);
    assert(snapshot.breakpoint_count == 1U);
    assert(strcmp(snapshot.current_instruction_id, "i2") == 0);
    assert(snapshot.minimum_address == 0x1000U);
    assert(snapshot.maximum_address == 0x1002U);

    instruction = make_instruction("sequence-invalid", 0x2000U, "nop", 0);
    instruction.sequence = UINT64_MAX;
    assert(umi_debug_disassembly_view_upsert(view, &instruction) ==
           UMI_STATUS_INVALID_ARGUMENT);
    instruction = make_instruction("sequence-limit", 0x2001U, "nop", 0);
    instruction.sequence = UINT64_MAX - 1U;
    assert(umi_debug_disassembly_view_upsert(view, &instruction) ==
           UMI_STATUS_OK);
    instruction = make_instruction("sequence-overflow", 0x2002U, "nop", 0);
    assert(umi_debug_disassembly_view_upsert(view, &instruction) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    umi_debug_disassembly_view_destroy(view);
    return 0;
}
