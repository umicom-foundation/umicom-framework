/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/hover_model.c
 *
 * PURPOSE:
 *   Implement dynamic hover-section ownership, validation, deterministic
 *   ordering, active-section selection and presentation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/hover_model.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorHoverModel {
    UmiEditorHoverSection *sections;
    size_t count;
    size_t capacity;
    char active_section_id[UMI_EDITOR_HOVER_ID_CAPACITY];
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

static UmiStatus validate_section(const UmiEditorHoverSection *section)
{
    if (section == NULL ||
        section->struct_size != (uint32_t)sizeof(*section) ||
        section->api_version != UMI_EDITOR_HOVER_MODEL_API_VERSION ||
        !terminated(section->id, sizeof(section->id)) ||
        section->id[0] == '\0' ||
        !terminated(section->provider_id, sizeof(section->provider_id)) ||
        section->provider_id[0] == '\0' ||
        !terminated(section->title, sizeof(section->title)) ||
        !terminated(section->language_id, sizeof(section->language_id)) ||
        !terminated(section->content, sizeof(section->content)) ||
        section->content_kind < UMI_EDITOR_HOVER_PLAIN_TEXT ||
        section->content_kind > UMI_EDITOR_HOVER_SOURCE_CODE ||
        section->resolution_state < UMI_EDITOR_HOVER_PENDING ||
        section->resolution_state > UMI_EDITOR_HOVER_FAILED ||
        umi_editor_source_location_validate(&section->location) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (section->resolution_state == UMI_EDITOR_HOVER_READY &&
        section->content[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (section->content_kind == UMI_EDITOR_HOVER_SOURCE_CODE &&
        section->language_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus reserve_sections(UmiEditorHoverModel *model,
                                  size_t required)
{
    size_t capacity;
    UmiEditorHoverSection *replacement;

    if (required <= model->capacity) return UMI_STATUS_OK;
    capacity = model->capacity > 0U ? model->capacity : 8U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorHoverSection *)realloc(
        model->sections, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->sections = replacement;
    model->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_section(const UmiEditorHoverModel *model,
                           const char *section_id)
{
    size_t index;

    if (model == NULL || section_id == NULL) return SIZE_MAX;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->sections[index].id, section_id) == 0) return index;
    }
    return SIZE_MAX;
}

static int compare_sections(const void *left_pointer,
                            const void *right_pointer)
{
    const UmiEditorHoverSection *left =
        (const UmiEditorHoverSection *)left_pointer;
    const UmiEditorHoverSection *right =
        (const UmiEditorHoverSection *)right_pointer;
    int order;

    if (left->priority > right->priority) return -1;
    if (left->priority < right->priority) return 1;
    order = strcmp(left->provider_id, right->provider_id);
    return order != 0 ? order : strcmp(left->id, right->id);
}

UmiStatus umi_editor_hover_model_create(UmiEditorHoverModel **out_model)
{
    UmiEditorHoverModel *model;

    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiEditorHoverModel *)calloc(1U, sizeof(*model));
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

void umi_editor_hover_model_destroy(UmiEditorHoverModel *model)
{
    if (model == NULL) return;
    free(model->sections);
    model->sections = NULL;
    free(model);
}

UmiStatus umi_editor_hover_model_clear(UmiEditorHoverModel *model)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->count = 0U;
    model->active_section_id[0] = '\0';
    model->pinned = 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_hover_model_upsert(
    UmiEditorHoverModel *model,
    const UmiEditorHoverSection *section)
{
    UmiEditorHoverSection stored;
    size_t index;
    UmiStatus status;

    if (model == NULL || validate_section(section) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_section(model, section->id);
    if (index == SIZE_MAX) {
        status = reserve_sections(model, model->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        index = model->count++;
    }
    stored = *section;
    stored.trusted = section->trusted != 0;
    stored.visible = section->visible != 0;
    model->sections[index] = stored;
    if (model->active_section_id[0] == '\0') {
        (void)memcpy(model->active_section_id, stored.id,
                     strlen(stored.id) + 1U);
    }
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_hover_model_remove(
    UmiEditorHoverModel *model,
    const char *section_id)
{
    size_t index;
    int removed_active;

    if (model == NULL || section_id == NULL || section_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_section(model, section_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    removed_active = strcmp(model->active_section_id, section_id) == 0;
    if (index + 1U < model->count) {
        (void)memmove(&model->sections[index], &model->sections[index + 1U],
                      (model->count - index - 1U) * sizeof(*model->sections));
    }
    --model->count;
    if (removed_active) {
        if (model->count > 0U) {
            (void)memcpy(model->active_section_id, model->sections[0].id,
                         strlen(model->sections[0].id) + 1U);
        } else {
            model->active_section_id[0] = '\0';
            model->pinned = 0;
        }
    }
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_hover_model_sort(UmiEditorHoverModel *model)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->count > 1U) {
        qsort(model->sections, model->count, sizeof(*model->sections),
              compare_sections);
    }
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_hover_model_set_active(
    UmiEditorHoverModel *model,
    const char *section_id)
{
    if (model == NULL || section_id == NULL || section_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_section(model, section_id) == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    (void)memcpy(model->active_section_id, section_id, strlen(section_id) + 1U);
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_hover_model_set_pinned(
    UmiEditorHoverModel *model,
    int pinned)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (pinned != 0 && model->active_section_id[0] == '\0') {
        return UMI_STATUS_INVALID_STATE;
    }
    model->pinned = pinned != 0;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_hover_model_at(
    const UmiEditorHoverModel *model,
    size_t index,
    UmiEditorHoverSection *out_section)
{
    if (model == NULL || out_section == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= model->count) return UMI_STATUS_NOT_FOUND;
    *out_section = model->sections[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_hover_model_visible_at(
    const UmiEditorHoverModel *model,
    size_t visible_index,
    UmiEditorHoverSection *out_section)
{
    size_t index;
    size_t position = 0U;

    if (model == NULL || out_section == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < model->count; ++index) {
        if (!model->sections[index].visible) continue;
        if (position++ == visible_index) {
            *out_section = model->sections[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_editor_hover_model_active(
    const UmiEditorHoverModel *model,
    UmiEditorHoverSection *out_section)
{
    size_t index;

    if (model == NULL || out_section == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_section(model, model->active_section_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_section = model->sections[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_hover_model_snapshot(
    const UmiEditorHoverModel *model,
    UmiEditorHoverSnapshot *out_snapshot)
{
    size_t index;

    if (model == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_HOVER_MODEL_API_VERSION;
    out_snapshot->section_count = model->count;
    out_snapshot->revision = model->revision;
    out_snapshot->pinned = model->pinned;
    (void)memcpy(out_snapshot->active_section_id, model->active_section_id,
                 strlen(model->active_section_id) + 1U);
    for (index = 0U; index < model->count; ++index) {
        if (model->sections[index].visible) {
            ++out_snapshot->visible_section_count;
        }
        if (model->sections[index].resolution_state == UMI_EDITOR_HOVER_READY) {
            ++out_snapshot->ready_section_count;
        } else if (model->sections[index].resolution_state ==
                   UMI_EDITOR_HOVER_PENDING) {
            ++out_snapshot->pending_section_count;
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_hover_model_count(const UmiEditorHoverModel *model)
{
    return model != NULL ? model->count : 0U;
}

uint64_t umi_editor_hover_model_revision(const UmiEditorHoverModel *model)
{
    return model != NULL ? model->revision : 0U;
}
