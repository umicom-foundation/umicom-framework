/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/live_source_workspace.c
 *
 * PURPOSE:
 *   Implement the language-neutral, revision-safe live preview workflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/live_source_workspace.h"

#include <ctype.h>
#include <string.h>

/* Copy text only when it fits, including its terminating null byte. */
static UmiStatus copy_bounded_text(
    char *destination,
    size_t capacity,
    const char *source,
    size_t *out_length)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    if (out_length != NULL) *out_length = length;
    return UMI_STATUS_OK;
}

/* Check mode values before storing them in persistent workspace state. */
static int mode_is_valid(UmiDesignerWorkspaceMode mode)
{
    return mode >= UMI_DESIGNER_WORKSPACE_DESIGN &&
           mode <= UMI_DESIGNER_WORKSPACE_PREVIEW;
}

/* Check language values so corrupt configuration cannot select an unknown provider. */
static int language_is_valid(UmiDesignerSourceLanguage language)
{
    return language >= UMI_DESIGNER_SOURCE_DECLARATIVE &&
           language <= UMI_DESIGNER_SOURCE_OTHER;
}

/* Initialise a live workspace with empty source and preview buffers. */
UmiStatus umi_designer_live_source_workspace_init(
    UmiDesignerLiveSourceWorkspace *workspace,
    UmiDesignerSourceLanguage language,
    uint32_t debounce_ms)
{
    if (workspace == NULL || !language_is_valid(language)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(workspace, 0, sizeof(*workspace));
    workspace->mode = UMI_DESIGNER_WORKSPACE_DESIGN;
    workspace->language = language;
    workspace->health = UMI_DESIGNER_PREVIEW_IDLE;
    workspace->debounce_ms = debounce_ms != 0U
        ? debounce_ms
        : UMI_DESIGNER_LIVE_DEFAULT_DEBOUNCE_MS;
    return UMI_STATUS_OK;
}

/* Store the selected arrangement after rejecting values outside the public enum. */
UmiStatus umi_designer_live_source_workspace_set_mode(
    UmiDesignerLiveSourceWorkspace *workspace,
    UmiDesignerWorkspaceMode mode)
{
    if (workspace == NULL || !mode_is_valid(mode)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    workspace->mode = mode;
    return UMI_STATUS_OK;
}

/* Queue one source revision and mark any older preview as stale. */
UmiStatus umi_designer_live_source_workspace_update(
    UmiDesignerLiveSourceWorkspace *workspace,
    const char *source,
    uint64_t source_revision,
    uint64_t now_millis)
{
    size_t length = 0U;
    UmiStatus status;

    if (workspace == NULL || source == NULL || source_revision == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Every edit receives a unique increasing revision. Rejecting an equal
     * revision prevents different source text from sharing one identity. */
    if (source_revision <= workspace->source_revision) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = copy_bounded_text(
        workspace->source,
        sizeof(workspace->source),
        source,
        &length);
    if (status != UMI_STATUS_OK) return status;
    workspace->source_length = length;
    workspace->source_revision = source_revision;
    workspace->last_edit_millis = now_millis;
    workspace->pending = 1;
    workspace->health = workspace->preview_revision == 0U
        ? UMI_DESIGNER_PREVIEW_IDLE
        : UMI_DESIGNER_PREVIEW_STALE;
    workspace->diagnostic[0] = '\0';
    return UMI_STATUS_OK;
}

/* Use subtraction so the deadline remains correct when an unsigned clock wraps. */
int umi_designer_live_source_workspace_is_due(
    const UmiDesignerLiveSourceWorkspace *workspace,
    uint64_t now_millis)
{
    if (workspace == NULL || !workspace->pending ||
        now_millis < workspace->last_edit_millis) {
        return 0;
    }
    return now_millis - workspace->last_edit_millis >=
        (uint64_t)workspace->debounce_ms;
}

/* Accept only the current revision so slow renderer results cannot flash stale output. */
UmiStatus umi_designer_live_source_workspace_accept(
    UmiDesignerLiveSourceWorkspace *workspace,
    uint64_t source_revision,
    const char *preview)
{
    size_t length = 0U;
    UmiStatus status;

    if (workspace == NULL || preview == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!workspace->pending || source_revision != workspace->source_revision) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = copy_bounded_text(
        workspace->preview,
        sizeof(workspace->preview),
        preview,
        &length);
    if (status != UMI_STATUS_OK) return status;
    workspace->preview_length = length;
    workspace->preview_revision = source_revision;
    workspace->pending = 0;
    workspace->health = UMI_DESIGNER_PREVIEW_CURRENT;
    workspace->diagnostic[0] = '\0';
    return UMI_STATUS_OK;
}

/* Retain the last successful preview while explaining why the new source is invalid. */
UmiStatus umi_designer_live_source_workspace_reject(
    UmiDesignerLiveSourceWorkspace *workspace,
    uint64_t source_revision,
    const char *diagnostic)
{
    UmiStatus status;

    if (workspace == NULL || diagnostic == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!workspace->pending || source_revision != workspace->source_revision) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = copy_bounded_text(
        workspace->diagnostic,
        sizeof(workspace->diagnostic),
        diagnostic,
        NULL);
    if (status != UMI_STATUS_OK) return status;
    workspace->pending = 0;
    workspace->health = UMI_DESIGNER_PREVIEW_INVALID;
    return UMI_STATUS_OK;
}

/* Compare an extension without depending on platform-specific case rules. */
static int extension_equals(const char *extension, const char *expected)
{
    if (extension == NULL || expected == NULL) return 0;
    while (*extension != '\0' && *expected != '\0') {
        if (tolower((unsigned char)*extension) !=
            tolower((unsigned char)*expected)) {
            return 0;
        }
        ++extension;
        ++expected;
    }
    return *extension == '\0' && *expected == '\0';
}

/* Map familiar extensions to provider-neutral source families. */
UmiDesignerSourceLanguage umi_designer_source_language_from_path(
    const char *path)
{
    const char *extension;

    if (path == NULL) return UMI_DESIGNER_SOURCE_OTHER;
    extension = strrchr(path, '.');
    if (extension == NULL) return UMI_DESIGNER_SOURCE_OTHER;
    if (extension_equals(extension, ".umiapp") ||
        extension_equals(extension, ".ui")) {
        return UMI_DESIGNER_SOURCE_DECLARATIVE;
    }
    if (extension_equals(extension, ".c") ||
        extension_equals(extension, ".h")) {
        return UMI_DESIGNER_SOURCE_C;
    }
    if (extension_equals(extension, ".html") ||
        extension_equals(extension, ".htm")) {
        return UMI_DESIGNER_SOURCE_HTML;
    }
    if (extension_equals(extension, ".js") ||
        extension_equals(extension, ".mjs")) {
        return UMI_DESIGNER_SOURCE_JAVASCRIPT;
    }
    if (extension_equals(extension, ".css")) {
        return UMI_DESIGNER_SOURCE_CSS;
    }
    if (extension_equals(extension, ".py") ||
        extension_equals(extension, ".sh") ||
        extension_equals(extension, ".ps1")) {
        return UMI_DESIGNER_SOURCE_SCRIPT;
    }
    return UMI_DESIGNER_SOURCE_OTHER;
}

/* Return concise labels shared by menus, tabs and persistence. */
const char *umi_designer_workspace_mode_text(UmiDesignerWorkspaceMode mode)
{
    switch (mode) {
        case UMI_DESIGNER_WORKSPACE_DESIGN: return "design";
        case UMI_DESIGNER_WORKSPACE_CODE: return "code";
        case UMI_DESIGNER_WORKSPACE_MIXED: return "mixed";
        case UMI_DESIGNER_WORKSPACE_PREVIEW: return "preview";
        default: return "unknown";
    }
}

/* Return concise language labels without naming a concrete toolchain. */
const char *umi_designer_source_language_text(
    UmiDesignerSourceLanguage language)
{
    switch (language) {
        case UMI_DESIGNER_SOURCE_DECLARATIVE: return "declarative-ui";
        case UMI_DESIGNER_SOURCE_C: return "c";
        case UMI_DESIGNER_SOURCE_HTML: return "html";
        case UMI_DESIGNER_SOURCE_JAVASCRIPT: return "javascript";
        case UMI_DESIGNER_SOURCE_CSS: return "css";
        case UMI_DESIGNER_SOURCE_SCRIPT: return "script";
        case UMI_DESIGNER_SOURCE_OTHER: return "other";
        default: return "unknown";
    }
}
