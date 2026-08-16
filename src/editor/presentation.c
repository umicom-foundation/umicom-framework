/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/presentation.c
 *
 * PURPOSE:
 *   Implement validated editor presentation profiles and language resolution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/presentation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorPresentationRegistry {
    UmiEditorPresentationSnapshot items[UMI_EDITOR_PRESENTATION_CAPACITY];
    size_t count;
    uint64_t revision;
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    if (destination == NULL || capacity == 0U) return;
    (void)snprintf(destination, capacity, "%s", source != NULL ? source : "");
}

static int valid_identity(const char *value, size_t capacity, int allow_empty)
{
    const unsigned char *cursor = (const unsigned char *)value;
    size_t length;
    if (value == NULL) return 0;
    length = strlen(value);
    if ((!allow_empty && length == 0U) || length >= capacity) return 0;
    while (*cursor != '\0') {
        if ((*cursor < (unsigned char)'a' || *cursor > (unsigned char)'z') &&
            (*cursor < (unsigned char)'A' || *cursor > (unsigned char)'Z') &&
            (*cursor < (unsigned char)'0' || *cursor > (unsigned char)'9') &&
            *cursor != (unsigned char)'.' && *cursor != (unsigned char)'_' &&
            *cursor != (unsigned char)'-' && *cursor != (unsigned char)'+' &&
            *cursor != (unsigned char)'#') {
            return 0;
        }
        ++cursor;
    }
    return 1;
}

static int boolean_value(int value)
{
    return value == 0 || value == 1;
}

