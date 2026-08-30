/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection/service.c
 *
 * PURPOSE:
 *   Implement structured selection submission, duplicate suppression and source-pipeline routing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection/service.h"

#include <stdlib.h>
#include <string.h>

UmiStatus umi_workbench_selection_service_create(
    UmiWorkbenchContextSourceService *sources,
    UmiWorkbenchSelectionService **out_service)
{
    UmiWorkbenchSelectionService *service;
    if (sources == NULL || out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    service = (UmiWorkbenchSelectionService *)calloc(
        1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->sources = sources;
    service->sequence = 1U;
    service->revision = 1U;
    service->metrics.revision = 1U;
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_workbench_selection_service_destroy(
    UmiWorkbenchSelectionService *service)
{
    free(service);
}

UmiStatus umi_workbench_selection_service_submit(
    UmiWorkbenchSelectionService *service,
    UmiWorkbenchSelection *selection,
    const char *source_id,
    UmiWorkbenchContextSourceKind source_kind,
    UmiWorkbenchContextSourceTrigger trigger)
{
    UmiWorkbenchContextSourceSample sample;
    UmiStatus status;
    if (service == NULL || selection == NULL || source_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    ++service->metrics.submitted_count;
    if (service->suspended) {
        ++service->metrics.rejected_count;
        ++service->metrics.revision;
        return UMI_STATUS_BUSY;
    }

    selection->sequence = ++service->sequence;
    (void)umi_workbench_selection_refresh_hash(selection);

    if (service->last_hash != 0U &&
        service->last_hash == selection->content_hash) {
        ++service->metrics.duplicate_count;
        ++service->metrics.revision;
        return UMI_STATUS_OK;
    }

    status = umi_workbench_selection_resolve_source_sample(
        selection,
        source_id,
        source_kind,
        trigger,
        &sample);
    if (status != UMI_STATUS_OK) {
        ++service->metrics.rejected_count;
        ++service->metrics.revision;
        return status;
    }
    ++service->metrics.resolved_count;

    status = umi_workbench_context_source_service_submit(
        service->sources, &sample);
    if (status != UMI_STATUS_OK) {
        ++service->metrics.rejected_count;
        ++service->metrics.revision;
        return status;
    }

    service->last_hash = selection->content_hash;
    selection->state = UMI_WORKBENCH_SELECTION_STATE_PUBLISHED;
    ++selection->revision;
    ++service->metrics.published_count;
    ++service->metrics.revision;
    ++service->revision;
    return UMI_STATUS_OK;
}

void umi_workbench_selection_service_set_suspended(
    UmiWorkbenchSelectionService *service,
    bool suspended)
{
    if (service == NULL) return;
    service->suspended = suspended;
    ++service->revision;
}
