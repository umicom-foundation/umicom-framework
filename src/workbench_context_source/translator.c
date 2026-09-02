/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/translator.c
 *
 * PURPOSE:
 *   Map live editor, project, diagnostic, source-control, test, AI, application and trading samples into normalised events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/translator.h"

#include "umicom/workbench_context_event/builders.h"

#include <stdio.h>
#include <string.h>

/* Provide the metadata operation used by this module and its client applications. */
static const char *metadata(
    const UmiWorkbenchContextSourceSample *sample,
    const char *name,
    const char *fallback)
{
    const UmiWorkbenchContextEventMetadata *item =
        umi_workbench_context_source_sample_find_metadata(sample, name);
    return item != NULL ? item->value : fallback;
}

/* Provide the copy metadata operation used by this module and its client applications. */
static UmiStatus copy_metadata(
    UmiWorkbenchContextEvent *event,
    const UmiWorkbenchContextSourceSample *sample)
{
    size_t index;
    UmiStatus status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sample->metadata_count; ++index) {
        status = umi_workbench_context_event_add_metadata(
            event,
            sample->metadata[index].name,
            sample->metadata[index].value);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/* Provide the build generic operation used by this module and its client applications. */
static UmiStatus build_generic(
    const UmiWorkbenchContextSourceDefinition *definition,
    const UmiWorkbenchContextSourceSample *sample,
    UmiWorkbenchContextEvent *event)
{
    UmiStatus status;
    umi_workbench_context_event_init(
        event,
        UMI_WORKBENCH_CONTEXT_EVENT_GENERIC_SELECTION,
        sample->sample_id);
    event->context_kind = sample->context_kind;
    event->timestamp_ms = sample->timestamp_ms;
    event->source_kind = UMI_WORKBENCH_CONTEXT_EVENT_SOURCE_MODEL;
    status = umi_workbench_context_event_copy_text(
        event->source_id, sizeof(event->source_id), definition->source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_event_copy_text(
        event->application_id, sizeof(event->application_id),
        definition->application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_event_copy_text(
        event->panel_id, sizeof(event->panel_id), definition->panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (sample->workspace_id[0] != '\0') {
        status = umi_workbench_context_event_copy_text(
            event->workspace_id, sizeof(event->workspace_id),
            sample->workspace_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (sample->subject_id[0] != '\0') {
        status = umi_workbench_context_event_copy_text(
            event->subject_id, sizeof(event->subject_id),
            sample->subject_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (sample->secondary_id[0] != '\0') {
        status = umi_workbench_context_event_copy_text(
            event->secondary_id, sizeof(event->secondary_id),
            sample->secondary_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (sample->path[0] != '\0') {
        status = umi_workbench_context_event_copy_text(
            event->path, sizeof(event->path), sample->path);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    event->line = sample->line;
    event->column = sample->column;
    event->selection_length = sample->selection_length;
    event->signed_value = sample->signed_value;
    event->unsigned_value = sample->unsigned_value;
    event->boolean_value = sample->boolean_value;
    return UMI_STATUS_OK;
}

/* Provide the build specific operation used by this module and its client applications. */
static UmiStatus build_specific(
    const UmiWorkbenchContextSourceDefinition *definition,
    const UmiWorkbenchContextSourceSample *sample,
    UmiWorkbenchContextEvent *event)
{
    const char *workspace = sample->workspace_id[0] != '\0'
        ? sample->workspace_id : "";
    UmiStatus status;

    /* Select the behaviour associated with the requested command or state value. */
    switch (definition->source_kind) {
    case UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR:
        return umi_workbench_context_event_build_editor_location(
            event,
            sample->sample_id,
            definition->source_id,
            definition->application_id,
            definition->panel_id,
            workspace,
            sample->path,
            sample->symbol,
            sample->line,
            sample->column,
            sample->selection_length,
            sample->timestamp_ms);

    case UMI_WORKBENCH_CONTEXT_SOURCE_PROJECT_EXPLORER:
        return umi_workbench_context_event_build_project(
            event,
            sample->sample_id,
            definition->source_id,
            definition->application_id,
            definition->panel_id,
            workspace,
            sample->subject_id,
            sample->path,
            metadata(sample, "repository-id", ""),
            metadata(sample, "language-id", ""),
            sample->timestamp_ms);

    case UMI_WORKBENCH_CONTEXT_SOURCE_PROBLEMS:
        /* Apply this branch only when its contract condition is satisfied. */
        if (sample->path[0] != '\0') {
            return umi_workbench_context_event_build_diagnostic(
                event,
                sample->sample_id,
                definition->source_id,
                definition->application_id,
                definition->panel_id,
                workspace,
                sample->path,
                sample->line,
                sample->column,
                metadata(sample, "severity", "unknown"),
                metadata(sample, "diagnostic-code", ""),
                metadata(sample, "message", sample->subject_id),
                sample->timestamp_ms);
        }
        break;

    case UMI_WORKBENCH_CONTEXT_SOURCE_SOURCE_CONTROL:
        /* Apply this branch only when its contract condition is satisfied. */
        if (sample->path[0] != '\0' &&
            sample->subject_id[0] != '\0') {
            return umi_workbench_context_event_build_source_control(
                event,
                sample->sample_id,
                definition->source_id,
                definition->application_id,
                definition->panel_id,
                workspace,
                sample->subject_id,
                sample->path,
                metadata(sample, "repository-id", ""),
                metadata(sample, "branch", ""),
                metadata(sample, "selected-path", ""),
                metadata(sample, "change-kind", ""),
                sample->timestamp_ms);
        }
        break;

    case UMI_WORKBENCH_CONTEXT_SOURCE_TEST_EXPLORER:
        return umi_workbench_context_event_build_test(
            event,
            sample->sample_id,
            definition->source_id,
            definition->application_id,
            definition->panel_id,
            workspace,
            sample->subject_id,
            metadata(sample, "suite-id", ""),
            metadata(sample, "outcome", ""),
            metadata(sample, "source-uri", sample->path),
            sample->line,
            sample->unsigned_value,
            sample->timestamp_ms);

    case UMI_WORKBENCH_CONTEXT_SOURCE_DEBUGGER:
        status = umi_workbench_context_event_build_editor_location(
            event,
            sample->sample_id,
            definition->source_id,
            definition->application_id,
            definition->panel_id,
            workspace,
            sample->path,
            sample->symbol,
            sample->line,
            sample->column,
            0U,
            sample->timestamp_ms);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            event->kind = UMI_WORKBENCH_CONTEXT_EVENT_DEBUG_LOCATION;
        }
        return status;

    case UMI_WORKBENCH_CONTEXT_SOURCE_AI:
        return umi_workbench_context_event_build_ai(
            event,
            sample->sample_id,
            definition->source_id,
            definition->application_id,
            definition->panel_id,
            workspace,
            sample->subject_id,
            metadata(sample, "conversation-id", ""),
            metadata(sample, "provider-id", ""),
            metadata(sample, "model-id", ""),
            metadata(sample, "evidence-id", ""),
            sample->timestamp_ms);

    case UMI_WORKBENCH_CONTEXT_SOURCE_WATCHLIST:
    case UMI_WORKBENCH_CONTEXT_SOURCE_CHART:
        return umi_workbench_context_event_build_instrument(
            event,
            sample->sample_id,
            definition->source_id,
            definition->application_id,
            definition->panel_id,
            sample->subject_id,
            metadata(sample, "symbol", sample->secondary_id),
            metadata(sample, "venue", ""),
            metadata(sample, "currency", ""),
            metadata(sample, "asset-class", ""),
            metadata(sample, "contract-id", ""),
            sample->timestamp_ms);

    case UMI_WORKBENCH_CONTEXT_SOURCE_ACCOUNT_SELECTOR:
    case UMI_WORKBENCH_CONTEXT_SOURCE_RISK:
        return umi_workbench_context_event_build_account(
            event,
            sample->sample_id,
            definition->source_id,
            definition->application_id,
            definition->panel_id,
            sample->subject_id,
            metadata(sample, "organisation-id", ""),
            metadata(sample, "book-id", ""),
            metadata(sample, "currency", ""),
            metadata(sample, "account-type", ""),
            metadata(sample, "environment", ""),
            sample->timestamp_ms);

    case UMI_WORKBENCH_CONTEXT_SOURCE_TRADE_BLOTTER:
        return umi_workbench_context_event_build_trade(
            event,
            sample->sample_id,
            definition->source_id,
            definition->application_id,
            definition->panel_id,
            sample->subject_id,
            metadata(sample, "source-system", ""),
            metadata(sample, "product-type", ""),
            metadata(sample, "book-id", ""),
            metadata(sample, "counterparty-id", ""),
            sample->unsigned_value,
            sample->timestamp_ms);

    default:
        break;
    }

    return build_generic(definition, sample, event);
}

/*
 * Provide the workbench context source translate operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_source_translate(
    const UmiWorkbenchContextSourceDefinition *definition,
    const UmiWorkbenchContextSourceSample *sample,
    UmiWorkbenchContextEvent *out_event)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || sample == NULL || out_event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_source_definition_validate(definition);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_source_sample_validate(sample);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_workbench_context_source_definition_accepts(
            definition, sample)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    status = build_specific(definition, sample, out_event);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = copy_metadata(out_event, sample);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (definition->preferred_group_id[0] != '\0' &&
        umi_workbench_context_event_find_metadata(
            out_event, "group-id") == NULL) {
        status = umi_workbench_context_event_add_metadata(
            out_event,
            "group-id",
            definition->preferred_group_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    (void)umi_workbench_context_event_refresh_hash(out_event);
    return umi_workbench_context_event_validate(out_event);
}
