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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiDebugRegisterBank UmiDebugRegisterBank;

UmiStatus umi_debug_register_bank_create(
    UmiDebugRegisterBank **out_bank);
void umi_debug_register_bank_destroy(UmiDebugRegisterBank *bank);
UmiStatus umi_debug_register_bank_clear(UmiDebugRegisterBank *bank);
UmiStatus umi_debug_register_bank_upsert_group(
    UmiDebugRegisterBank *bank,
    const UmiDebugRegisterGroup *group);
UmiStatus umi_debug_register_bank_upsert_register(
    UmiDebugRegisterBank *bank,
    const UmiDebugRegisterValue *register_value);
UmiStatus umi_debug_register_bank_remove_group(
    UmiDebugRegisterBank *bank,
    const char *group_id);
UmiStatus umi_debug_register_bank_remove_register(
    UmiDebugRegisterBank *bank,
    const char *register_id);
UmiStatus umi_debug_register_bank_set_group_expanded(
    UmiDebugRegisterBank *bank,
    const char *group_id,
    int expanded);
UmiStatus umi_debug_register_bank_select(
    UmiDebugRegisterBank *bank,
    const char *register_id);
UmiStatus umi_debug_register_bank_update_value(
    UmiDebugRegisterBank *bank,
    const char *register_id,
    const char *value,
    const char *raw_value,
    int changed);
UmiStatus umi_debug_register_bank_find_group(
    const UmiDebugRegisterBank *bank,
    const char *group_id,
    UmiDebugRegisterGroup *out_group);
UmiStatus umi_debug_register_bank_find_register(
    const UmiDebugRegisterBank *bank,
    const char *register_id,
    UmiDebugRegisterValue *out_register);
UmiStatus umi_debug_register_bank_group_at(
    const UmiDebugRegisterBank *bank,
    size_t index,
    UmiDebugRegisterGroup *out_group);
UmiStatus umi_debug_register_bank_register_at(
    const UmiDebugRegisterBank *bank,
    size_t index,
    UmiDebugRegisterValue *out_register);
UmiStatus umi_debug_register_bank_visible_register_at(
    const UmiDebugRegisterBank *bank,
    size_t visible_index,
    UmiDebugRegisterValue *out_register);
UmiStatus umi_debug_register_bank_selected(
    const UmiDebugRegisterBank *bank,
    UmiDebugRegisterValue *out_register);
UmiStatus umi_debug_register_bank_snapshot(
    const UmiDebugRegisterBank *bank,
    UmiDebugRegisterBankSnapshot *out_snapshot);
size_t umi_debug_register_bank_group_count(const UmiDebugRegisterBank *bank);
size_t umi_debug_register_bank_register_count(const UmiDebugRegisterBank *bank);
size_t umi_debug_register_bank_visible_count(const UmiDebugRegisterBank *bank);
uint64_t umi_debug_register_bank_revision(const UmiDebugRegisterBank *bank);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_REGISTER_BANK_H */
