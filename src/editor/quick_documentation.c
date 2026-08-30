/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/quick_documentation.c
 *
 * PURPOSE:
 *   Implement structured documentation ownership, symbol selection, trust
 *   metadata and deterministic section ordering for reusable editor clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/quick_documentation.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorQuickDocumentation {
    UmiEditorQuickDocumentationSection *sections;
    size_t count;
    size_t capacity;
    char selected_symbol_id[UMI_EDITOR_DOCUMENTATION_SYMBOL_CAPACITY];
    uint64_t revision;
    int pinned;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus validate_section(
    const UmiEditorQuickDocumentationSection *section)
{
    if (section == NULL ||
        section->struct_size != (uint32_t)sizeof(*section) ||
        section->api_version != UMI_EDITOR_QUICK_DOCUMENTATION_API_VERSION ||
        !terminated(section->id, sizeof(section->id)) ||
        section->id[0] == '\0' ||
        !terminated(section->symbol_id, sizeof(section->symbol_id)) ||
        section->symbol_id[0] == '\0' ||
        !terminated(section->provider_id, sizeof(section->provider_id)) ||
        section->provider_id[0] == '\0' ||
        !terminated(section->title, sizeof(section->title)) ||
        !terminated(section->language_id, sizeof(section->language_id)) ||
        !terminated(section->content, sizeof(section->content)) ||
        section->content[0] == '\0' ||
        !terminated(section->source_uri, sizeof(section->source_uri)) ||
        section->section_kind < UMI_EDITOR_DOCUMENTATION_SUMMARY ||
        section->section_kind > UMI_EDITOR_DOCUMENTATION_LINK ||
        section->content_kind < UMI_EDITOR_DOCUMENTATION_PLAIN_TEXT ||
        section->content_kind > UMI_EDITOR_DOCUMENTATION_SOURCE_CODE ||
        umi_editor_source_location_validate(&section->location) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (section->content_kind == UMI_EDITOR_DOCUMENTATION_SOURCE_CODE &&
        section->language_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (section->section_kind == UMI_EDITOR_DOCUMENTATION_LINK &&
        section->source_uri[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus reserve_sections(UmiEditorQuickDocumentation *documentation,
                                  size_t required)
{
    size_t capacity;
    UmiEditorQuickDocumentationSection *replacement;

    if (required <= documentation->capacity) return UMI_STATUS_OK;
    capacity = documentation->capacity > 0U ? documentation->capacity : 8U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorQuickDocumentationSection *)realloc(
        documentation->sections, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    documentation->sections = replacement;
    documentation->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_section(const UmiEditorQuickDocumentation *documentation,
                           const char *section_id)
{
    size_t index;

    if (documentation == NULL || section_id == NULL) return SIZE_MAX;
    for (index = 0U; index < documentation->count; ++index) {
        if (strcmp(documentation->sections[index].id, section_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static int compare_sections(const void *left_pointer,
                            const void *right_pointer)
{
    const UmiEditorQuickDocumentationSection *left =
        (const UmiEditorQuickDocumentationSection *)left_pointer;
    const UmiEditorQuickDocumentationSection *right =
        (const UmiEditorQuickDocumentationSection *)right_pointer;
    int order = strcmp(left->symbol_id, right->symbol_id);

    if (order != 0) return order;
    if (left->order < right->order) return -1;
    if (left->order > right->order) return 1;
    if (left->section_kind < right->section_kind) return -1;
    if (left->section_kind > right->section_kind) return 1;
    return strcmp(left->id, right->id);
}

static int symbol_exists(const UmiEditorQuickDocumentation *documentation,
                         const char *symbol_id)
{
    size_t index;

    for (index = 0U; index < documentation->count; ++index) {
        if (strcmp(documentation->sections[index].symbol_id, symbol_id) == 0) {
            return 1;
        }
    }
    return 0;
}

static void clear_selection_if_missing(
    UmiEditorQuickDocumentation *documentation)
{
    if (documentation->selected_symbol_id[0] != '\0' &&
        !symbol_exists(documentation, documentation->selected_symbol_id)) {
        documentation->selected_symbol_id[0] = '\0';
        documentation->pinned = 0;
    }
}

UmiStatus umi_editor_quick_documentation_create(
    UmiEditorQuickDocumentation **out_documentation)
{
    UmiEditorQuickDocumentation *documentation;

    if (out_documentation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_documentation = NULL;
    documentation = (UmiEditorQuickDocumentation *)calloc(
        1U, sizeof(*documentation));
    if (documentation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    documentation->revision = 1U;
    *out_documentation = documentation;
    return UMI_STATUS_OK;
}

void umi_editor_quick_documentation_destroy(
    UmiEditorQuickDocumentation *documentation)
{
    if (documentation == NULL) return;
    free(documentation->sections);
    documentation->sections = NULL;
    free(documentation);
}

UmiStatus umi_editor_quick_documentation_clear(
    UmiEditorQuickDocumentation *documentation)
{
    if (documentation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    documentation->count = 0U;
    documentation->selected_symbol_id[0] = '\0';
    documentation->pinned = 0;
    documentation->revision = next_revision(documentation->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_quick_documentation_upsert(
    UmiEditorQuickDocumentation *documentation,
    const UmiEditorQuickDocumentationSection *section)
{
    UmiEditorQuickDocumentationSection stored;
    size_t index;
    UmiStatus status;

    if (documentation == NULL || validate_section(section) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_section(documentation, section->id);
    if (index == SIZE_MAX) {
        status = reserve_sections(documentation, documentation->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        index = documentation->count++;
    }
    stored = *section;
    stored.trusted = section->trusted != 0;
    stored.visible = section->visible != 0;
    documentation->sections[index] = stored;
    if (documentation->selected_symbol_id[0] == '\0') {
        (void)memcpy(documentation->selected_symbol_id, stored.symbol_id,
                     strlen(stored.symbol_id) + 1U);
    }
    documentation->revision = next_revision(documentation->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_quick_documentation_remove(
    UmiEditorQuickDocumentation *documentation,
    const char *section_id)
{
    size_t index;

    if (documentation == NULL || section_id == NULL || section_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_section(documentation, section_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < documentation->count) {
        (void)memmove(&documentation->sections[index],
                      &documentation->sections[index + 1U],
                      (documentation->count - index - 1U) *
                          sizeof(*documentation->sections));
    }
    --documentation->count;
    clear_selection_if_missing(documentation);
    documentation->revision = next_revision(documentation->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_quick_documentation_remove_symbol(
    UmiEditorQuickDocumentation *documentation,
    const char *symbol_id)
{
    size_t read_index;
    size_t write_index = 0U;
    size_t removed = 0U;

    if (documentation == NULL || symbol_id == NULL || symbol_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (read_index = 0U; read_index < documentation->count; ++read_index) {
        if (strcmp(documentation->sections[read_index].symbol_id,
                   symbol_id) == 0) {
            ++removed;
            continue;
        }
        if (read_index != write_index) {
            documentation->sections[write_index] =
                documentation->sections[read_index];
        }
        ++write_index;
    }
    if (removed == 0U) return UMI_STATUS_NOT_FOUND;
    documentation->count = write_index;
    clear_selection_if_missing(documentation);
    documentation->revision = next_revision(documentation->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_quick_documentation_sort(
    UmiEditorQuickDocumentation *documentation)
{
    if (documentation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (documentation->count > 1U) {
        qsort(documentation->sections, documentation->count,
              sizeof(*documentation->sections), compare_sections);
    }
    documentation->revision = next_revision(documentation->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_quick_documentation_select_symbol(
    UmiEditorQuickDocumentation *documentation,
    const char *symbol_id)
{
    if (documentation == NULL || symbol_id == NULL || symbol_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!symbol_exists(documentation, symbol_id)) return UMI_STATUS_NOT_FOUND;
    (void)memcpy(documentation->selected_symbol_id, symbol_id,
                 strlen(symbol_id) + 1U);
    documentation->revision = next_revision(documentation->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_quick_documentation_set_pinned(
    UmiEditorQuickDocumentation *documentation,
    int pinned)
{
    if (documentation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (pinned != 0 && documentation->selected_symbol_id[0] == '\0') {
        return UMI_STATUS_INVALID_STATE;
    }
    documentation->pinned = pinned != 0;
    documentation->revision = next_revision(documentation->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_quick_documentation_at(
    const UmiEditorQuickDocumentation *documentation,
    size_t index,
    UmiEditorQuickDocumentationSection *out_section)
{
    if (documentation == NULL || out_section == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= documentation->count) return UMI_STATUS_NOT_FOUND;
    *out_section = documentation->sections[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_quick_documentation_selected_at(
    const UmiEditorQuickDocumentation *documentation,
    size_t selected_index,
    UmiEditorQuickDocumentationSection *out_section)
{
    size_t index;
    size_t position = 0U;

    if (documentation == NULL || out_section == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (documentation->selected_symbol_id[0] == '\0') {
        return UMI_STATUS_NOT_FOUND;
    }
    for (index = 0U; index < documentation->count; ++index) {
        if (!documentation->sections[index].visible ||
            strcmp(documentation->sections[index].symbol_id,
                   documentation->selected_symbol_id) != 0) {
            continue;
        }
        if (position++ == selected_index) {
            *out_section = documentation->sections[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_editor_quick_documentation_snapshot(
    const UmiEditorQuickDocumentation *documentation,
    UmiEditorQuickDocumentationSnapshot *out_snapshot)
{
    size_t index;

    if (documentation == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_QUICK_DOCUMENTATION_API_VERSION;
    out_snapshot->section_count = documentation->count;
    out_snapshot->revision = documentation->revision;
    out_snapshot->pinned = documentation->pinned;
    (void)memcpy(out_snapshot->selected_symbol_id,
                 documentation->selected_symbol_id,
                 strlen(documentation->selected_symbol_id) + 1U);
    for (index = 0U; index < documentation->count; ++index) {
        if (documentation->sections[index].trusted) {
            ++out_snapshot->trusted_section_count;
        }
        if (documentation->sections[index].section_kind ==
            UMI_EDITOR_DOCUMENTATION_LINK) {
            ++out_snapshot->link_section_count;
        }
        if (documentation->sections[index].visible &&
            strcmp(documentation->sections[index].symbol_id,
                   documentation->selected_symbol_id) == 0) {
            ++out_snapshot->selected_section_count;
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_quick_documentation_count(
    const UmiEditorQuickDocumentation *documentation)
{
    return documentation != NULL ? documentation->count : 0U;
}

size_t umi_editor_quick_documentation_symbol_count(
    const UmiEditorQuickDocumentation *documentation,
    const char *symbol_id)
{
    size_t index;
    size_t count = 0U;

    if (documentation == NULL || symbol_id == NULL) return 0U;
    for (index = 0U; index < documentation->count; ++index) {
        if (strcmp(documentation->sections[index].symbol_id, symbol_id) == 0) {
            ++count;
        }
    }
    return count;
}

uint64_t umi_editor_quick_documentation_revision(
    const UmiEditorQuickDocumentation *documentation)
{
    return documentation != NULL ? documentation->revision : 0U;
}
