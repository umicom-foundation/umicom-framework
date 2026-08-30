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

static UmiStatus field_to_metadata(
    UmiWorkbenchContextSourceSample *sample,
    const UmiWorkbenchSelectionField *field)
{
    char text[96U];
    int written;
    if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_TEXT) {
        return umi_workbench_context_source_sample_add_metadata(
            sample, field->name, field->text);
    }
    if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_INTEGER) {
        written = snprintf(text, sizeof(text), "%lld",
            (long long)field->integer_value);
    } else if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_UNSIGNED) {
        written = snprintf(text, sizeof(text), "%llu",
            (unsigned long long)field->unsigned_value);
    } else if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_DECIMAL) {
        written = snprintf(text, sizeof(text), "%.12g",
            field->decimal_value);
    } else if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_BOOLEAN) {
        written = snprintf(text, sizeof(text), "%s",
            field->boolean_value ? "true" : "false");
    } else {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_workbench_context_source_sample_add_metadata(
        sample, field->name, text);
}

UmiStatus umi_workbench_selection_resolve_source_sample(
    const UmiWorkbenchSelection *selection,
    const char *source_id,
    UmiWorkbenchContextSourceKind source_kind,
    UmiWorkbenchContextSourceTrigger trigger,
    UmiWorkbenchContextSourceSample *out_sample)
{
    size_t index;
    UmiStatus status;
    if (selection == NULL || source_id == NULL || out_sample == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_validate(selection);
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
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_source_sample_set_subject(
        out_sample,
        selection->subject_id,
        selection->secondary_id);
    if (status != UMI_STATUS_OK) return status;

    if (selection->path[0] != '\0') {
        status = umi_workbench_context_source_sample_set_location(
            out_sample,
            selection->path,
            "",
            selection->line,
            selection->column,
            selection->selection_length);
        if (status != UMI_STATUS_OK) return status;
    }
    out_sample->signed_value = selection->signed_value;
    out_sample->unsigned_value = selection->unsigned_value;
    out_sample->boolean_value = selection->boolean_value;
    out_sample->timestamp_ms = selection->timestamp_ms;

    for (index = 0U; index < selection->field_count; ++index) {
        status = field_to_metadata(
            out_sample, &selection->fields[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    if (selection->group_hint[0] != '\0') {
        status = umi_workbench_context_source_sample_add_metadata(
            out_sample, "group-id", selection->group_hint);
        if (status != UMI_STATUS_OK) return status;
    }
    (void)umi_workbench_context_source_sample_refresh_hash(out_sample);
    return umi_workbench_context_source_sample_validate(out_sample);
}
