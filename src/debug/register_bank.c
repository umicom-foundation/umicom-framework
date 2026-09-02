/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/register_bank.c
 *
 * PURPOSE:
 *   Implement grouped register storage, selection, visibility and confirmed
 *   value updates for the advanced debugger experience.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/register_bank.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugRegisterBank {
    UmiDebugRegisterGroup *groups;
    size_t group_count;
    size_t group_capacity;
    UmiDebugRegisterValue *registers;
    size_t register_count;
    size_t register_capacity;
    char selected_register_id[UMI_DEBUG_REGISTER_ID_CAPACITY];
    uint64_t next_group_sequence;
    uint64_t next_register_sequence;
    uint64_t revision;
};

/* Provide the has terminator operation used by this module and its client applications. */
static int has_terminator(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

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

/* Provide the valid group operation used by this module and its client applications. */
static int valid_group(const UmiDebugRegisterGroup *group)
{
    return group != NULL &&
           group->struct_size == (uint32_t)sizeof(*group) &&
           group->api_version == UMI_DEBUG_REGISTER_BANK_API_VERSION &&
           has_terminator(group->id, sizeof(group->id)) &&
           group->id[0] != '\0' &&
           has_terminator(group->session_id, sizeof(group->session_id)) &&
           has_terminator(group->frame_id, sizeof(group->frame_id)) &&
           has_terminator(group->name, sizeof(group->name)) &&
           group->name[0] != '\0' &&
           has_terminator(group->description, sizeof(group->description)) &&
           group->sequence != UINT64_MAX;
}

/* Add valid only after its inputs and available capacity have been checked. */
static int valid_register(const UmiDebugRegisterValue *register_value)
{
    return register_value != NULL &&
           register_value->struct_size ==
               (uint32_t)sizeof(*register_value) &&
           register_value->api_version ==
               UMI_DEBUG_REGISTER_BANK_API_VERSION &&
           has_terminator(register_value->id, sizeof(register_value->id)) &&
           register_value->id[0] != '\0' &&
           has_terminator(register_value->group_id,
                          sizeof(register_value->group_id)) &&
           register_value->group_id[0] != '\0' &&
           has_terminator(register_value->name,
                          sizeof(register_value->name)) &&
           register_value->name[0] != '\0' &&
           has_terminator(register_value->alternate_name,
                          sizeof(register_value->alternate_name)) &&
           has_terminator(register_value->value,
                          sizeof(register_value->value)) &&
           has_terminator(register_value->raw_value,
                          sizeof(register_value->raw_value)) &&
           has_terminator(register_value->type,
                          sizeof(register_value->type)) &&
           has_terminator(register_value->format,
                          sizeof(register_value->format)) &&
           register_value->sequence != UINT64_MAX;
}

/* Provide the find group index operation used by this module and its client applications. */
static size_t find_group_index(const UmiDebugRegisterBank *bank,
                               const char *group_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || group_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < bank->group_count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(bank->groups[position].id, group_id) == 0) return position;
    }
    return SIZE_MAX;
}

/*
 * Provide the find register index operation used by this module and its client
 * applications.
 */
