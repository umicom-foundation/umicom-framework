/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/formatting_profile.h
 *
 * PURPOSE:
 *   Define reusable language-formatting policy profiles shared by Studio,
 *   future applications, native formatters and language-server adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_FORMATTING_PROFILE_H
#define UMICOM_EDITOR_FORMATTING_PROFILE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_FORMATTING_PROFILE_API_VERSION 1U
#define UMI_EDITOR_FORMATTING_PROFILE_ID_CAPACITY 128U
#define UMI_EDITOR_FORMATTING_LANGUAGE_CAPACITY 64U
#define UMI_EDITOR_FORMATTING_PROVIDER_CAPACITY 128U
#define UMI_EDITOR_FORMATTING_LABEL_CAPACITY 256U

/**
 * List the named editor formatting brace style values accepted by this public contract.
 */
typedef enum UmiEditorFormattingBraceStyle {
    UMI_EDITOR_FORMATTING_BRACES_PRESERVE = 0,
    UMI_EDITOR_FORMATTING_BRACES_K_AND_R = 1,
    UMI_EDITOR_FORMATTING_BRACES_ALLMAN = 2,
    UMI_EDITOR_FORMATTING_BRACES_GNU = 3
} UmiEditorFormattingBraceStyle;

/**
 * List the named editor formatting line ending values accepted by this public contract.
 */
typedef enum UmiEditorFormattingLineEnding {
    UMI_EDITOR_FORMATTING_LINE_ENDING_PRESERVE = 0,
    UMI_EDITOR_FORMATTING_LINE_ENDING_LF = 1,
    UMI_EDITOR_FORMATTING_LINE_ENDING_CRLF = 2
} UmiEditorFormattingLineEnding;

/**
 * Represent the editor formatting profile item data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorFormattingProfileItem {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_FORMATTING_PROFILE_ID_CAPACITY];
    char language_id[UMI_EDITOR_FORMATTING_LANGUAGE_CAPACITY];
    char provider_id[UMI_EDITOR_FORMATTING_PROVIDER_CAPACITY];
    char label[UMI_EDITOR_FORMATTING_LABEL_CAPACITY];
    uint32_t tab_size;
    uint32_t max_line_length;
    UmiEditorFormattingBraceStyle brace_style;
    UmiEditorFormattingLineEnding line_ending;
    int32_t priority;
    int insert_spaces;
    int trim_trailing_whitespace;
    int insert_final_newline;
    int format_on_save;
    int format_on_type;
    int format_on_paste;
    int organise_imports;
    int enabled;
} UmiEditorFormattingProfileItem;

/**
 * Represent the editor formatting profile snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorFormattingProfileSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t profile_count;
    size_t enabled_profile_count;
    size_t language_count;
    uint64_t revision;
} UmiEditorFormattingProfileSnapshot;

/**
 * Represent the editor formatting profile registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorFormattingProfileRegistry
    UmiEditorFormattingProfileRegistry;

/**
 * Initialise editor formatting profile registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_formatting_profile_registry_create(
    UmiEditorFormattingProfileRegistry **out_registry);
/**
 * Release or reset state held by editor formatting profile registry so the same storage
 * can be reused safely.
 */
void umi_editor_formatting_profile_registry_destroy(
    UmiEditorFormattingProfileRegistry *registry);
/**
 * Release or reset state held by editor formatting profile registry so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_formatting_profile_registry_clear(
    UmiEditorFormattingProfileRegistry *registry);
/**
 * Provide the editor formatting profile registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_formatting_profile_registry_upsert(
    UmiEditorFormattingProfileRegistry *registry,
    const UmiEditorFormattingProfileItem *profile);
/**
 * Remove editor formatting profile registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_editor_formatting_profile_registry_remove(
    UmiEditorFormattingProfileRegistry *registry,
    const char *profile_id);
/**
 * Find editor formatting profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_formatting_profile_registry_find(
    const UmiEditorFormattingProfileRegistry *registry,
    const char *profile_id,
    UmiEditorFormattingProfileItem *out_profile);
/**
 * Provide the editor formatting profile registry resolve operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_formatting_profile_registry_resolve(
    const UmiEditorFormattingProfileRegistry *registry,
    const char *language_id,
    UmiEditorFormattingProfileItem *out_profile);
/**
 * Find editor formatting profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_formatting_profile_registry_at(
    const UmiEditorFormattingProfileRegistry *registry,
    size_t index,
    UmiEditorFormattingProfileItem *out_profile);
/**
 * Provide the editor formatting profile registry snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_formatting_profile_registry_snapshot(
    const UmiEditorFormattingProfileRegistry *registry,
    UmiEditorFormattingProfileSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor formatting profile registry without
 * changing their state.
 */
size_t umi_editor_formatting_profile_registry_count(
    const UmiEditorFormattingProfileRegistry *registry);
/**
 * Provide the editor formatting profile registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_editor_formatting_profile_registry_revision(
    const UmiEditorFormattingProfileRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FORMATTING_PROFILE_H */
