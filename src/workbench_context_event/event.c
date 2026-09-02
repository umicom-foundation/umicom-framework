/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_event/event.c
 *
 * PURPOSE:
 *   Implement interaction-event validation, metadata and stable content hashing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_event/event.h"

#include <string.h>

/*
 * Initialise workbench context event from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_context_event_init(
    UmiWorkbenchContextEvent *event,
    UmiWorkbenchContextEventKind kind,
    const char *event_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event == NULL) return;
    memset(event, 0, sizeof(*event));
    event->structure_size = (uint32_t)sizeof(*event);
    event->kind = kind;
    event->source_kind = UMI_WORKBENCH_CONTEXT_EVENT_SOURCE_MODEL;
    event->state = UMI_WORKBENCH_CONTEXT_EVENT_CREATED;
    event->priority = UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_INTERACTIVE;
    event->context_kind = UMI_CONTEXT_KIND_SELECTION;
    event->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event_id != NULL) {
        (void)umi_workbench_context_event_copy_text(
            event->event_id, sizeof(event->event_id), event_id);
    }
}

/*
 * Check that workbench context event satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_workbench_context_event_validate(
    const UmiWorkbenchContextEvent *event)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event == NULL || event->structure_size != sizeof(*event)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (event->event_id[0] == '\0' ||
        event->application_id[0] == '\0' ||
        event->panel_id[0] == '\0' ||
        event->kind <= UMI_WORKBENCH_CONTEXT_EVENT_NONE ||
        event->kind > UMI_WORKBENCH_CONTEXT_EVENT_GENERIC_SELECTION ||
        event->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        event->context_kind > UMI_CONTEXT_KIND_SELECTION ||
        event->metadata_count > UMI_WORKBENCH_CONTEXT_EVENT_MAX_METADATA) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context event add metadata operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_add_metadata(
    UmiWorkbenchContextEvent *event,
    const char *name,
    const char *value)
{
    UmiWorkbenchContextEventMetadata *item;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event == NULL || name == NULL || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (event->metadata_count >= UMI_WORKBENCH_CONTEXT_EVENT_MAX_METADATA) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    item = &event->metadata[event->metadata_count];
    status = umi_workbench_context_event_copy_text(
        item->name, sizeof(item->name), name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_event_copy_text(
        item->value, sizeof(item->value), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    ++event->metadata_count;
    ++event->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context event find metadata operation used by this module and its
 * client applications.
 */
const UmiWorkbenchContextEventMetadata *
umi_workbench_context_event_find_metadata(
    const UmiWorkbenchContextEvent *event,
    const char *name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event == NULL || name == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < event->metadata_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(event->metadata[index].name, name) == 0) {
            return &event->metadata[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench context event refresh hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_event_refresh_hash(
    UmiWorkbenchContextEvent *event)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event == NULL) return 0U;
    hash = umi_workbench_context_event_hash_text(hash, event->source_id, sizeof(event->source_id));
    hash = umi_workbench_context_event_hash_text(hash, event->application_id, sizeof(event->application_id));
    hash = umi_workbench_context_event_hash_text(hash, event->panel_id, sizeof(event->panel_id));
    hash = umi_workbench_context_event_hash_text(hash, event->subject_id, sizeof(event->subject_id));
    hash = umi_workbench_context_event_hash_text(hash, event->workspace_id, sizeof(event->workspace_id));
    hash = umi_workbench_context_event_hash_text(hash, event->path, sizeof(event->path));
    hash = umi_workbench_context_event_hash_text(hash, event->secondary_id, sizeof(event->secondary_id));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < event->metadata_count; ++index) {
        hash = umi_workbench_context_event_hash_text(
            hash, event->metadata[index].name, sizeof(event->metadata[index].name));
        hash = umi_workbench_context_event_hash_text(
            hash, event->metadata[index].value, sizeof(event->metadata[index].value));
    }
    hash ^= (uint64_t)event->kind;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)event->context_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= event->unsigned_value;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)event->signed_value;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)event->line;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)event->column;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)event->selection_length;
    hash *= UINT64_C(1099511628211);
    hash ^= event->boolean_value ? UINT64_C(1) : UINT64_C(0);
    hash *= UINT64_C(1099511628211);
    event->content_hash = hash;
    return hash;
}