static size_t find_register_index(const UmiDebugRegisterBank *bank,
                                  const char *register_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || register_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < bank->register_count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(bank->registers[position].id, register_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/* Provide the reserve groups operation used by this module and its client applications. */
static UmiStatus reserve_groups(UmiDebugRegisterBank *bank,
                                size_t required_capacity)
{
    UmiDebugRegisterGroup *replacement;
    size_t new_capacity;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required_capacity <= bank->group_capacity) return UMI_STATUS_OK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (required_capacity > UMI_DEBUG_REGISTER_GROUP_MAXIMUM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = bank->group_capacity;
    /* Apply this branch only when its contract condition is satisfied. */
    if (new_capacity == 0U) {
        new_capacity = UMI_DEBUG_REGISTER_GROUP_DEFAULT_CAPACITY;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (new_capacity < required_capacity) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (new_capacity > UMI_DEBUG_REGISTER_GROUP_MAXIMUM_CAPACITY / 2U) {
            new_capacity = UMI_DEBUG_REGISTER_GROUP_MAXIMUM_CAPACITY;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            new_capacity *= 2U;
        }
    }
    replacement = (UmiDebugRegisterGroup *)realloc(
        bank->groups, new_capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memset(&replacement[bank->group_capacity], 0,
                 (new_capacity - bank->group_capacity) * sizeof(*replacement));
    bank->groups = replacement;
    bank->group_capacity = new_capacity;
    return UMI_STATUS_OK;
}

/* Provide the reserve registers operation used by this module and its client applications. */
static UmiStatus reserve_registers(UmiDebugRegisterBank *bank,
                                   size_t required_capacity)
{
    UmiDebugRegisterValue *replacement;
    size_t new_capacity;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required_capacity <= bank->register_capacity) return UMI_STATUS_OK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (required_capacity > UMI_DEBUG_REGISTER_MAXIMUM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = bank->register_capacity;
    /* Apply this branch only when its contract condition is satisfied. */
    if (new_capacity == 0U) new_capacity = UMI_DEBUG_REGISTER_DEFAULT_CAPACITY;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (new_capacity < required_capacity) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (new_capacity > UMI_DEBUG_REGISTER_MAXIMUM_CAPACITY / 2U) {
            new_capacity = UMI_DEBUG_REGISTER_MAXIMUM_CAPACITY;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            new_capacity *= 2U;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (new_capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiDebugRegisterValue *)realloc(
        bank->registers, new_capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memset(&replacement[bank->register_capacity], 0,
                 (new_capacity - bank->register_capacity) *
                     sizeof(*replacement));
    bank->registers = replacement;
    bank->register_capacity = new_capacity;
    return UMI_STATUS_OK;
}

/* Provide the compare groups operation used by this module and its client applications. */
static int compare_groups(const void *left_pointer,
                          const void *right_pointer)
{
    const UmiDebugRegisterGroup *left =
        (const UmiDebugRegisterGroup *)left_pointer;
    const UmiDebugRegisterGroup *right =
        (const UmiDebugRegisterGroup *)right_pointer;
    int order;

    /* Apply this branch only when its contract condition is satisfied. */
    if (left->order < right->order) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->order > right->order) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->sequence < right->sequence) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->sequence > right->sequence) return 1;
    order = strcmp(left->name, right->name);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order < 0 ? -1 : 1;
    order = strcmp(left->id, right->id);
    return order < 0 ? -1 : (order > 0 ? 1 : 0);
}

/* Provide the compare registers operation used by this module and its client applications. */
static int compare_registers(const void *left_pointer,
                             const void *right_pointer)
{
    const UmiDebugRegisterValue *left =
        (const UmiDebugRegisterValue *)left_pointer;
    const UmiDebugRegisterValue *right =
        (const UmiDebugRegisterValue *)right_pointer;
    int order;

    order = strcmp(left->group_id, right->group_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order < 0 ? -1 : 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->order < right->order) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->order > right->order) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->sequence < right->sequence) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->sequence > right->sequence) return 1;
    order = strcmp(left->name, right->name);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order < 0 ? -1 : 1;
    order = strcmp(left->id, right->id);
    return order < 0 ? -1 : (order > 0 ? 1 : 0);
}

/* Provide the normalize group operation used by this module and its client applications. */
static void normalize_group(UmiDebugRegisterGroup *group)
{
    group->struct_size = (uint32_t)sizeof(*group);
    group->api_version = UMI_DEBUG_REGISTER_BANK_API_VERSION;
    group->id[sizeof(group->id) - 1U] = '\0';
    group->session_id[sizeof(group->session_id) - 1U] = '\0';
    group->frame_id[sizeof(group->frame_id) - 1U] = '\0';
    group->name[sizeof(group->name) - 1U] = '\0';
    group->description[sizeof(group->description) - 1U] = '\0';
    group->expanded = group->expanded != 0;
    group->available = group->available != 0;
}

/* Add normalize only after its inputs and available capacity have been checked. */
static void normalize_register(UmiDebugRegisterValue *register_value)
{
    register_value->struct_size = (uint32_t)sizeof(*register_value);
    register_value->api_version = UMI_DEBUG_REGISTER_BANK_API_VERSION;
    register_value->id[sizeof(register_value->id) - 1U] = '\0';
    register_value->group_id[sizeof(register_value->group_id) - 1U] = '\0';
    register_value->name[sizeof(register_value->name) - 1U] = '\0';
    register_value->alternate_name[
        sizeof(register_value->alternate_name) - 1U] = '\0';
    register_value->value[sizeof(register_value->value) - 1U] = '\0';
    register_value->raw_value[sizeof(register_value->raw_value) - 1U] = '\0';
    register_value->type[sizeof(register_value->type) - 1U] = '\0';
    register_value->format[sizeof(register_value->format) - 1U] = '\0';
    register_value->changed = register_value->changed != 0;
    register_value->read_only = register_value->read_only != 0;
    register_value->available = register_value->available != 0;
}

