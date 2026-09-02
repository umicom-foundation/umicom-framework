/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/register_bank.h
 *
 * PURPOSE:
 *   Represent architecture-neutral CPU, virtual-machine and runtime registers
 *   as grouped, selectable debugger data without assuming x86, ARM, RISC-V,
 *   DAP, GDB, LLDB, CDB or a graphical toolkit.
 *
 * OWNERSHIP:
 *   Groups and registers are copied into Framework-owned storage. Values are
 *   display strings because adapters retain responsibility for architecture-
 *   specific parsing and writes. A frontend may stage an edit, invoke an
 *   adapter and update this model only after the adapter confirms success.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_REGISTER_BANK_H
#define UMICOM_DEBUG_REGISTER_BANK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_REGISTER_BANK_API_VERSION 1U
#define UMI_DEBUG_REGISTER_GROUP_DEFAULT_CAPACITY 16U
#define UMI_DEBUG_REGISTER_GROUP_MAXIMUM_CAPACITY 256U
#define UMI_DEBUG_REGISTER_DEFAULT_CAPACITY 128U
#define UMI_DEBUG_REGISTER_MAXIMUM_CAPACITY 8192U
#define UMI_DEBUG_REGISTER_ID_CAPACITY 128U
#define UMI_DEBUG_REGISTER_GROUP_ID_CAPACITY 128U
#define UMI_DEBUG_REGISTER_SESSION_ID_CAPACITY 128U
#define UMI_DEBUG_REGISTER_FRAME_ID_CAPACITY 128U
#define UMI_DEBUG_REGISTER_NAME_CAPACITY 128U
#define UMI_DEBUG_REGISTER_DESCRIPTION_CAPACITY 512U
#define UMI_DEBUG_REGISTER_VALUE_CAPACITY 512U
#define UMI_DEBUG_REGISTER_TYPE_CAPACITY 128U
#define UMI_DEBUG_REGISTER_FORMAT_CAPACITY 64U

/**
 * Represent the debug register group data shared with callers of this public contract.
 */
typedef struct UmiDebugRegisterGroup {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DEBUG_REGISTER_GROUP_ID_CAPACITY];
    char session_id[UMI_DEBUG_REGISTER_SESSION_ID_CAPACITY];
    char frame_id[UMI_DEBUG_REGISTER_FRAME_ID_CAPACITY];
    char name[UMI_DEBUG_REGISTER_NAME_CAPACITY];
    char description[UMI_DEBUG_REGISTER_DESCRIPTION_CAPACITY];
    int32_t order;
    uint64_t sequence;
    uint64_t revision;
    int expanded;
    int available;
} UmiDebugRegisterGroup;

/**
 * Represent the debug register value data shared with callers of this public contract.
 */
typedef struct UmiDebugRegisterValue {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DEBUG_REGISTER_ID_CAPACITY];
    char group_id[UMI_DEBUG_REGISTER_GROUP_ID_CAPACITY];
    char name[UMI_DEBUG_REGISTER_NAME_CAPACITY];
    char alternate_name[UMI_DEBUG_REGISTER_NAME_CAPACITY];
    char value[UMI_DEBUG_REGISTER_VALUE_CAPACITY];
    char raw_value[UMI_DEBUG_REGISTER_VALUE_CAPACITY];
    char type[UMI_DEBUG_REGISTER_TYPE_CAPACITY];
    char format[UMI_DEBUG_REGISTER_FORMAT_CAPACITY];
    uint32_t bit_width;
    int32_t order;
    uint64_t sequence;
    uint64_t revision;
    int changed;
    int read_only;
    int available;
} UmiDebugRegisterValue;

/**
 * Represent the debug register bank snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRegisterBankSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t group_count;
    size_t register_count;
    size_t visible_register_count;
    size_t changed_register_count;
    size_t read_only_register_count;
    char selected_register_id[UMI_DEBUG_REGISTER_ID_CAPACITY];
    uint64_t revision;
    int has_selection;
} UmiDebugRegisterBankSnapshot;

/**
 * Represent the debug register bank data shared with callers of this public contract.
 */
