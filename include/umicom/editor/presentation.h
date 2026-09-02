/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/presentation.h
 *
 * PURPOSE:
 *   Define reusable, toolkit-neutral editor presentation profiles for source,
 *   markup, scripting and configuration languages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the editor presentation snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the editor presentation registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorPresentationRegistry UmiEditorPresentationRegistry;

/**
 * Initialise editor presentation from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_presentation_init(
    UmiEditorPresentationSnapshot *presentation,
    const char *profile_id,
    const char *language_id);
/**
 * Check that editor presentation satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_editor_presentation_validate(
    const UmiEditorPresentationSnapshot *presentation,
    char *out_reason,
    size_t capacity);
/**
 * Provide the editor presentation default operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_presentation_default(
    const char *language_id,
    UmiEditorPresentationSnapshot *out_presentation);
/**
 * Initialise editor presentation registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_presentation_registry_create(
    UmiEditorPresentationRegistry **out_registry);
/**
 * Release or reset state held by editor presentation registry so the same storage can be
 * reused safely.
 */
void umi_editor_presentation_registry_destroy(
    UmiEditorPresentationRegistry *registry);
/**
 * Provide the editor presentation registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_presentation_registry_upsert(
    UmiEditorPresentationRegistry *registry,
    const UmiEditorPresentationSnapshot *presentation);
/**
 * Remove editor presentation registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_presentation_registry_remove(
    UmiEditorPresentationRegistry *registry,
    const char *profile_id);
/**
 * Find editor presentation registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_presentation_registry_find(
    const UmiEditorPresentationRegistry *registry,
    const char *profile_id,
    UmiEditorPresentationSnapshot *out_presentation);
/**
 * Find editor presentation registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_presentation_registry_at(
    const UmiEditorPresentationRegistry *registry,
    size_t index,
    UmiEditorPresentationSnapshot *out_presentation);
/**
 * Provide the editor presentation registry resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_presentation_registry_resolve(
    const UmiEditorPresentationRegistry *registry,
    const char *language_id,
    UmiEditorPresentationSnapshot *out_presentation);
/**
 * Provide the editor presentation registry seed defaults operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_presentation_registry_seed_defaults(
    UmiEditorPresentationRegistry *registry);
/**
 * Return the number of records represented by editor presentation registry without
 * changing their state.
 */
size_t umi_editor_presentation_registry_count(
    const UmiEditorPresentationRegistry *registry);
/**
 * Provide the editor presentation registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_presentation_registry_revision(
    const UmiEditorPresentationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
