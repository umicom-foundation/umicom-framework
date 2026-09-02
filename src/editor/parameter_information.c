/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/parameter_information.c
 *
 * PURPOSE:
 *   Implement dynamic parameter metadata, signature grouping, deterministic
 *   ordering and active-parameter selection for editor assistance services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/parameter_information.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorParameterInformation {
    UmiEditorParameterInformationItem *items;
    size_t count;
    size_t capacity;
    char active_signature_id[UMI_EDITOR_PARAMETER_SIGNATURE_ID_CAPACITY];
    size_t active_ordinal;
    uint64_t revision;
    int has_active_parameter;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the validate item operation used by this module and its client applications. */
static UmiStatus validate_item(const UmiEditorParameterInformationItem *item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || item->struct_size != (uint32_t)sizeof(*item) ||
        item->api_version != UMI_EDITOR_PARAMETER_INFORMATION_API_VERSION ||
        !terminated(item->signature_id, sizeof(item->signature_id)) ||
        item->signature_id[0] == '\0' ||
        !terminated(item->label, sizeof(item->label)) ||
        item->label[0] == '\0' ||
        !terminated(item->type_name, sizeof(item->type_name)) ||
        !terminated(item->documentation, sizeof(item->documentation)) ||
        !terminated(item->default_value, sizeof(item->default_value)) ||
        item->label_end < item->label_start) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the reserve items operation used by this module and its client applications. */
static UmiStatus reserve_items(UmiEditorParameterInformation *information,
                               size_t required)
{
    size_t capacity;
    UmiEditorParameterInformationItem *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= information->capacity) return UMI_STATUS_OK;
    capacity = information->capacity > 0U ? information->capacity : 16U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorParameterInformationItem *)realloc(
        information->items, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    information->items = replacement;
    information->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find item operation used by this module and its client applications. */
static size_t find_item(const UmiEditorParameterInformation *information,
                        const char *signature_id,
                        size_t ordinal)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL || signature_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < information->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (information->items[index].ordinal == ordinal &&
            strcmp(information->items[index].signature_id, signature_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/* Provide the compare items operation used by this module and its client applications. */
static int compare_items(const void *left_pointer, const void *right_pointer)
{
    const UmiEditorParameterInformationItem *left =
        (const UmiEditorParameterInformationItem *)left_pointer;
    const UmiEditorParameterInformationItem *right =
        (const UmiEditorParameterInformationItem *)right_pointer;
    int order = strcmp(left->signature_id, right->signature_id);

    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->ordinal < right->ordinal) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->ordinal > right->ordinal) return 1;
    return strcmp(left->label, right->label);
}

/*
 * Provide the clear active if missing operation used by this module and its client
 * applications.
 */
static void clear_active_if_missing(UmiEditorParameterInformation *information)
{
    /* Apply this operation only while the related capability or state is available. */
    if (information->has_active_parameter &&
        find_item(information, information->active_signature_id,
                  information->active_ordinal) == SIZE_MAX) {
        information->active_signature_id[0] = '\0';
        information->active_ordinal = 0U;
        information->has_active_parameter = 0;
    }
}

/*
 * Initialise editor parameter information from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_parameter_information_create(
    UmiEditorParameterInformation **out_information)
{
    UmiEditorParameterInformation *information;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_information == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_information = NULL;
    information = (UmiEditorParameterInformation *)calloc(
        1U, sizeof(*information));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    information->revision = 1U;
    *out_information = information;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor parameter information so the same storage can be
 * reused safely.
 */
void umi_editor_parameter_information_destroy(
    UmiEditorParameterInformation *information)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL) return;
    free(information->items);
    information->items = NULL;
    free(information);
}

/*
 * Release or reset state held by editor parameter information so the same storage can be
 * reused safely.
 */
