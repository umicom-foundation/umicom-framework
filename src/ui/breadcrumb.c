/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/breadcrumb.c
 *
 * PURPOSE:
 *   Implement breadcrumb path parsing and snapshot publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Both Windows and POSIX separators are accepted so the same model works in Umicom Studio on Windows and Linux.
 */

#include "umicom/ui/breadcrumb.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"
#include "ui_internal.h"

struct UmiUiBreadcrumbModel {
    UmiUiBreadcrumbSegment segments[UMI_UI_BREADCRUMB_MAX_SEGMENTS];
    size_t count;
    uint64_t revision;
    UmiMutex *mutex;
};

/*
 * Initialise ui breadcrumb model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_breadcrumb_model_create(UmiUiBreadcrumbModel **out_model)
{
    UmiUiBreadcrumbModel *model;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiUiBreadcrumbModel *)calloc(1U, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&model->mutex);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(model);
        return status;
    }
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ui breadcrumb model so the same storage can be reused
 * safely.
 */
void umi_ui_breadcrumb_model_destroy(UmiUiBreadcrumbModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    umi_mutex_destroy(model->mutex);
    free(model);
}

/*
 * Provide the ui breadcrumb set segments operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_breadcrumb_set_segments(
    UmiUiBreadcrumbModel *model,
    const UmiUiBreadcrumbSegment *segments,
    size_t count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || (segments == NULL && count != 0U) ||
        count > UMI_UI_BREADCRUMB_MAX_SEGMENTS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    (void)memset(model->segments, 0, sizeof(model->segments));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > 0U) {
        (void)memcpy(model->segments, segments,
                     count * sizeof(model->segments[0]));
        model->segments[count - 1U].current = 1;
    }
    model->count = count;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui breadcrumb set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_breadcrumb_set_path(UmiUiBreadcrumbModel *model,
                                     const char *path)
{
    UmiUiBreadcrumbSegment segments[UMI_UI_BREADCRUMB_MAX_SEGMENTS] = {0};
    char accumulated[UMI_UI_PATH_CAPACITY] = "";
    const char *cursor;
    size_t count = 0U;
    size_t accumulated_length = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    cursor = path;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0' && count < UMI_UI_BREADCRUMB_MAX_SEGMENTS) {
        const char *start;
        const char *end;
        size_t length;

        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*cursor == '/' || *cursor == '\\') ++cursor;
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '\0') break;
        start = cursor;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*cursor != '\0' && *cursor != '/' && *cursor != '\\') ++cursor;
        end = cursor;
        length = (size_t)(end - start);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length == 0U) continue;

        /* Apply this branch only when its contract condition is satisfied. */
        if (accumulated_length != 0U &&
            accumulated_length + 1U < sizeof(accumulated)) {
            accumulated[accumulated_length++] = '/';
            accumulated[accumulated_length] = '\0';
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (accumulated_length + length >= sizeof(accumulated)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)memcpy(accumulated + accumulated_length, start, length);
        accumulated_length += length;
        accumulated[accumulated_length] = '\0';

        (void)snprintf(segments[count].segment_id,
                       sizeof(segments[count].segment_id),
                       "segment.%zu", count);
        (void)snprintf(segments[count].label,
                       sizeof(segments[count].label),
                       "%.*s", (int)length, start);
        (void)umi_ui_copy_text(segments[count].path,
                               sizeof(segments[count].path),
                               accumulated);
        ++count;
    }
    return umi_ui_breadcrumb_set_segments(model, segments, count);
}

/*
 * Provide the ui breadcrumb snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_breadcrumb_snapshot(const UmiUiBreadcrumbModel *model,
                                     UmiUiBreadcrumbSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count > 0U) {
        (void)memcpy(out_snapshot->segments, model->segments,
                     model->count * sizeof(model->segments[0]));
    }
    out_snapshot->count = model->count;
    out_snapshot->revision = model->revision;
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}