UmiStatus umi_editor_presentation_init(
    UmiEditorPresentationSnapshot *presentation,
    const char *profile_id,
    const char *language_id)
{
    if (presentation == NULL ||
        !valid_identity(profile_id, UMI_EDITOR_PRESENTATION_ID_CAPACITY, 0) ||
        !valid_identity(language_id, UMI_EDITOR_PRESENTATION_LANGUAGE_CAPACITY,
                        1)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(presentation, 0, sizeof(*presentation));
    presentation->struct_size = (uint32_t)sizeof(*presentation);
    presentation->api_version = UMI_EDITOR_PRESENTATION_API_VERSION;
    copy_text(presentation->profile_id, sizeof(presentation->profile_id),
              profile_id);
    copy_text(presentation->language_id, sizeof(presentation->language_id),
              language_id);
    presentation->tab_width = 4U;
    presentation->indent_width = 4U;
    presentation->right_margin_column = 100U;
    presentation->overscroll_lines = 4U;
    presentation->insert_spaces = 1;
    presentation->show_line_numbers = 1;
    presentation->highlight_current_line = 1;
    presentation->highlight_matching_brackets = 1;
    presentation->auto_indent = 1;
    presentation->indent_on_tab = 1;
    presentation->word_wrap = 0;
    presentation->show_right_margin = 0;
    presentation->code_folding = 1;
    presentation->show_fold_gutter = 1;
    presentation->show_minimap = 0;
    presentation->smart_home_end = 1;
    copy_text(presentation->font_family, sizeof(presentation->font_family),
              "Cascadia Code, JetBrains Mono, Monospace");
    presentation->font_size = 11.0;
    presentation->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_presentation_validate(
    const UmiEditorPresentationSnapshot *presentation,
    char *out_reason,
    size_t capacity)
{
    const char *reason = "Editor presentation profile is valid";
    UmiStatus status = UMI_STATUS_OK;
    int written;
    if (presentation == NULL || out_reason == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (presentation->struct_size != sizeof(*presentation) ||
        presentation->api_version != UMI_EDITOR_PRESENTATION_API_VERSION) {
        reason = "Editor presentation structure or API version is invalid";
        status = UMI_STATUS_INVALID_STATE;
    } else if (!valid_identity(presentation->profile_id,
                               sizeof(presentation->profile_id), 0) ||
               !valid_identity(presentation->language_id,
                               sizeof(presentation->language_id), 1)) {
        reason = "Editor presentation identity is invalid";
        status = UMI_STATUS_INVALID_STATE;
    } else if (presentation->tab_width < 1U ||
               presentation->tab_width > 16U ||
               presentation->indent_width < 1U ||
               presentation->indent_width > 16U ||
               presentation->right_margin_column < 40U ||
               presentation->right_margin_column > 240U ||
               presentation->overscroll_lines > 100U) {
        reason = "Editor dimensions are outside supported limits";
        status = UMI_STATUS_INVALID_STATE;
    } else if (!boolean_value(presentation->insert_spaces) ||
               !boolean_value(presentation->show_line_numbers) ||
               !boolean_value(presentation->highlight_current_line) ||
               !boolean_value(presentation->highlight_matching_brackets) ||
               !boolean_value(presentation->auto_indent) ||
               !boolean_value(presentation->indent_on_tab) ||
               !boolean_value(presentation->word_wrap) ||
               !boolean_value(presentation->show_right_margin) ||
               !boolean_value(presentation->show_spaces) ||
               !boolean_value(presentation->show_tabs) ||
               !boolean_value(presentation->show_newlines) ||
               !boolean_value(presentation->show_trailing_whitespace) ||
               !boolean_value(presentation->code_folding) ||
               !boolean_value(presentation->show_fold_gutter) ||
               !boolean_value(presentation->show_minimap) ||
               !boolean_value(presentation->smart_home_end)) {
        reason = "Editor switches must be zero or one";
        status = UMI_STATUS_INVALID_STATE;
    } else if (presentation->font_family[0] == '\0' ||
               strlen(presentation->font_family) >=
                   sizeof(presentation->font_family) ||
               presentation->font_size < 8.0 ||
               presentation->font_size > 48.0) {
        reason = "Editor font settings are invalid";
        status = UMI_STATUS_INVALID_STATE;
    }
    written = snprintf(out_reason, capacity, "%s", reason);
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return status;
}

UmiStatus umi_editor_presentation_default(
    const char *language_id,
    UmiEditorPresentationSnapshot *out_presentation)
{
    const char *profile_id = "editor.default";
    uint32_t tab_width = 4U;
    int word_wrap = 0;
    UmiStatus status;
    if (language_id == NULL || out_presentation == NULL ||
        !valid_identity(language_id, UMI_EDITOR_PRESENTATION_LANGUAGE_CAPACITY,
                        1)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(language_id, "c") == 0) {
        profile_id = "editor.c";
    } else if (strcmp(language_id, "cpp") == 0 ||
               strcmp(language_id, "c++") == 0) {
        profile_id = "editor.cpp";
    } else if (strcmp(language_id, "asm") == 0 ||
               strcmp(language_id, "assembly") == 0) {
        profile_id = "editor.assembly";
        tab_width = 8U;
    } else if (strcmp(language_id, "python") == 0 ||
               strcmp(language_id, "python3") == 0) {
        profile_id = "editor.python";
    } else if (strcmp(language_id, "markdown") == 0 ||
               strcmp(language_id, "md") == 0) {
        profile_id = "editor.markdown";
        tab_width = 2U;
        word_wrap = 1;
    } else if (strcmp(language_id, "html") == 0) {
        profile_id = "editor.html";
        tab_width = 2U;
        word_wrap = 1;
    } else if (strcmp(language_id, "css") == 0) {
        profile_id = "editor.css";
        tab_width = 2U;
        word_wrap = 1;
    } else if (strcmp(language_id, "javascript") == 0 ||
               strcmp(language_id, "js") == 0) {
        profile_id = "editor.javascript";
        tab_width = 2U;
    } else if (strcmp(language_id, "json") == 0) {
        profile_id = "editor.json";
        tab_width = 2U;
        word_wrap = 1;
    } else if (strcmp(language_id, "sh") == 0 ||
               strcmp(language_id, "shell") == 0 ||
               strcmp(language_id, "bash") == 0) {
        profile_id = "editor.shell";
    } else if (strcmp(language_id, "cmake") == 0) {
        profile_id = "editor.cmake";
    }
    status = umi_editor_presentation_init(out_presentation, profile_id,
                                          language_id);
    if (status != UMI_STATUS_OK) return status;
    out_presentation->tab_width = tab_width;
    out_presentation->indent_width = tab_width;
    out_presentation->word_wrap = word_wrap;
    return UMI_STATUS_OK;
}

static size_t find_profile(const UmiEditorPresentationRegistry *registry,
                           const char *profile_id)
{
    size_t index;
    if (registry == NULL || profile_id == NULL) return SIZE_MAX;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].profile_id, profile_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

UmiStatus umi_editor_presentation_registry_create(
    UmiEditorPresentationRegistry **out_registry)
{
    UmiEditorPresentationRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiEditorPresentationRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_editor_presentation_registry_destroy(
    UmiEditorPresentationRegistry *registry)
{
    free(registry);
}

UmiStatus umi_editor_presentation_registry_upsert(
    UmiEditorPresentationRegistry *registry,
    const UmiEditorPresentationSnapshot *presentation)
{
    UmiEditorPresentationSnapshot stored;
    char reason[192U];
    size_t index;
    if (registry == NULL || presentation == NULL ||
        umi_editor_presentation_validate(presentation, reason,
                                         sizeof(reason)) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_profile(registry, presentation->profile_id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_EDITOR_PRESENTATION_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->count++;
    }
    stored = *presentation;
    stored.revision = ++registry->revision;
    registry->items[index] = stored;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_presentation_registry_remove(
    UmiEditorPresentationRegistry *registry,
    const char *profile_id)
{
    size_t index;
    if (registry == NULL || profile_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_profile(registry, profile_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        (void)memmove(&registry->items[index], &registry->items[index + 1U],
                      (registry->count - index - 1U) *
                          sizeof(registry->items[0]));
    }
    --registry->count;
    ++registry->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_presentation_registry_find(
    const UmiEditorPresentationRegistry *registry,
    const char *profile_id,
    UmiEditorPresentationSnapshot *out_presentation)
{
    size_t index;
    if (registry == NULL || profile_id == NULL || out_presentation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_profile(registry, profile_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_presentation = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_presentation_registry_at(
    const UmiEditorPresentationRegistry *registry,
    size_t index,
    UmiEditorPresentationSnapshot *out_presentation)
{
    if (registry == NULL || out_presentation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_presentation = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_presentation_registry_resolve(
    const UmiEditorPresentationRegistry *registry,
    const char *language_id,
    UmiEditorPresentationSnapshot *out_presentation)
{
    size_t index;
    size_t default_index = SIZE_MAX;
    size_t language_index = SIZE_MAX;
    if (registry == NULL || language_id == NULL || out_presentation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].language_id, language_id) == 0) {
            language_index = index;
        }
        if (registry->items[index].language_id[0] == '\0') {
            default_index = index;
        }
    }
    /* Later registrations intentionally override seeded defaults without
     * mutating or deleting the built-in profile. */
    if (language_index != SIZE_MAX) {
        *out_presentation = registry->items[language_index];
        return UMI_STATUS_OK;
    }
    if (default_index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_presentation = registry->items[default_index];
    return UMI_STATUS_OK;
}

static UmiStatus seed_profile(UmiEditorPresentationRegistry *registry,
                              const char *profile_id,
                              const char *language_id,
                              uint32_t tab_width,
                              int word_wrap)
{
    UmiEditorPresentationSnapshot presentation;
    UmiStatus status = umi_editor_presentation_init(
        &presentation, profile_id, language_id);
    if (status != UMI_STATUS_OK) return status;
    presentation.tab_width = tab_width;
    presentation.indent_width = tab_width;
    presentation.word_wrap = word_wrap;
    return umi_editor_presentation_registry_upsert(registry, &presentation);
}

UmiStatus umi_editor_presentation_registry_seed_defaults(
    UmiEditorPresentationRegistry *registry)
{
    static const struct {
        const char *profile_id;
        const char *language_id;
        uint32_t tab_width;
        int word_wrap;
    } defaults[] = {
        {"editor.default", "", 4U, 0},
        {"editor.c", "c", 4U, 0},
        {"editor.cpp", "cpp", 4U, 0},
        {"editor.assembly", "asm", 8U, 0},
        {"editor.python", "python", 4U, 0},
        {"editor.markdown", "markdown", 2U, 1},
        {"editor.html", "html", 2U, 1},
        {"editor.css", "css", 2U, 1},
        {"editor.javascript", "javascript", 2U, 0},
        {"editor.json", "json", 2U, 1},
        {"editor.shell", "sh", 4U, 0},
        {"editor.cmake", "cmake", 4U, 0}
    };
    size_t index;
    UmiStatus status;
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < sizeof(defaults) / sizeof(defaults[0]); ++index) {
        status = seed_profile(registry, defaults[index].profile_id,
                              defaults[index].language_id,
                              defaults[index].tab_width,
                              defaults[index].word_wrap);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_presentation_registry_count(
    const UmiEditorPresentationRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_editor_presentation_registry_revision(
    const UmiEditorPresentationRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
