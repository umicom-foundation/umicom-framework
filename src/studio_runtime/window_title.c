/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/window_title.c
 *
 * PURPOSE:
 *   Implement Studio window-title projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/window_title.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *out, size_t capacity, const char *value)
{
    size_t length = strlen(value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out, value, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the studio window title build operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_window_title_build(
    const char *application_name,
    const char *workspace_name,
    const UmiStudioRuntimeDocumentState *documents,
    UmiStudioRuntimeWindowTitle *out_title)
{
    int written;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application_name == NULL || application_name[0] == '\0' ||
        workspace_name == NULL || documents == NULL ||
        out_title == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_title, 0, sizeof(*out_title));

    status = copy_text(
        out_title->application_name,
        sizeof(out_title->application_name),
        application_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_title->workspace_name,
            sizeof(out_title->workspace_name),
            workspace_name);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this operation only while the related capability or state is available. */
    if (documents->has_active) {
        status = copy_text(
            out_title->document_name,
            sizeof(out_title->document_name),
            documents->active.display_name);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        out_title->document_dirty = documents->active.dirty;

        written = snprintf(
            out_title->title,
            sizeof(out_title->title),
            "%.240s%s — %.240s — %.120s",
            out_title->document_name,
            out_title->document_dirty ? " *" : "",
            workspace_name,
            application_name);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (workspace_name[0] != '\0') {
        written = snprintf(
            out_title->title,
            sizeof(out_title->title),
            "%.300s — %.120s",
            workspace_name,
            application_name);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        written = snprintf(
            out_title->title,
            sizeof(out_title->title),
            "%.120s",
            application_name);
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(out_title->title)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    out_title->revision = documents->revision + 1U;
    return UMI_STATUS_OK;
}
