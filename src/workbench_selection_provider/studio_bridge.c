/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/studio_bridge.c
 *
 * PURPOSE:
 *   Resolve authoritative model snapshots into structured selections and submit them without display-string parsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/studio_bridge.h"

static const UmiWorkbenchSelectionProviderDescriptor *provider(
    const UmiWorkbenchSelectionProviderService *service,
    const char *provider_id)
{
    return service != NULL
        ? umi_workbench_selection_provider_registry_find(
            &service->registry, provider_id)
        : NULL;
}

UmiStatus umi_workbench_selection_provider_publish_project(
    UmiWorkbenchSelectionProviderService *service,
    const UmiProjectWorkspaceSelectionSnapshot *snapshot,
    const char *workspace_id,
    uint64_t timestamp_ms)
{
    const UmiWorkbenchSelectionProviderDescriptor *descriptor =
        provider(service, "studio.provider.project");
    UmiWorkbenchSelection selection;
    UmiStatus status;
    if (descriptor == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_workbench_selection_provider_from_project_selection(
        snapshot,
        descriptor->application_id,
        descriptor->panel_id,
        workspace_id,
        timestamp_ms,
        &selection);
    if (status != UMI_STATUS_OK) return status;

    return umi_workbench_selection_provider_service_publish(
        service,
        descriptor->provider_id,
        &selection,
        UMI_WORKBENCH_CONTEXT_SOURCE_PROJECT_EXPLORER,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT);
}

UmiStatus umi_workbench_selection_provider_publish_problem(
    UmiWorkbenchSelectionProviderService *service,
    const UmiUiProblemSnapshot *problem_snapshot,
    const char *workspace_id,
    uint64_t timestamp_ms)
{
    const UmiWorkbenchSelectionProviderDescriptor *descriptor =
        provider(service, "studio.provider.problems");
    UmiWorkbenchSelection selection;
    UmiStatus status;
    if (descriptor == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_workbench_selection_provider_from_problem(
        problem_snapshot,
        descriptor->application_id,
        descriptor->panel_id,
        workspace_id,
        timestamp_ms,
        &selection);
    if (status != UMI_STATUS_OK) return status;

    return umi_workbench_selection_provider_service_publish(
        service,
        descriptor->provider_id,
        &selection,
        UMI_WORKBENCH_CONTEXT_SOURCE_PROBLEMS,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT);
}

UmiStatus umi_workbench_selection_provider_publish_source_change(
    UmiWorkbenchSelectionProviderService *service,
    const UmiSourceControlChangeSnapshot *change,
    const char *workspace_id,
    const char *workspace_root,
    const char *branch,
    uint64_t timestamp_ms)
{
    const UmiWorkbenchSelectionProviderDescriptor *descriptor =
        provider(service, "studio.provider.source-control");
    UmiWorkbenchSelection selection;
    UmiStatus status;
    if (descriptor == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_workbench_selection_provider_from_source_control_change(
        change,
        descriptor->application_id,
        descriptor->panel_id,
        workspace_id,
        workspace_root,
        branch,
        timestamp_ms,
        &selection);
    if (status != UMI_STATUS_OK) return status;

    return umi_workbench_selection_provider_service_publish(
        service,
        descriptor->provider_id,
        &selection,
        UMI_WORKBENCH_CONTEXT_SOURCE_SOURCE_CONTROL,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT);
}

UmiStatus umi_workbench_selection_provider_publish_source_commit(
    UmiWorkbenchSelectionProviderService *service,
    const UmiSourceControlCommitSnapshot *commit,
    const char *workspace_id,
    uint64_t timestamp_ms)
{
    const UmiWorkbenchSelectionProviderDescriptor *descriptor =
        provider(service, "studio.provider.source-control");
    UmiWorkbenchSelection selection;
    UmiStatus status;
    if (descriptor == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_workbench_selection_provider_from_source_control_commit(
        commit,
        descriptor->application_id,
        descriptor->panel_id,
        workspace_id,
        timestamp_ms,
        &selection);
    if (status != UMI_STATUS_OK) return status;

    return umi_workbench_selection_provider_service_publish(
        service,
        descriptor->provider_id,
        &selection,
        UMI_WORKBENCH_CONTEXT_SOURCE_SOURCE_CONTROL,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT);
}

UmiStatus umi_workbench_selection_provider_publish_source_branch(
    UmiWorkbenchSelectionProviderService *service,
    const UmiSourceControlBranchSnapshot *branch,
    const char *workspace_id,
    uint64_t timestamp_ms)
{
    const UmiWorkbenchSelectionProviderDescriptor *descriptor =
        provider(service, "studio.provider.source-control");
    UmiWorkbenchSelection selection;
    UmiStatus status;
    if (descriptor == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_workbench_selection_provider_from_source_control_branch(
        branch,
        descriptor->application_id,
        descriptor->panel_id,
        workspace_id,
        timestamp_ms,
        &selection);
    if (status != UMI_STATUS_OK) return status;

    return umi_workbench_selection_provider_service_publish(
        service,
        descriptor->provider_id,
        &selection,
        UMI_WORKBENCH_CONTEXT_SOURCE_SOURCE_CONTROL,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT);
}

UmiStatus umi_workbench_selection_provider_publish_test_row(
    UmiWorkbenchSelectionProviderService *service,
    const UmiTestExplorerRow *row,
    const char *workspace_id,
    uint64_t timestamp_ms)
{
    const UmiWorkbenchSelectionProviderDescriptor *descriptor =
        provider(service, "studio.provider.test-explorer");
    UmiWorkbenchSelection selection;
    UmiStatus status;
    if (descriptor == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_workbench_selection_provider_from_test_row(
        row,
        descriptor->application_id,
        descriptor->panel_id,
        workspace_id,
        timestamp_ms,
        &selection);
    if (status != UMI_STATUS_OK) return status;

    return umi_workbench_selection_provider_service_publish(
        service,
        descriptor->provider_id,
        &selection,
        UMI_WORKBENCH_CONTEXT_SOURCE_TEST_EXPLORER,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT);
}
