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

void umi_workbench_selection_provider_descriptor_init(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *provider_id)
{
    if (descriptor == NULL) return;
    memset(descriptor, 0, sizeof(*descriptor));
    descriptor->structure_size = (uint32_t)sizeof(*descriptor);
    descriptor->kind = UMI_WORKBENCH_SELECTION_PROVIDER_GENERIC;
    descriptor->state = UMI_WORKBENCH_SELECTION_PROVIDER_CREATED;
    descriptor->selection_kind = UMI_WORKBENCH_SELECTION_GENERIC;
    descriptor->context_kind = UMI_CONTEXT_KIND_SELECTION;
    descriptor->enabled = true;
    descriptor->revision = 1U;
    if (provider_id != NULL) {
        (void)umi_workbench_selection_provider_copy_text(
            descriptor->provider_id,
            sizeof(descriptor->provider_id),
            provider_id);
    }
}

UmiStatus umi_workbench_selection_provider_descriptor_set_identity(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *application_id,
    const char *panel_id,
    const char *display_name)
{
    UmiStatus status;
    if (descriptor == NULL || application_id == NULL ||
        panel_id == NULL || display_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_provider_copy_text(
        descriptor->application_id,
        sizeof(descriptor->application_id),
        application_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_provider_copy_text(
        descriptor->panel_id,
        sizeof(descriptor->panel_id),
        panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_provider_copy_text(
        descriptor->display_name,
        sizeof(descriptor->display_name),
        display_name);
    if (status == UMI_STATUS_OK) ++descriptor->revision;
    return status;
}

UmiStatus umi_workbench_selection_provider_descriptor_set_routing(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *source_id,
    const char *group_id)
{
    UmiStatus status;
    if (descriptor == NULL || source_id == NULL || group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_provider_copy_text(
        descriptor->default_source_id,
        sizeof(descriptor->default_source_id),
        source_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_provider_copy_text(
        descriptor->default_group_id,
        sizeof(descriptor->default_group_id),
        group_id);
    if (status == UMI_STATUS_OK) ++descriptor->revision;
    return status;
}

UmiStatus umi_workbench_selection_provider_descriptor_validate(
    const UmiWorkbenchSelectionProviderDescriptor *descriptor)
{
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
