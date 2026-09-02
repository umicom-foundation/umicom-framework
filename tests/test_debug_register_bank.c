/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_register_bank.c
 *
 * PURPOSE:
 *   Implement the test debug register bank behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework register bank tests.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/debug/register_bank.h"

/*
 * Exercise make group and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiDebugRegisterGroup make_group(
    const char *id,
    const char *name,
    int32_t order)
{
    UmiDebugRegisterGroup group;
    (void)memset(&group, 0, sizeof(group));
    group.struct_size = (uint32_t)sizeof(group);
    group.api_version = UMI_DEBUG_REGISTER_BANK_API_VERSION;
    (void)snprintf(group.id, sizeof(group.id), "%s", id);
    (void)snprintf(group.session_id, sizeof(group.session_id), "%s", "session-1");
    (void)snprintf(group.frame_id, sizeof(group.frame_id), "%s", "frame-1");
    (void)snprintf(group.name, sizeof(group.name), "%s", name);
    group.order = order;
    group.expanded = 1;
    group.available = 1;
    return group;
}

/* Add make only after its inputs and available capacity have been checked. */
static UmiDebugRegisterValue make_register(
    const char *id,
    const char *group_id,
    const char *name,
    const char *value,
    int32_t order)
{
    UmiDebugRegisterValue item;
    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_DEBUG_REGISTER_BANK_API_VERSION;
    (void)snprintf(item.id, sizeof(item.id), "%s", id);
    (void)snprintf(item.group_id, sizeof(item.group_id), "%s", group_id);
    (void)snprintf(item.name, sizeof(item.name), "%s", name);
    (void)snprintf(item.value, sizeof(item.value), "%s", value);
    (void)snprintf(item.raw_value, sizeof(item.raw_value), "%s", value);
    (void)snprintf(item.type, sizeof(item.type), "%s", "uint64");
    (void)snprintf(item.format, sizeof(item.format), "%s", "hex");
    item.bit_width = 64U;
    item.order = order;
    item.available = 1;
    return item;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugRegisterBank *bank = NULL;
    UmiDebugRegisterBankSnapshot snapshot;
    UmiDebugRegisterGroup group;
    UmiDebugRegisterValue item;

    assert(umi_debug_register_bank_create(&bank) == UMI_STATUS_OK);
    group = make_group("general", "General", 0);
    assert(umi_debug_register_bank_upsert_group(bank, &group) == UMI_STATUS_OK);
    group = make_group("vector", "Vector", 1);
    assert(umi_debug_register_bank_upsert_group(bank, &group) == UMI_STATUS_OK);
    assert(umi_debug_register_bank_find_group(bank, "general", &group) ==
           UMI_STATUS_OK);
    {
        const uint64_t group_sequence = group.sequence;
        (void)snprintf(group.description, sizeof(group.description), "%s",
                       "General purpose registers");
        group.sequence = 0U;
        assert(umi_debug_register_bank_upsert_group(bank, &group) ==
               UMI_STATUS_OK);
        assert(umi_debug_register_bank_find_group(bank, "general", &group) ==
               UMI_STATUS_OK);
        assert(group.sequence == group_sequence);
    }

    item = make_register("rax", "general", "rax", "0x1", 0);
    assert(umi_debug_register_bank_upsert_register(bank, &item) ==
           UMI_STATUS_OK);
    assert(umi_debug_register_bank_find_register(bank, "rax", &item) ==
           UMI_STATUS_OK);
    {
        const uint64_t register_sequence = item.sequence;
        (void)snprintf(item.value, sizeof(item.value), "%s", "0x10");
        item.sequence = 0U;
        assert(umi_debug_register_bank_upsert_register(bank, &item) ==
               UMI_STATUS_OK);
        assert(umi_debug_register_bank_find_register(bank, "rax", &item) ==
               UMI_STATUS_OK);
        assert(item.sequence == register_sequence);
    }
    item = make_register("rbx", "general", "rbx", "0x2", 1);
    assert(umi_debug_register_bank_upsert_register(bank, &item) ==
           UMI_STATUS_OK);
    item = make_register("xmm0", "vector", "xmm0", "0x00", 0);
    item.read_only = 1;
    assert(umi_debug_register_bank_upsert_register(bank, &item) ==
           UMI_STATUS_OK);

    assert(umi_debug_register_bank_snapshot(bank, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.group_count == 2U);
    assert(snapshot.register_count == 3U);
    assert(snapshot.visible_register_count == 3U);
    assert(snapshot.read_only_register_count == 1U);

    assert(umi_debug_register_bank_select(bank, "rbx") == UMI_STATUS_OK);
    assert(umi_debug_register_bank_update_value(
               bank, "rbx", "0x3", "0x3", 1) == UMI_STATUS_OK);
    assert(umi_debug_register_bank_selected(bank, &item) == UMI_STATUS_OK);
    assert(strcmp(item.value, "0x3") == 0);
    assert(item.changed);
    assert(umi_debug_register_bank_update_value(
               bank, "xmm0", "0x1", "0x1", 1) ==
           UMI_STATUS_PERMISSION_DENIED);

    assert(umi_debug_register_bank_set_group_expanded(
               bank, "vector", 0) == UMI_STATUS_OK);
    assert(umi_debug_register_bank_visible_count(bank) == 2U);
    assert(umi_debug_register_bank_remove_group(bank, "general") ==
           UMI_STATUS_OK);
    assert(umi_debug_register_bank_register_count(bank) == 1U);

    group = make_group("sequence-invalid", "Invalid", 2);
    group.sequence = UINT64_MAX;
    assert(umi_debug_register_bank_upsert_group(bank, &group) ==
           UMI_STATUS_INVALID_ARGUMENT);
    group = make_group("sequence-limit", "Limit", 2);
    group.sequence = UINT64_MAX - 1U;
    assert(umi_debug_register_bank_upsert_group(bank, &group) == UMI_STATUS_OK);
    group = make_group("sequence-overflow", "Overflow", 3);
    assert(umi_debug_register_bank_upsert_group(bank, &group) ==
           UMI_STATUS_CAPACITY_EXCEEDED);

    item = make_register("sequence-invalid-register", "vector", "invalid",
                         "0x0", 2);
    item.sequence = UINT64_MAX;
    assert(umi_debug_register_bank_upsert_register(bank, &item) ==
           UMI_STATUS_INVALID_ARGUMENT);
    item = make_register("sequence-limit-register", "vector", "limit",
                         "0x0", 2);
    item.sequence = UINT64_MAX - 1U;
    assert(umi_debug_register_bank_upsert_register(bank, &item) ==
           UMI_STATUS_OK);
    item = make_register("sequence-overflow-register", "vector", "overflow",
                         "0x0", 3);
    assert(umi_debug_register_bank_upsert_register(bank, &item) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    umi_debug_register_bank_destroy(bank);
    return 0;
}