typedef struct UmiDebugRegisterBank UmiDebugRegisterBank;

/**
 * Initialise debug register bank from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_debug_register_bank_create(
    UmiDebugRegisterBank **out_bank);
/**
 * Release or reset state held by debug register bank so the same storage can be reused
 * safely.
 */
void umi_debug_register_bank_destroy(UmiDebugRegisterBank *bank);
/**
 * Release or reset state held by debug register bank so the same storage can be reused
 * safely.
 */
UmiStatus umi_debug_register_bank_clear(UmiDebugRegisterBank *bank);
/**
 * Provide the debug register bank upsert group operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_register_bank_upsert_group(
    UmiDebugRegisterBank *bank,
    const UmiDebugRegisterGroup *group);
/**
 * Add debug register bank upsert only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_debug_register_bank_upsert_register(
    UmiDebugRegisterBank *bank,
    const UmiDebugRegisterValue *register_value);
/**
 * Provide the debug register bank remove group operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_register_bank_remove_group(
    UmiDebugRegisterBank *bank,
    const char *group_id);
/**
 * Add debug register bank remove only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_debug_register_bank_remove_register(
    UmiDebugRegisterBank *bank,
    const char *register_id);
/**
 * Provide the debug register bank set group expanded operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_register_bank_set_group_expanded(
    UmiDebugRegisterBank *bank,
    const char *group_id,
    int expanded);
/**
 * Provide the debug register bank select operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_register_bank_select(
    UmiDebugRegisterBank *bank,
    const char *register_id);
/**
 * Provide the debug register bank update value operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_register_bank_update_value(
    UmiDebugRegisterBank *bank,
    const char *register_id,
    const char *value,
    const char *raw_value,
    int changed);
/**
 * Provide the debug register bank find group operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_register_bank_find_group(
    const UmiDebugRegisterBank *bank,
    const char *group_id,
    UmiDebugRegisterGroup *out_group);
/**
 * Add debug register bank find only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_debug_register_bank_find_register(
    const UmiDebugRegisterBank *bank,
    const char *register_id,
    UmiDebugRegisterValue *out_register);
/**
 * Find debug register bank group while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_register_bank_group_at(
    const UmiDebugRegisterBank *bank,
    size_t index,
    UmiDebugRegisterGroup *out_group);
/**
 * Find debug register bank register while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_debug_register_bank_register_at(
    const UmiDebugRegisterBank *bank,
    size_t index,
    UmiDebugRegisterValue *out_register);
/**
 * Find debug register bank visible register while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_register_bank_visible_register_at(
    const UmiDebugRegisterBank *bank,
    size_t visible_index,
    UmiDebugRegisterValue *out_register);
/**
 * Find debug register bank while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_register_bank_selected(
    const UmiDebugRegisterBank *bank,
    UmiDebugRegisterValue *out_register);
/**
 * Provide the debug register bank snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_register_bank_snapshot(
    const UmiDebugRegisterBank *bank,
    UmiDebugRegisterBankSnapshot *out_snapshot);
/**
 * Return the number of records represented by debug register bank group without changing
 * their state.
 */
size_t umi_debug_register_bank_group_count(const UmiDebugRegisterBank *bank);
/**
 * Return the number of records represented by debug register bank register without
 * changing their state.
 */
size_t umi_debug_register_bank_register_count(const UmiDebugRegisterBank *bank);
/**
 * Return the number of records represented by debug register bank visible without changing
 * their state.
 */
size_t umi_debug_register_bank_visible_count(const UmiDebugRegisterBank *bank);
/**
 * Provide the debug register bank revision operation used by this module and its client
 * applications.
 */
uint64_t umi_debug_register_bank_revision(const UmiDebugRegisterBank *bank);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_REGISTER_BANK_H */
