/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection/resolver.c
 *
 * PURPOSE:
 *   Translate structured fields directly into source samples for the existing interaction/event/context pipeline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection/resolver.h"

#include <stdio.h>

/* Provide the field to metadata operation used by this module and its client applications. */
static UmiStatus field_to_metadata(
    UmiWorkbenchContextSourceSample *sample,
    const UmiWorkbenchSelectionField *field)
{
    char text[96U];
    int written;
    /* Apply this branch only when its contract condition is satisfied. */
    if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_TEXT) {
        return umi_workbench_context_source_sample_add_metadata(
            sample, field->name, field->text);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_INTEGER) {
        written = snprintf(text, sizeof(text), "%lld",
            (long long)field->integer_value);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_UNSIGNED) {
        written = snprintf(text, sizeof(text), "%llu",
            (unsigned long long)field->unsigned_value);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_DECIMAL) {
        written = snprintf(text, sizeof(text), "%.12g",
            field->decimal_value);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_BOOLEAN) {
        written = snprintf(text, sizeof(text), "%s",
            field->boolean_value ? "true" : "false");
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_workbench_context_source_sample_add_metadata(
        sample, field->name, text);
}

/*
 * Provide the workbench selection resolve source sample operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_resolve_source_sample(
    const UmiWorkbenchSelection *selection,
    const char *source_id,
    UmiWorkbenchContextSourceKind source_kind,
    UmiWorkbenchContextSourceTrigger trigger,
    UmiWorkbenchContextSourceSample *out_sample)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || source_id == NULL || out_sample == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_validate(selection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_workbench_context_source_sample_init(
        out_sample,
        source_kind,
        trigger,
        selection->context_kind,
        selection->selection_id);
    status = umi_workbench_context_source_sample_set_identity(
        out_sample,
        source_id,
        selection->application_id,
        selection->panel_id,
        selection->workspace_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_source_sample_set_subject(
        out_sample,
        selection->subject_id,
        selection->secondary_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (selection->path[0] != '\0') {
        status = umi_workbench_context_source_sample_set_location(
            out_sample,
            selection->path,
            "",
            selection->line,
            selection->column,
            selection->selection_length);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    out_sample->signed_value = selection->signed_value;
    out_sample->unsigned_value = selection->unsigned_value;
    out_sample->boolean_value = selection->boolean_value;
    out_sample->timestamp_ms = selection->timestamp_ms;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->field_count; ++index) {
        status = field_to_metadata(
            out_sample, &selection->fields[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (selection->group_hint[0] != '\0') {
        status = umi_workbench_context_source_sample_add_metadata(
            out_sample, "group-id", selection->group_hint);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    (void)umi_workbench_context_source_sample_refresh_hash(out_sample);
    return umi_workbench_context_source_sample_validate(out_sample);
}
