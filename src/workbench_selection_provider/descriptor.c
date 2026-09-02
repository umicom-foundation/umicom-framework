/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/descriptor.c
 *
 * PURPOSE:
 *   Implement provider descriptor construction, routing identity and validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/descriptor.h"

#include <string.h>

/*
 * Initialise workbench selection provider descriptor from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_provider_descriptor_init(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *provider_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL) return;
    memset(descriptor, 0, sizeof(*descriptor));
    descriptor->structure_size = (uint32_t)sizeof(*descriptor);
    descriptor->kind = UMI_WORKBENCH_SELECTION_PROVIDER_GENERIC;
    descriptor->state = UMI_WORKBENCH_SELECTION_PROVIDER_CREATED;
    descriptor->selection_kind = UMI_WORKBENCH_SELECTION_GENERIC;
    descriptor->context_kind = UMI_CONTEXT_KIND_SELECTION;
    descriptor->enabled = true;
    descriptor->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider_id != NULL) {
        (void)umi_workbench_selection_provider_copy_text(
            descriptor->provider_id,
            sizeof(descriptor->provider_id),
            provider_id);
    }
}

/*
 * Provide the workbench selection provider descriptor set identity operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_descriptor_set_identity(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *application_id,
    const char *panel_id,
    const char *display_name)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL || application_id == NULL ||
        panel_id == NULL || display_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_provider_copy_text(
        descriptor->application_id,
        sizeof(descriptor->application_id),
        application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_provider_copy_text(
        descriptor->panel_id,
        sizeof(descriptor->panel_id),
        panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_provider_copy_text(
        descriptor->display_name,
        sizeof(descriptor->display_name),
        display_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++descriptor->revision;
    return status;
}

/*
 * Provide the workbench selection provider descriptor set routing operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_descriptor_set_routing(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *source_id,
    const char *group_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL || source_id == NULL || group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_provider_copy_text(
        descriptor->default_source_id,
        sizeof(descriptor->default_source_id),
        source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_provider_copy_text(
        descriptor->default_group_id,
        sizeof(descriptor->default_group_id),
        group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++descriptor->revision;
    return status;
}

/*
 * Check that workbench selection provider descriptor satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_provider_descriptor_validate(
    const UmiWorkbenchSelectionProviderDescriptor *descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL ||
        descriptor->structure_size != sizeof(*descriptor) ||
        descriptor->provider_id[0] == '\0' ||
        descriptor->application_id[0] == '\0' ||
        descriptor->panel_id[0] == '\0' ||
        descriptor->kind < UMI_WORKBENCH_SELECTION_PROVIDER_GENERIC ||
        descriptor->kind > UMI_WORKBENCH_SELECTION_PROVIDER_MEDIA ||
        descriptor->state < UMI_WORKBENCH_SELECTION_PROVIDER_CREATED ||
        descriptor->state > UMI_WORKBENCH_SELECTION_PROVIDER_STOPPED ||
        descriptor->selection_kind < UMI_WORKBENCH_SELECTION_GENERIC ||
        descriptor->selection_kind > UMI_WORKBENCH_SELECTION_MEDIA ||
        descriptor->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        descriptor->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
