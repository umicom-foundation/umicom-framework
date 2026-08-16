/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/presentation.h
 *
 * PURPOSE:
 *   Define reusable, toolkit-neutral editor presentation profiles for source,
 *   markup, scripting and configuration languages.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A presentation profile describes how an editor behaves and looks without
 * naming GTK, Qt, Wt or a browser API. Frontend adapters translate the resolved
 * snapshot into their native controls.
 */

#ifndef UMICOM_EDITOR_PRESENTATION_H
#define UMICOM_EDITOR_PRESENTATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_PRESENTATION_API_VERSION 1U
#define UMI_EDITOR_PRESENTATION_CAPACITY 128U
#define UMI_EDITOR_PRESENTATION_ID_CAPACITY 128U
#define UMI_EDITOR_PRESENTATION_LANGUAGE_CAPACITY 64U
#define UMI_EDITOR_PRESENTATION_FONT_CAPACITY 96U

typedef struct UmiEditorPresentationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char profile_id[UMI_EDITOR_PRESENTATION_ID_CAPACITY];
    char language_id[UMI_EDITOR_PRESENTATION_LANGUAGE_CAPACITY];
    uint32_t tab_width;
    uint32_t indent_width;
    uint32_t right_margin_column;
    uint32_t overscroll_lines;
    int insert_spaces;
    int show_line_numbers;
    int highlight_current_line;
    int highlight_matching_brackets;
    int auto_indent;
    int indent_on_tab;
    int word_wrap;
    int show_right_margin;
    int show_spaces;
    int show_tabs;
    int show_newlines;
    int show_trailing_whitespace;
    int code_folding;
    int show_fold_gutter;
    int show_minimap;
    int smart_home_end;
    char font_family[UMI_EDITOR_PRESENTATION_FONT_CAPACITY];
    double font_size;
    uint64_t revision;
} UmiEditorPresentationSnapshot;

typedef struct UmiEditorPresentationRegistry UmiEditorPresentationRegistry;

UmiStatus umi_editor_presentation_init(
    UmiEditorPresentationSnapshot *presentation,
    const char *profile_id,
    const char *language_id);
UmiStatus umi_editor_presentation_validate(
    const UmiEditorPresentationSnapshot *presentation,
    char *out_reason,
    size_t capacity);
UmiStatus umi_editor_presentation_default(
    const char *language_id,
    UmiEditorPresentationSnapshot *out_presentation);
UmiStatus umi_editor_presentation_registry_create(
    UmiEditorPresentationRegistry **out_registry);
void umi_editor_presentation_registry_destroy(
    UmiEditorPresentationRegistry *registry);
UmiStatus umi_editor_presentation_registry_upsert(
    UmiEditorPresentationRegistry *registry,
    const UmiEditorPresentationSnapshot *presentation);
UmiStatus umi_editor_presentation_registry_remove(
    UmiEditorPresentationRegistry *registry,
    const char *profile_id);
UmiStatus umi_editor_presentation_registry_find(
    const UmiEditorPresentationRegistry *registry,
    const char *profile_id,
    UmiEditorPresentationSnapshot *out_presentation);
UmiStatus umi_editor_presentation_registry_at(
    const UmiEditorPresentationRegistry *registry,
    size_t index,
    UmiEditorPresentationSnapshot *out_presentation);
UmiStatus umi_editor_presentation_registry_resolve(
    const UmiEditorPresentationRegistry *registry,
    const char *language_id,
    UmiEditorPresentationSnapshot *out_presentation);
UmiStatus umi_editor_presentation_registry_seed_defaults(
    UmiEditorPresentationRegistry *registry);
size_t umi_editor_presentation_registry_count(
    const UmiEditorPresentationRegistry *registry);
uint64_t umi_editor_presentation_registry_revision(
    const UmiEditorPresentationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
