/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/import_organisation.c
 *
 * PURPOSE:
 *   Implement cross-language import planning with configurable comparison,
 *   stable grouping and preservation-aware removal decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/import_organisation.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorImportOrganisationPlan {
    UmiEditorImportItem *items;
    size_t count;
    size_t capacity;
    UmiEditorImportOrganisationOptions options;
    uint64_t revision;
    int analysed;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static int text_compare(const char *left, const char *right, int case_sensitive)
{
    if (case_sensitive) return strcmp(left, right);
    while (*left != '\0' && *right != '\0') {
        int left_value = tolower((unsigned char)*left);
        int right_value = tolower((unsigned char)*right);
        if (left_value != right_value) return left_value - right_value;
        ++left;
        ++right;
    }
    return (unsigned char)*left - (unsigned char)*right;
}

static UmiStatus validate_item(const UmiEditorImportItem *item)
{
    if (item == NULL || item->struct_size != (uint32_t)sizeof(*item) ||
        item->api_version != UMI_EDITOR_IMPORT_ORGANISATION_API_VERSION ||
        !terminated(item->id, sizeof(item->id)) || item->id[0] == '\0' ||
        !terminated(item->provider_id, sizeof(item->provider_id)) ||
        item->provider_id[0] == '\0' ||
        !terminated(item->canonical_source, sizeof(item->canonical_source)) ||
        item->canonical_source[0] == '\0' ||
        !terminated(item->imported_symbol, sizeof(item->imported_symbol)) ||
        !terminated(item->original_text, sizeof(item->original_text)) ||
        item->original_text[0] == '\0' ||
        item->kind < UMI_EDITOR_IMPORT_SYSTEM ||
        item->kind > UMI_EDITOR_IMPORT_MODULE ||
        umi_editor_source_location_validate(&item->location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus validate_options(
    const UmiEditorImportOrganisationOptions *options)
{
    if (options == NULL ||
        options->struct_size != (uint32_t)sizeof(*options) ||
        options->api_version != UMI_EDITOR_IMPORT_ORGANISATION_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus reserve_items(UmiEditorImportOrganisationPlan *plan,
                               size_t required)
{
    size_t capacity;
    UmiEditorImportItem *replacement;

    if (required <= plan->capacity) return UMI_STATUS_OK;
    capacity = plan->capacity > 0U ? plan->capacity : 16U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorImportItem *)realloc(
        plan->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    plan->items = replacement;
    plan->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_item(const UmiEditorImportOrganisationPlan *plan,
                        const char *item_id)
{
    size_t index;

    if (plan == NULL || item_id == NULL) return SIZE_MAX;
    for (index = 0U; index < plan->count; ++index) {
        if (strcmp(plan->items[index].id, item_id) == 0) return index;
    }
    return SIZE_MAX;
}

static int same_import(const UmiEditorImportOrganisationPlan *plan,
                       const UmiEditorImportItem *left,
                       const UmiEditorImportItem *right)
{
    return text_compare(left->canonical_source, right->canonical_source,
                        plan->options.case_sensitive) == 0 &&
           text_compare(left->imported_symbol, right->imported_symbol,
                        plan->options.case_sensitive) == 0;
}

static int compare_items(const UmiEditorImportOrganisationPlan *plan,
                         const UmiEditorImportItem *left,
                         const UmiEditorImportItem *right)
{
    int order;

    if (left->group_order < right->group_order) return -1;
    if (left->group_order > right->group_order) return 1;
    if (left->kind < right->kind) return -1;
    if (left->kind > right->kind) return 1;
    order = text_compare(left->canonical_source, right->canonical_source,
                         plan->options.case_sensitive);
    if (order != 0) return order;
    order = text_compare(left->imported_symbol, right->imported_symbol,
                         plan->options.case_sensitive);
    return order != 0 ? order : strcmp(left->id, right->id);
}

UmiStatus umi_editor_import_organisation_plan_create(
    UmiEditorImportOrganisationPlan **out_plan)
{
    UmiEditorImportOrganisationPlan *plan;

    if (out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_plan = NULL;
    plan = (UmiEditorImportOrganisationPlan *)calloc(1U, sizeof(*plan));
    if (plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    plan->options.struct_size = (uint32_t)sizeof(plan->options);
    plan->options.api_version = UMI_EDITOR_IMPORT_ORGANISATION_API_VERSION;
    plan->options.case_sensitive = 1;
    plan->options.remove_unused = 1;
    plan->options.remove_duplicates = 1;
    plan->options.separate_groups = 1;
    plan->revision = 1U;
    *out_plan = plan;
    return UMI_STATUS_OK;
}

void umi_editor_import_organisation_plan_destroy(
    UmiEditorImportOrganisationPlan *plan)
{
    if (plan == NULL) return;
    free(plan->items);
    plan->items = NULL;
    free(plan);
}

UmiStatus umi_editor_import_organisation_plan_clear(
    UmiEditorImportOrganisationPlan *plan)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    plan->count = 0U;
    plan->analysed = 0;
    plan->revision = next_revision(plan->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_import_organisation_plan_set_options(
    UmiEditorImportOrganisationPlan *plan,
    const UmiEditorImportOrganisationOptions *options)
{
    if (plan == NULL || validate_options(options) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    plan->options = *options;
    plan->options.case_sensitive = options->case_sensitive != 0;
    plan->options.remove_unused = options->remove_unused != 0;
    plan->options.remove_duplicates = options->remove_duplicates != 0;
    plan->options.separate_groups = options->separate_groups != 0;
    plan->analysed = 0;
    plan->revision = next_revision(plan->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_import_organisation_plan_upsert(
    UmiEditorImportOrganisationPlan *plan,
    const UmiEditorImportItem *item)
{
    UmiEditorImportItem stored;
    size_t index;
    UmiStatus status;

    if (plan == NULL || validate_item(item) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_item(plan, item->id);
    if (index == SIZE_MAX) {
        status = reserve_items(plan, plan->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        index = plan->count++;
    }
    stored = *item;
    stored.used = item->used != 0;
    stored.pinned = item->pinned != 0;
    stored.duplicate = 0;
    stored.removable = 0;
    plan->items[index] = stored;
    plan->analysed = 0;
    plan->revision = next_revision(plan->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_import_organisation_plan_remove(
    UmiEditorImportOrganisationPlan *plan,
    const char *item_id)
{
    size_t index;

    if (plan == NULL || item_id == NULL || item_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_item(plan, item_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < plan->count) {
        (void)memmove(&plan->items[index], &plan->items[index + 1U],
                      (plan->count - index - 1U) * sizeof(*plan->items));
    }
    --plan->count;
    plan->analysed = 0;
    plan->revision = next_revision(plan->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_import_organisation_plan_analyse(
    UmiEditorImportOrganisationPlan *plan)
{
    size_t index;
    size_t comparison;

    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < plan->count; ++index) {
        plan->items[index].duplicate = 0;
        plan->items[index].removable = 0;
    }
    for (index = 0U; index < plan->count; ++index) {
        for (comparison = 0U; comparison < index; ++comparison) {
            if (!same_import(plan, &plan->items[index],
                             &plan->items[comparison])) {
                continue;
            }
            if (plan->items[index].pinned &&
                !plan->items[comparison].pinned) {
                plan->items[comparison].duplicate = 1;
                plan->items[comparison].removable =
                    plan->options.remove_duplicates;
            } else {
                plan->items[index].duplicate = 1;
                plan->items[index].removable =
                    plan->options.remove_duplicates &&
                    !plan->items[index].pinned;
            }
            break;
        }
        if (!plan->items[index].used && !plan->items[index].pinned &&
            plan->options.remove_unused) {
            plan->items[index].removable = 1;
        }
    }
    plan->analysed = 1;
    plan->revision = next_revision(plan->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_import_organisation_plan_sort(
    UmiEditorImportOrganisationPlan *plan)
{
    size_t index;

    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 1U; index < plan->count; ++index) {
        UmiEditorImportItem item = plan->items[index];
        size_t position = index;

        while (position > 0U &&
               compare_items(plan, &item, &plan->items[position - 1U]) < 0) {
            plan->items[position] = plan->items[position - 1U];
            --position;
        }
        plan->items[position] = item;
    }
    plan->revision = next_revision(plan->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_import_organisation_plan_at(
    const UmiEditorImportOrganisationPlan *plan,
    size_t index,
    UmiEditorImportItem *out_item)
{
    if (plan == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= plan->count) return UMI_STATUS_NOT_FOUND;
    *out_item = plan->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_import_organisation_plan_kept_at(
    const UmiEditorImportOrganisationPlan *plan,
    size_t kept_index,
    UmiEditorImportItem *out_item)
{
    size_t index;
    size_t position = 0U;

    if (plan == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!plan->analysed) return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < plan->count; ++index) {
        if (plan->items[index].removable) continue;
        if (position++ == kept_index) {
            *out_item = plan->items[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_editor_import_organisation_plan_snapshot(
    const UmiEditorImportOrganisationPlan *plan,
    UmiEditorImportOrganisationSnapshot *out_snapshot)
{
    size_t index;
    size_t comparison;

    if (plan == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_IMPORT_ORGANISATION_API_VERSION;
    out_snapshot->import_count = plan->count;
    out_snapshot->revision = plan->revision;
    out_snapshot->analysed = plan->analysed;
    for (index = 0U; index < plan->count; ++index) {
        int first_group = 1;
        if (plan->items[index].duplicate) ++out_snapshot->duplicate_count;
        if (!plan->items[index].used) ++out_snapshot->unused_count;
        if (plan->items[index].removable) ++out_snapshot->removable_count;
        for (comparison = 0U; comparison < index; ++comparison) {
            if (plan->items[index].kind == plan->items[comparison].kind &&
                plan->items[index].group_order ==
                    plan->items[comparison].group_order) {
                first_group = 0;
                break;
            }
        }
        if (first_group) ++out_snapshot->group_count;
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_import_organisation_plan_count(
    const UmiEditorImportOrganisationPlan *plan)
{
    return plan != NULL ? plan->count : 0U;
}

uint64_t umi_editor_import_organisation_plan_revision(
    const UmiEditorImportOrganisationPlan *plan)
{
    return plan != NULL ? plan->revision : 0U;
}