UmiStatus umi_editor_parameter_information_clear(
    UmiEditorParameterInformation *information)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    information->count = 0U;
    information->active_signature_id[0] = '\0';
    information->active_ordinal = 0U;
    information->has_active_parameter = 0;
    information->revision = next_revision(information->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor parameter information upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_parameter_information_upsert(
    UmiEditorParameterInformation *information,
    const UmiEditorParameterInformationItem *item)
{
    UmiEditorParameterInformationItem stored;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL || validate_item(item) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_item(information, item->signature_id, item->ordinal);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        status = reserve_items(information, information->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        index = information->count++;
    }
    stored = *item;
    stored.required = item->required != 0;
    stored.variadic = item->variadic != 0;
    stored.deprecated = item->deprecated != 0;
    information->items[index] = stored;
    information->revision = next_revision(information->revision);
    return UMI_STATUS_OK;
}

/*
 * Remove editor parameter information while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_parameter_information_remove(
    UmiEditorParameterInformation *information,
    const char *signature_id,
    size_t ordinal)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL || signature_id == NULL ||
        signature_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_item(information, signature_id, ordinal);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < information->count) {
        (void)memmove(&information->items[index],
                      &information->items[index + 1U],
                      (information->count - index - 1U) *
                          sizeof(*information->items));
    }
    --information->count;
    clear_active_if_missing(information);
    information->revision = next_revision(information->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor parameter information remove signature operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_parameter_information_remove_signature(
    UmiEditorParameterInformation *information,
    const char *signature_id)
{
    size_t read_index;
    size_t write_index = 0U;
    size_t removed = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL || signature_id == NULL ||
        signature_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (read_index = 0U; read_index < information->count; ++read_index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(information->items[read_index].signature_id,
                   signature_id) == 0) {
            ++removed;
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (write_index != read_index) {
            information->items[write_index] = information->items[read_index];
        }
        ++write_index;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (removed == 0U) return UMI_STATUS_NOT_FOUND;
    information->count = write_index;
    clear_active_if_missing(information);
    information->revision = next_revision(information->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor parameter information sort operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_parameter_information_sort(
    UmiEditorParameterInformation *information)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (information->count > 1U) {
        qsort(information->items, information->count,
              sizeof(*information->items), compare_items);
    }
    information->revision = next_revision(information->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor parameter information set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_parameter_information_set_active(
    UmiEditorParameterInformation *information,
    const char *signature_id,
    size_t ordinal)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL || signature_id == NULL ||
        signature_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_item(information, signature_id, ordinal) == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    (void)memcpy(information->active_signature_id, signature_id,
                 strlen(signature_id) + 1U);
    information->active_ordinal = ordinal;
    information->has_active_parameter = 1;
    information->revision = next_revision(information->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor parameter information clear active operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_parameter_information_clear_active(
    UmiEditorParameterInformation *information)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    information->active_signature_id[0] = '\0';
    information->active_ordinal = 0U;
    information->has_active_parameter = 0;
    information->revision = next_revision(information->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor parameter information while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_parameter_information_at(
    const UmiEditorParameterInformation *information,
    size_t index,
    UmiEditorParameterInformationItem *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= information->count) return UMI_STATUS_NOT_FOUND;
    *out_item = information->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find editor parameter information for signature while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_editor_parameter_information_for_signature_at(
    const UmiEditorParameterInformation *information,
    const char *signature_id,
    size_t signature_index,
    UmiEditorParameterInformationItem *out_item)
{
    size_t index;
    size_t position = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL || signature_id == NULL ||
        signature_id[0] == '\0' || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < information->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(information->items[index].signature_id, signature_id) != 0) {
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (position++ == signature_index) {
            *out_item = information->items[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the editor parameter information active operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_parameter_information_active(
    const UmiEditorParameterInformation *information,
    UmiEditorParameterInformationItem *out_item)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!information->has_active_parameter) return UMI_STATUS_NOT_FOUND;
    index = find_item(information, information->active_signature_id,
                      information->active_ordinal);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = information->items[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor parameter information snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_parameter_information_snapshot(
    const UmiEditorParameterInformation *information,
    UmiEditorParameterInformationSnapshot *out_snapshot)
{
    size_t index;
    size_t comparison;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_PARAMETER_INFORMATION_API_VERSION;
    out_snapshot->parameter_count = information->count;
    out_snapshot->active_ordinal = information->active_ordinal;
    out_snapshot->revision = information->revision;
    out_snapshot->has_active_parameter = information->has_active_parameter;
    (void)memcpy(out_snapshot->active_signature_id,
                 information->active_signature_id,
                 strlen(information->active_signature_id) + 1U);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < information->count; ++index) {
        int first_signature = 1;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (information->items[index].required) {
            ++out_snapshot->required_parameter_count;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (information->items[index].deprecated) {
            ++out_snapshot->deprecated_parameter_count;
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for (comparison = 0U; comparison < index; ++comparison) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (strcmp(information->items[index].signature_id,
                       information->items[comparison].signature_id) == 0) {
                first_signature = 0;
                break;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (first_signature) ++out_snapshot->signature_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor parameter information without
 * changing their state.
 */
size_t umi_editor_parameter_information_count(
    const UmiEditorParameterInformation *information)
{
    return information != NULL ? information->count : 0U;
}

/*
 * Return the number of records represented by editor parameter information signature
 * without changing their state.
 */
size_t umi_editor_parameter_information_signature_count(
    const UmiEditorParameterInformation *information,
    const char *signature_id)
{
    size_t index;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (information == NULL || signature_id == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < information->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(information->items[index].signature_id, signature_id) == 0) {
            ++count;
        }
    }
    return count;
}

/*
 * Provide the editor parameter information revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_parameter_information_revision(
    const UmiEditorParameterInformation *information)
{
    return information != NULL ? information->revision : 0U;
}
