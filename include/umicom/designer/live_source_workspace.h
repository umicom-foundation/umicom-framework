/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/live_source_workspace.h
 *
 * PURPOSE:
 *   Coordinate debounced source editing and real-time preview without tying
 *   the workflow to one language, renderer or graphical toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A frontend records each source edit here and asks whether the debounce
 * deadline has passed. A language provider then validates or renders the
 * source and reports success or failure. Keeping this policy in Framework
 * gives desktop, web and future frontends the same safe behaviour.
 */
#ifndef UMICOM_DESIGNER_LIVE_SOURCE_WORKSPACE_H
#define UMICOM_DESIGNER_LIVE_SOURCE_WORKSPACE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/designer/builder_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_LIVE_SOURCE_CAPACITY 65536U
#define UMI_DESIGNER_LIVE_PREVIEW_CAPACITY 65536U
#define UMI_DESIGNER_LIVE_DIAGNOSTIC_CAPACITY 1024U
#define UMI_DESIGNER_LIVE_DEFAULT_DEBOUNCE_MS 350U

/* Identify the editor/preview arrangement selected by the user. */
typedef enum UmiDesignerWorkspaceMode {
    UMI_DESIGNER_WORKSPACE_DESIGN = 1,
    UMI_DESIGNER_WORKSPACE_CODE = 2,
    UMI_DESIGNER_WORKSPACE_MIXED = 3,
    UMI_DESIGNER_WORKSPACE_PREVIEW = 4
} UmiDesignerWorkspaceMode;

/* Identify common source families without prescribing one compiler or runtime. */
typedef enum UmiDesignerSourceLanguage {
    UMI_DESIGNER_SOURCE_DECLARATIVE = 1,
    UMI_DESIGNER_SOURCE_C = 2,
    UMI_DESIGNER_SOURCE_HTML = 3,
    UMI_DESIGNER_SOURCE_JAVASCRIPT = 4,
    UMI_DESIGNER_SOURCE_CSS = 5,
    UMI_DESIGNER_SOURCE_SCRIPT = 6,
    UMI_DESIGNER_SOURCE_OTHER = 7
} UmiDesignerSourceLanguage;

/*
 * Store only bounded text and revision data. Executable source is never run
 * by this record; an approved renderer or sandbox provider owns that step.
 */
typedef struct UmiDesignerLiveSourceWorkspace {
    UmiDesignerWorkspaceMode mode;
    UmiDesignerSourceLanguage language;
    UmiDesignerPreviewHealth health;
    uint32_t debounce_ms;
    uint64_t source_revision;
    uint64_t preview_revision;
    uint64_t last_edit_millis;
    size_t source_length;
    size_t preview_length;
    int pending;
    char source[UMI_DESIGNER_LIVE_SOURCE_CAPACITY];
    char preview[UMI_DESIGNER_LIVE_PREVIEW_CAPACITY];
    char diagnostic[UMI_DESIGNER_LIVE_DIAGNOSTIC_CAPACITY];
} UmiDesignerLiveSourceWorkspace;

/* Initialise a live workspace with a known language and bounded debounce. */
UmiStatus umi_designer_live_source_workspace_init(
    UmiDesignerLiveSourceWorkspace *workspace,
    UmiDesignerSourceLanguage language,
    uint32_t debounce_ms);

/* Change the visible Design, Code, Mixed or Preview arrangement. */
UmiStatus umi_designer_live_source_workspace_set_mode(
    UmiDesignerLiveSourceWorkspace *workspace,
    UmiDesignerWorkspaceMode mode);

/* Replace pending source and restart its debounce deadline. */
UmiStatus umi_designer_live_source_workspace_update(
    UmiDesignerLiveSourceWorkspace *workspace,
    const char *source,
    uint64_t source_revision,
    uint64_t now_millis);

/* Report whether a provider may validate or render the latest edit now. */
int umi_designer_live_source_workspace_is_due(
    const UmiDesignerLiveSourceWorkspace *workspace,
    uint64_t now_millis);

/* Publish validated preview output for the exact pending source revision. */
UmiStatus umi_designer_live_source_workspace_accept(
    UmiDesignerLiveSourceWorkspace *workspace,
    uint64_t source_revision,
    const char *preview);

/* Record a validation problem while retaining the last valid preview. */
UmiStatus umi_designer_live_source_workspace_reject(
    UmiDesignerLiveSourceWorkspace *workspace,
    uint64_t source_revision,
    const char *diagnostic);

/* Infer a common source family from a file path or display name. */
UmiDesignerSourceLanguage umi_designer_source_language_from_path(
    const char *path);

/* Return stable text for user interfaces, logs and generated documentation. */
const char *umi_designer_workspace_mode_text(UmiDesignerWorkspaceMode mode);
const char *umi_designer_source_language_text(
    UmiDesignerSourceLanguage language);

#ifdef __cplusplus
}
#endif
#endif
