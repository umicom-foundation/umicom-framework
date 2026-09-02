/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/disassembly_view.h
 *
 * PURPOSE:
 *   Represent adapter-neutral disassembled instructions, source correlation,
 *   current-instruction state and instruction-breakpoint decoration.
 *
 * ARCHITECTURE:
 *   Adapters decode architecture-specific instruction data. The Framework owns
 *   the deterministic model consumed by GTK4, Qt, Wt, headless tests and future
 *   frontends. No machine-code decoder or vendor debugger type crosses this
 *   public boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_DISASSEMBLY_VIEW_H
#define UMICOM_DEBUG_DISASSEMBLY_VIEW_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_DISASSEMBLY_VIEW_API_VERSION 1U
#define UMI_DEBUG_DISASSEMBLY_DEFAULT_CAPACITY 256U
#define UMI_DEBUG_DISASSEMBLY_MAXIMUM_CAPACITY 65536U
#define UMI_DEBUG_INSTRUCTION_ID_CAPACITY 128U
#define UMI_DEBUG_INSTRUCTION_SESSION_ID_CAPACITY 128U
#define UMI_DEBUG_INSTRUCTION_MEMORY_REFERENCE_CAPACITY 512U
#define UMI_DEBUG_INSTRUCTION_BYTES_CAPACITY 192U
#define UMI_DEBUG_INSTRUCTION_MNEMONIC_CAPACITY 128U
#define UMI_DEBUG_INSTRUCTION_OPERANDS_CAPACITY 512U
#define UMI_DEBUG_INSTRUCTION_COMMENT_CAPACITY 512U
#define UMI_DEBUG_INSTRUCTION_SYMBOL_CAPACITY 512U
#define UMI_DEBUG_INSTRUCTION_SOURCE_URI_CAPACITY 1024U

/**
 * Represent the debug instruction data shared with callers of this public contract.
 */
typedef struct UmiDebugInstruction {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DEBUG_INSTRUCTION_ID_CAPACITY];
    char session_id[UMI_DEBUG_INSTRUCTION_SESSION_ID_CAPACITY];
    char memory_reference[UMI_DEBUG_INSTRUCTION_MEMORY_REFERENCE_CAPACITY];
    uint64_t address;
    int64_t instruction_offset;
    char instruction_bytes[UMI_DEBUG_INSTRUCTION_BYTES_CAPACITY];
    char mnemonic[UMI_DEBUG_INSTRUCTION_MNEMONIC_CAPACITY];
    char operands[UMI_DEBUG_INSTRUCTION_OPERANDS_CAPACITY];
    char comment[UMI_DEBUG_INSTRUCTION_COMMENT_CAPACITY];
    char symbol[UMI_DEBUG_INSTRUCTION_SYMBOL_CAPACITY];
    char source_uri[UMI_DEBUG_INSTRUCTION_SOURCE_URI_CAPACITY];
    uint32_t source_line;
    uint32_t source_column;
    uint64_t sequence;
    uint64_t revision;
    int has_source;
    int current;
    int can_breakpoint;
    int breakpoint;
    int valid;
} UmiDebugInstruction;

/**
 * Represent the debug disassembly snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugDisassemblySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t instruction_count;
    size_t source_correlated_count;
    size_t breakpoint_count;
    size_t invalid_instruction_count;
    char selected_instruction_id[UMI_DEBUG_INSTRUCTION_ID_CAPACITY];
    char current_instruction_id[UMI_DEBUG_INSTRUCTION_ID_CAPACITY];
    uint64_t minimum_address;
    uint64_t maximum_address;
    uint64_t revision;
    int has_selection;
    int has_current_instruction;
} UmiDebugDisassemblySnapshot;

/**
 * Represent the debug disassembly view data shared with callers of this public contract.
 */
typedef struct UmiDebugDisassemblyView UmiDebugDisassemblyView;

/**
 * Initialise debug disassembly view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_disassembly_view_create(
    UmiDebugDisassemblyView **out_view);
/**
 * Release or reset state held by debug disassembly view so the same storage can be reused
 * safely.
 */
void umi_debug_disassembly_view_destroy(UmiDebugDisassemblyView *view);
/**
 * Release or reset state held by debug disassembly view so the same storage can be reused
 * safely.
 */
UmiStatus umi_debug_disassembly_view_clear(UmiDebugDisassemblyView *view);
/**
 * Read debug disassembly view into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_debug_disassembly_view_load(
    UmiDebugDisassemblyView *view,
    const UmiDebugInstruction *instructions,
    size_t instruction_count);
/**
 * Provide the debug disassembly view upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_disassembly_view_upsert(
    UmiDebugDisassemblyView *view,
    const UmiDebugInstruction *instruction);
/**
 * Remove debug disassembly view while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_disassembly_view_remove(
    UmiDebugDisassemblyView *view,
    const char *instruction_id);
/**
 * Provide the debug disassembly view select operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_disassembly_view_select(
    UmiDebugDisassemblyView *view,
    const char *instruction_id);
/**
 * Provide the debug disassembly view select address operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_disassembly_view_select_address(
    UmiDebugDisassemblyView *view,
    uint64_t address);
/**
 * Provide the debug disassembly view set current operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_disassembly_view_set_current(
    UmiDebugDisassemblyView *view,
    const char *instruction_id);
/**
 * Provide the debug disassembly view set breakpoint operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_disassembly_view_set_breakpoint(
    UmiDebugDisassemblyView *view,
    const char *instruction_id,
    int has_breakpoint);
/**
 * Find debug disassembly view while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_disassembly_view_find(
    const UmiDebugDisassemblyView *view,
    const char *instruction_id,
    UmiDebugInstruction *out_instruction);
/**
 * Provide the debug disassembly view find address operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_disassembly_view_find_address(
    const UmiDebugDisassemblyView *view,
    uint64_t address,
    UmiDebugInstruction *out_instruction);
/**
 * Find debug disassembly view while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_disassembly_view_at(
    const UmiDebugDisassemblyView *view,
    size_t index,
    UmiDebugInstruction *out_instruction);
/**
 * Find debug disassembly view while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_disassembly_view_selected(
    const UmiDebugDisassemblyView *view,
    UmiDebugInstruction *out_instruction);
/**
 * Provide the debug disassembly view current operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_disassembly_view_current(
    const UmiDebugDisassemblyView *view,
    UmiDebugInstruction *out_instruction);
/**
 * Provide the debug disassembly view snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_disassembly_view_snapshot(
    const UmiDebugDisassemblyView *view,
    UmiDebugDisassemblySnapshot *out_snapshot);
/**
 * Return the number of records represented by debug disassembly view without changing
 * their state.
 */
size_t umi_debug_disassembly_view_count(const UmiDebugDisassemblyView *view);
/**
 * Provide the debug disassembly view revision operation used by this module and its client
 * applications.
 */
uint64_t umi_debug_disassembly_view_revision(
    const UmiDebugDisassemblyView *view);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_DISASSEMBLY_VIEW_H */
