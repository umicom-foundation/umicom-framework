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

static int has_terminator(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus copy_text(char *destination,
                           size_t destination_capacity,
                           const char *source)
{
    size_t length;

    if (destination == NULL || destination_capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= destination_capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

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

static size_t find_group_index(const UmiDebugRegisterBank *bank,
                               const char *group_id)
{
    size_t position;

    if (bank == NULL || group_id == NULL) return SIZE_MAX;
    for (position = 0U; position < bank->group_count; ++position) {
        if (strcmp(bank->groups[position].id, group_id) == 0) return position;
    }
    return SIZE_MAX;
}

static size_t find_register_index(const UmiDebugRegisterBank *bank,
                                  const char *register_id)
{
    size_t position;

    if (bank == NULL || register_id == NULL) return SIZE_MAX;
    for (position = 0U; position < bank->register_count; ++position) {
        if (strcmp(bank->registers[position].id, register_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

static UmiStatus reserve_groups(UmiDebugRegisterBank *bank,
                                size_t required_capacity)
{
    UmiDebugRegisterGroup *replacement;
    size_t new_capacity;

    if (required_capacity <= bank->group_capacity) return UMI_STATUS_OK;
    if (required_capacity > UMI_DEBUG_REGISTER_GROUP_MAXIMUM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = bank->group_capacity;
    if (new_capacity == 0U) {
        new_capacity = UMI_DEBUG_REGISTER_GROUP_DEFAULT_CAPACITY;
    }
    while (new_capacity < required_capacity) {
        if (new_capacity > UMI_DEBUG_REGISTER_GROUP_MAXIMUM_CAPACITY / 2U) {
            new_capacity = UMI_DEBUG_REGISTER_GROUP_MAXIMUM_CAPACITY;
        } else {
            new_capacity *= 2U;
        }
    }
    replacement = (UmiDebugRegisterGroup *)realloc(
        bank->groups, new_capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memset(&replacement[bank->group_capacity], 0,
                 (new_capacity - bank->group_capacity) * sizeof(*replacement));
    bank->groups = replacement;
    bank->group_capacity = new_capacity;
    return UMI_STATUS_OK;
}

static UmiStatus reserve_registers(UmiDebugRegisterBank *bank,
                                   size_t required_capacity)
{
    UmiDebugRegisterValue *replacement;
    size_t new_capacity;

    if (required_capacity <= bank->register_capacity) return UMI_STATUS_OK;
    if (required_capacity > UMI_DEBUG_REGISTER_MAXIMUM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = bank->register_capacity;
    if (new_capacity == 0U) new_capacity = UMI_DEBUG_REGISTER_DEFAULT_CAPACITY;
    while (new_capacity < required_capacity) {
        if (new_capacity > UMI_DEBUG_REGISTER_MAXIMUM_CAPACITY / 2U) {
            new_capacity = UMI_DEBUG_REGISTER_MAXIMUM_CAPACITY;
        } else {
            new_capacity *= 2U;
        }
    }
    if (new_capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiDebugRegisterValue *)realloc(
        bank->registers, new_capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memset(&replacement[bank->register_capacity], 0,
                 (new_capacity - bank->register_capacity) *
                     sizeof(*replacement));
    bank->registers = replacement;
    bank->register_capacity = new_capacity;
    return UMI_STATUS_OK;
}

static int compare_groups(const void *left_pointer,
                          const void *right_pointer)
{
    const UmiDebugRegisterGroup *left =
        (const UmiDebugRegisterGroup *)left_pointer;
    const UmiDebugRegisterGroup *right =
        (const UmiDebugRegisterGroup *)right_pointer;
    int order;

    if (left->order < right->order) return -1;
    if (left->order > right->order) return 1;
    if (left->sequence < right->sequence) return -1;
    if (left->sequence > right->sequence) return 1;
    order = strcmp(left->name, right->name);
    if (order != 0) return order < 0 ? -1 : 1;
    order = strcmp(left->id, right->id);
    return order < 0 ? -1 : (order > 0 ? 1 : 0);
}

static int compare_registers(const void *left_pointer,
                             const void *right_pointer)
{
    const UmiDebugRegisterValue *left =
        (const UmiDebugRegisterValue *)left_pointer;
    const UmiDebugRegisterValue *right =
        (const UmiDebugRegisterValue *)right_pointer;
    int order;

    order = strcmp(left->group_id, right->group_id);
    if (order != 0) return order < 0 ? -1 : 1;
    if (left->order < right->order) return -1;
    if (left->order > right->order) return 1;
    if (left->sequence < right->sequence) return -1;
    if (left->sequence > right->sequence) return 1;
    order = strcmp(left->name, right->name);
    if (order != 0) return order < 0 ? -1 : 1;
    order = strcmp(left->id, right->id);
    return order < 0 ? -1 : (order > 0 ? 1 : 0);
}

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

static int register_visible(const UmiDebugRegisterBank *bank,
                            const UmiDebugRegisterValue *register_value)
{
    size_t group_position;

    if (!register_value->available) return 0;
    group_position = find_group_index(bank, register_value->group_id);
    if (group_position == SIZE_MAX) return 0;
    return bank->groups[group_position].available &&
           bank->groups[group_position].expanded;
}

UmiStatus umi_debug_register_bank_create(UmiDebugRegisterBank **out_bank)
{
    UmiDebugRegisterBank *bank;

    if (out_bank == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_bank = NULL;
    bank = (UmiDebugRegisterBank *)calloc(1U, sizeof(*bank));
    if (bank == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    bank->next_group_sequence = 1U;
    bank->next_register_sequence = 1U;
    bank->revision = 1U;
    *out_bank = bank;
    return UMI_STATUS_OK;
}

void umi_debug_register_bank_destroy(UmiDebugRegisterBank *bank)
{
    if (bank == NULL) return;
    free(bank->groups);
    free(bank->registers);
    (void)memset(bank, 0, sizeof(*bank));
    free(bank);
}

UmiStatus umi_debug_register_bank_clear(UmiDebugRegisterBank *bank)
{
    if (bank == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (bank->groups != NULL) {
        (void)memset(bank->groups, 0,
                     bank->group_capacity * sizeof(bank->groups[0]));
    }
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

UmiStatus umi_debug_register_bank_upsert_group(
    UmiDebugRegisterBank *bank,
    const UmiDebugRegisterGroup *group)
{
    UmiDebugRegisterGroup copy;
    size_t position;
    UmiStatus status;
    int existing;

    if (bank == NULL || !valid_group(group)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_group_index(bank, group->id);
    existing = position != SIZE_MAX;
    if (!existing && group->sequence == 0U &&
        bank->next_group_sequence == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (!existing) {
        status = reserve_groups(bank, bank->group_count + 1U);
        if (status != UMI_STATUS_OK) return status;
        position = bank->group_count;
        bank->group_count += 1U;
    }
    copy = *group;
    normalize_group(&copy);
    if (copy.sequence == 0U && existing) {
        copy.sequence = bank->groups[position].sequence;
    } else if (copy.sequence == 0U) {
        copy.sequence = bank->next_group_sequence;
        bank->next_group_sequence += 1U;
    } else if (copy.sequence >= bank->next_group_sequence) {
        bank->next_group_sequence = copy.sequence + 1U;
    }
    bank->revision += 1U;
    copy.revision = bank->revision;
    bank->groups[position] = copy;
    if (bank->group_count > 1U) {
        qsort(bank->groups, bank->group_count, sizeof(bank->groups[0]),
              compare_groups);
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_register_bank_upsert_register(
    UmiDebugRegisterBank *bank,
    const UmiDebugRegisterValue *register_value)
{
    UmiDebugRegisterValue copy;
    size_t position;
    UmiStatus status;
    int existing;

    if (bank == NULL || !valid_register(register_value)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_group_index(bank, register_value->group_id) == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    position = find_register_index(bank, register_value->id);
    existing = position != SIZE_MAX;
    if (!existing && register_value->sequence == 0U &&
        bank->next_register_sequence == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (!existing) {
        status = reserve_registers(bank, bank->register_count + 1U);
        if (status != UMI_STATUS_OK) return status;
        position = bank->register_count;
        bank->register_count += 1U;
    }
    copy = *register_value;
    normalize_register(&copy);
    if (copy.sequence == 0U && existing) {
        copy.sequence = bank->registers[position].sequence;
    } else if (copy.sequence == 0U) {
        copy.sequence = bank->next_register_sequence;
        bank->next_register_sequence += 1U;
    } else if (copy.sequence >= bank->next_register_sequence) {
        bank->next_register_sequence = copy.sequence + 1U;
    }
    bank->revision += 1U;
    copy.revision = bank->revision;
    bank->registers[position] = copy;
    if (bank->register_count > 1U) {
        qsort(bank->registers, bank->register_count,
              sizeof(bank->registers[0]), compare_registers);
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_register_bank_remove_group(
    UmiDebugRegisterBank *bank,
    const char *group_id)
{
    size_t group_position;
    size_t read_position;
    size_t write_position = 0U;

    if (bank == NULL || group_id == NULL || group_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    group_position = find_group_index(bank, group_id);
    if (group_position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;

    for (read_position = 0U; read_position < bank->register_count;
         ++read_position) {
        if (strcmp(bank->registers[read_position].group_id, group_id) == 0) {
            if (strcmp(bank->selected_register_id,
                       bank->registers[read_position].id) == 0) {
                bank->selected_register_id[0] = '\0';
            }
            continue;
        }
        if (write_position != read_position) {
            bank->registers[write_position] = bank->registers[read_position];
        }
        write_position += 1U;
    }
    if (write_position < bank->register_count) {
        (void)memset(&bank->registers[write_position], 0,
                     (bank->register_count - write_position) *
                         sizeof(bank->registers[0]));
        bank->register_count = write_position;
    }
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

UmiStatus umi_debug_register_bank_remove_register(
    UmiDebugRegisterBank *bank,
    const char *register_id)
{
    size_t position;

    if (bank == NULL || register_id == NULL || register_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_register_index(bank, register_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (position + 1U < bank->register_count) {
        (void)memmove(&bank->registers[position],
                      &bank->registers[position + 1U],
                      (bank->register_count - position - 1U) *
                          sizeof(bank->registers[0]));
    }
    bank->register_count -= 1U;
    (void)memset(&bank->registers[bank->register_count], 0,
                 sizeof(bank->registers[bank->register_count]));
    if (strcmp(bank->selected_register_id, register_id) == 0) {
        bank->selected_register_id[0] = '\0';
    }
    bank->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_register_bank_set_group_expanded(
    UmiDebugRegisterBank *bank,
    const char *group_id,
    int expanded)
{
    size_t position;
    int normalized;

    if (bank == NULL || group_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    position = find_group_index(bank, group_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    normalized = expanded != 0;
    if (bank->groups[position].expanded != normalized) {
        bank->groups[position].expanded = normalized;
        bank->revision += 1U;
        bank->groups[position].revision = bank->revision;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_register_bank_select(
    UmiDebugRegisterBank *bank,
    const char *register_id)
{
    size_t position;

    if (bank == NULL || register_id == NULL || register_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_register_index(bank, register_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (!bank->registers[position].available) return UMI_STATUS_UNAVAILABLE;
    if (strcmp(bank->selected_register_id, register_id) != 0) {
        UmiStatus status = copy_text(bank->selected_register_id,
                                     sizeof(bank->selected_register_id),
                                     register_id);
        if (status != UMI_STATUS_OK) return status;
        bank->revision += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_register_bank_update_value(
    UmiDebugRegisterBank *bank,
    const char *register_id,
    const char *value,
    const char *raw_value,
    int changed)
{
    size_t position;
    UmiStatus status;

    if (bank == NULL || register_id == NULL || value == NULL ||
        raw_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_register_index(bank, register_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (!bank->registers[position].available) return UMI_STATUS_UNAVAILABLE;
    if (bank->registers[position].read_only) return UMI_STATUS_PERMISSION_DENIED;
    if (strlen(value) >= sizeof(bank->registers[position].value) ||
        strlen(raw_value) >= sizeof(bank->registers[position].raw_value)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = copy_text(bank->registers[position].value,
                       sizeof(bank->registers[position].value), value);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(bank->registers[position].raw_value,
                       sizeof(bank->registers[position].raw_value), raw_value);
    if (status != UMI_STATUS_OK) return status;
    bank->registers[position].changed = changed != 0;
    bank->revision += 1U;
    bank->registers[position].revision = bank->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_register_bank_find_group(
    const UmiDebugRegisterBank *bank,
    const char *group_id,
    UmiDebugRegisterGroup *out_group)
{
    size_t position;

    if (bank == NULL || group_id == NULL || out_group == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_group_index(bank, group_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_group = bank->groups[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_register_bank_find_register(
    const UmiDebugRegisterBank *bank,
    const char *register_id,
    UmiDebugRegisterValue *out_register)
{
    size_t position;

    if (bank == NULL || register_id == NULL || out_register == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_register_index(bank, register_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_register = bank->registers[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_register_bank_group_at(
    const UmiDebugRegisterBank *bank,
    size_t index,
    UmiDebugRegisterGroup *out_group)
{
    if (bank == NULL || out_group == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= bank->group_count) return UMI_STATUS_NOT_FOUND;
    *out_group = bank->groups[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_register_bank_register_at(
    const UmiDebugRegisterBank *bank,
    size_t index,
    UmiDebugRegisterValue *out_register)
{
    if (bank == NULL || out_register == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= bank->register_count) return UMI_STATUS_NOT_FOUND;
    *out_register = bank->registers[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_register_bank_visible_register_at(
    const UmiDebugRegisterBank *bank,
    size_t visible_index,
    UmiDebugRegisterValue *out_register)
{
    size_t group_position;
    size_t register_position;
    size_t current_visible = 0U;

    if (bank == NULL || out_register == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (group_position = 0U; group_position < bank->group_count;
         ++group_position) {
        const UmiDebugRegisterGroup *group = &bank->groups[group_position];
        if (!group->available || !group->expanded) continue;
        for (register_position = 0U;
             register_position < bank->register_count; ++register_position) {
            const UmiDebugRegisterValue *register_value =
                &bank->registers[register_position];
            if (!register_value->available ||
                strcmp(register_value->group_id, group->id) != 0) {
                continue;
            }
            if (current_visible == visible_index) {
                *out_register = *register_value;
                return UMI_STATUS_OK;
            }
            current_visible += 1U;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_debug_register_bank_selected(
    const UmiDebugRegisterBank *bank,
    UmiDebugRegisterValue *out_register)
{
    return umi_debug_register_bank_find_register(
        bank, bank != NULL ? bank->selected_register_id : NULL, out_register);
}

UmiStatus umi_debug_register_bank_snapshot(
    const UmiDebugRegisterBank *bank,
    UmiDebugRegisterBankSnapshot *out_snapshot)
{
    size_t position;

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
    for (position = 0U; position < bank->register_count; ++position) {
        if (register_visible(bank, &bank->registers[position])) {
            out_snapshot->visible_register_count += 1U;
        }
        if (bank->registers[position].changed) {
            out_snapshot->changed_register_count += 1U;
        }
        if (bank->registers[position].read_only) {
            out_snapshot->read_only_register_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_debug_register_bank_group_count(const UmiDebugRegisterBank *bank)
{
    return bank != NULL ? bank->group_count : 0U;
}

size_t umi_debug_register_bank_register_count(const UmiDebugRegisterBank *bank)
{
    return bank != NULL ? bank->register_count : 0U;
}

size_t umi_debug_register_bank_visible_count(const UmiDebugRegisterBank *bank)
{
    UmiDebugRegisterBankSnapshot snapshot;
    if (umi_debug_register_bank_snapshot(bank, &snapshot) != UMI_STATUS_OK) {
        return 0U;
    }
    return snapshot.visible_register_count;
}

uint64_t umi_debug_register_bank_revision(const UmiDebugRegisterBank *bank)
{
    return bank != NULL ? bank->revision : 0U;
}