/* Provide the register visible operation used by this module and its client applications. */
static int register_visible(const UmiDebugRegisterBank *bank,
                            const UmiDebugRegisterValue *register_value)
{
    size_t group_position;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!register_value->available) return 0;
    group_position = find_group_index(bank, register_value->group_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (group_position == SIZE_MAX) return 0;
    return bank->groups[group_position].available &&
           bank->groups[group_position].expanded;
}

/*
 * Initialise debug register bank from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_debug_register_bank_create(UmiDebugRegisterBank **out_bank)
{
    UmiDebugRegisterBank *bank;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_bank == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_bank = NULL;
    bank = (UmiDebugRegisterBank *)calloc(1U, sizeof(*bank));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    bank->next_group_sequence = 1U;
    bank->next_register_sequence = 1U;
    bank->revision = 1U;
    *out_bank = bank;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by debug register bank so the same storage can be reused
 * safely.
 */
void umi_debug_register_bank_destroy(UmiDebugRegisterBank *bank)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL) return;
    free(bank->groups);
    free(bank->registers);
    (void)memset(bank, 0, sizeof(*bank));
    free(bank);
}

/*
 * Release or reset state held by debug register bank so the same storage can be reused
 * safely.
 */
UmiStatus umi_debug_register_bank_clear(UmiDebugRegisterBank *bank)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank->groups != NULL) {
        (void)memset(bank->groups, 0,
                     bank->group_capacity * sizeof(bank->groups[0]));
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank->registers != NULL) {
        (void)memset(bank->registers, 0,
                     bank->register_capacity * sizeof(bank->registers[0]));
    }
    bank->group_count = 0U;
    bank->register_count = 0U;
    bank->selected_register_id[0] = '\0';
    bank->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug register bank upsert group operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_register_bank_upsert_group(
    UmiDebugRegisterBank *bank,
    const UmiDebugRegisterGroup *group)
{
    UmiDebugRegisterGroup copy;
    size_t position;
    UmiStatus status;
    int existing;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || !valid_group(group)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_group_index(bank, group->id);
    existing = position != SIZE_MAX;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!existing && group->sequence == 0U &&
        bank->next_group_sequence == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!existing) {
        status = reserve_groups(bank, bank->group_count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        position = bank->group_count;
        bank->group_count += 1U;
    }
    copy = *group;
    normalize_group(&copy);
    /* Apply this branch only when its contract condition is satisfied. */
    if (copy.sequence == 0U && existing) {
        copy.sequence = bank->groups[position].sequence;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (copy.sequence == 0U) {
        copy.sequence = bank->next_group_sequence;
        bank->next_group_sequence += 1U;
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (copy.sequence >= bank->next_group_sequence) {
        bank->next_group_sequence = copy.sequence + 1U;
    }
    bank->revision += 1U;
    copy.revision = bank->revision;
    bank->groups[position] = copy;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bank->group_count > 1U) {
        qsort(bank->groups, bank->group_count, sizeof(bank->groups[0]),
              compare_groups);
    }
    return UMI_STATUS_OK;
}

/*
 * Add debug register bank upsert only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_debug_register_bank_upsert_register(
    UmiDebugRegisterBank *bank,
    const UmiDebugRegisterValue *register_value)
{
    UmiDebugRegisterValue copy;
    size_t position;
    UmiStatus status;
    int existing;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || !valid_register(register_value)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_group_index(bank, register_value->group_id) == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    position = find_register_index(bank, register_value->id);
    existing = position != SIZE_MAX;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!existing && register_value->sequence == 0U &&
        bank->next_register_sequence == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!existing) {
        status = reserve_registers(bank, bank->register_count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        position = bank->register_count;
        bank->register_count += 1U;
    }
    copy = *register_value;
    normalize_register(&copy);
    /* Apply this branch only when its contract condition is satisfied. */
    if (copy.sequence == 0U && existing) {
        copy.sequence = bank->registers[position].sequence;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (copy.sequence == 0U) {
        copy.sequence = bank->next_register_sequence;
        bank->next_register_sequence += 1U;
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (copy.sequence >= bank->next_register_sequence) {
        bank->next_register_sequence = copy.sequence + 1U;
    }
    bank->revision += 1U;
    copy.revision = bank->revision;
    bank->registers[position] = copy;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bank->register_count > 1U) {
        qsort(bank->registers, bank->register_count,
              sizeof(bank->registers[0]), compare_registers);
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the debug register bank remove group operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_register_bank_remove_group(
    UmiDebugRegisterBank *bank,
    const char *group_id)
{
    size_t group_position;
    size_t read_position;
    size_t write_position = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || group_id == NULL || group_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    group_position = find_group_index(bank, group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (group_position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;

    /* Visit each bounded item once so every record receives the same rule. */
    for (read_position = 0U; read_position < bank->register_count;
         ++read_position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(bank->registers[read_position].group_id, group_id) == 0) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(bank->selected_register_id,
                       bank->registers[read_position].id) == 0) {
                bank->selected_register_id[0] = '\0';
            }
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (write_position != read_position) {
            bank->registers[write_position] = bank->registers[read_position];
        }
        write_position += 1U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (write_position < bank->register_count) {
        (void)memset(&bank->registers[write_position], 0,
                     (bank->register_count - write_position) *
                         sizeof(bank->registers[0]));
        bank->register_count = write_position;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (group_position + 1U < bank->group_count) {
        (void)memmove(&bank->groups[group_position],
                      &bank->groups[group_position + 1U],
                      (bank->group_count - group_position - 1U) *
                          sizeof(bank->groups[0]));
    }
    bank->group_count -= 1U;
    (void)memset(&bank->groups[bank->group_count], 0,
                 sizeof(bank->groups[bank->group_count]));
    bank->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Add debug register bank remove only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_debug_register_bank_remove_register(
    UmiDebugRegisterBank *bank,
    const char *register_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || register_id == NULL || register_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_register_index(bank, register_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (position + 1U < bank->register_count) {
        (void)memmove(&bank->registers[position],
                      &bank->registers[position + 1U],
                      (bank->register_count - position - 1U) *
                          sizeof(bank->registers[0]));
    }
    bank->register_count -= 1U;
    (void)memset(&bank->registers[bank->register_count], 0,
                 sizeof(bank->registers[bank->register_count]));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(bank->selected_register_id, register_id) == 0) {
        bank->selected_register_id[0] = '\0';
    }
    bank->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug register bank set group expanded operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_register_bank_set_group_expanded(
    UmiDebugRegisterBank *bank,
    const char *group_id,
    int expanded)
{
    size_t position;
    int normalized;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || group_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    position = find_group_index(bank, group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    normalized = expanded != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bank->groups[position].expanded != normalized) {
        bank->groups[position].expanded = normalized;
        bank->revision += 1U;
        bank->groups[position].revision = bank->revision;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the debug register bank select operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_register_bank_select(
    UmiDebugRegisterBank *bank,
    const char *register_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || register_id == NULL || register_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_register_index(bank, register_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!bank->registers[position].available) return UMI_STATUS_UNAVAILABLE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(bank->selected_register_id, register_id) != 0) {
        UmiStatus status = copy_text(bank->selected_register_id,
                                     sizeof(bank->selected_register_id),
                                     register_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        bank->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the debug register bank update value operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_register_bank_update_value(
    UmiDebugRegisterBank *bank,
    const char *register_id,
    const char *value,
    const char *raw_value,
    int changed)
{
    size_t position;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || register_id == NULL || value == NULL ||
        raw_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_register_index(bank, register_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!bank->registers[position].available) return UMI_STATUS_UNAVAILABLE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (bank->registers[position].read_only) return UMI_STATUS_PERMISSION_DENIED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(value) >= sizeof(bank->registers[position].value) ||
        strlen(raw_value) >= sizeof(bank->registers[position].raw_value)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = copy_text(bank->registers[position].value,
                       sizeof(bank->registers[position].value), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(bank->registers[position].raw_value,
                       sizeof(bank->registers[position].raw_value), raw_value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    bank->registers[position].changed = changed != 0;
    bank->revision += 1U;
    bank->registers[position].revision = bank->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug register bank find group operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_register_bank_find_group(
    const UmiDebugRegisterBank *bank,
    const char *group_id,
    UmiDebugRegisterGroup *out_group)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || group_id == NULL || out_group == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_group_index(bank, group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_group = bank->groups[position];
    return UMI_STATUS_OK;
}

/*
 * Add debug register bank find only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_debug_register_bank_find_register(
    const UmiDebugRegisterBank *bank,
    const char *register_id,
    UmiDebugRegisterValue *out_register)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || register_id == NULL || out_register == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_register_index(bank, register_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_register = bank->registers[position];
    return UMI_STATUS_OK;
}

/*
 * Find debug register bank group while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_register_bank_group_at(
    const UmiDebugRegisterBank *bank,
    size_t index,
    UmiDebugRegisterGroup *out_group)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || out_group == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= bank->group_count) return UMI_STATUS_NOT_FOUND;
    *out_group = bank->groups[index];
    return UMI_STATUS_OK;
}

/*
 * Find debug register bank register while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_debug_register_bank_register_at(
    const UmiDebugRegisterBank *bank,
    size_t index,
    UmiDebugRegisterValue *out_register)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || out_register == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= bank->register_count) return UMI_STATUS_NOT_FOUND;
    *out_register = bank->registers[index];
    return UMI_STATUS_OK;
}

/*
 * Find debug register bank visible register while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_register_bank_visible_register_at(
    const UmiDebugRegisterBank *bank,
    size_t visible_index,
    UmiDebugRegisterValue *out_register)
{
    size_t group_position;
    size_t register_position;
    size_t current_visible = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || out_register == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (group_position = 0U; group_position < bank->group_count;
         ++group_position) {
        const UmiDebugRegisterGroup *group = &bank->groups[group_position];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!group->available || !group->expanded) continue;
        /* Visit each bounded item once so every record receives the same rule. */
        for (register_position = 0U;
             register_position < bank->register_count; ++register_position) {
            const UmiDebugRegisterValue *register_value =
                &bank->registers[register_position];
            /* Apply this branch only when its contract condition is satisfied. */
            if (!register_value->available ||
                strcmp(register_value->group_id, group->id) != 0) {
                continue;
            }
            /* Apply this operation only while the related capability or state is available. */
            if (current_visible == visible_index) {
                *out_register = *register_value;
                return UMI_STATUS_OK;
            }
            current_visible += 1U;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find debug register bank while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_register_bank_selected(
    const UmiDebugRegisterBank *bank,
    UmiDebugRegisterValue *out_register)
{
    return umi_debug_register_bank_find_register(
        bank, bank != NULL ? bank->selected_register_id : NULL, out_register);
}

/*
 * Provide the debug register bank snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_register_bank_snapshot(
    const UmiDebugRegisterBank *bank,
    UmiDebugRegisterBankSnapshot *out_snapshot)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEBUG_REGISTER_BANK_API_VERSION;
    out_snapshot->group_count = bank->group_count;
    out_snapshot->register_count = bank->register_count;
    out_snapshot->revision = bank->revision;
    (void)copy_text(out_snapshot->selected_register_id,
                    sizeof(out_snapshot->selected_register_id),
                    bank->selected_register_id);
    out_snapshot->has_selection = bank->selected_register_id[0] != '\0';
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < bank->register_count; ++position) {
        /* Apply this operation only while the related capability or state is available. */
        if (register_visible(bank, &bank->registers[position])) {
            out_snapshot->visible_register_count += 1U;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (bank->registers[position].changed) {
            out_snapshot->changed_register_count += 1U;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (bank->registers[position].read_only) {
            out_snapshot->read_only_register_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by debug register bank group without changing
 * their state.
 */
size_t umi_debug_register_bank_group_count(const UmiDebugRegisterBank *bank)
{
    return bank != NULL ? bank->group_count : 0U;
}

/*
 * Return the number of records represented by debug register bank register without
 * changing their state.
 */
size_t umi_debug_register_bank_register_count(const UmiDebugRegisterBank *bank)
{
    return bank != NULL ? bank->register_count : 0U;
}

/*
 * Return the number of records represented by debug register bank visible without changing
 * their state.
 */
size_t umi_debug_register_bank_visible_count(const UmiDebugRegisterBank *bank)
{
    UmiDebugRegisterBankSnapshot snapshot;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_register_bank_snapshot(bank, &snapshot) != UMI_STATUS_OK) {
        return 0U;
    }
    return snapshot.visible_register_count;
}

/*
 * Provide the debug register bank revision operation used by this module and its client
 * applications.
 */
uint64_t umi_debug_register_bank_revision(const UmiDebugRegisterBank *bank)
{
    return bank != NULL ? bank->revision : 0U;
}
