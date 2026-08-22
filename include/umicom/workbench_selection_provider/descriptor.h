/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/descriptor.h
 *
 * PURPOSE:
 *   Describe one reusable structured-selection provider and its source/context capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_DESCRIPTOR_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_DESCRIPTOR_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderDescriptor {
    uint32_t structure_size;
    char provider_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char application_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char display_name[UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY];
    char default_source_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char default_group_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    UmiWorkbenchSelectionProviderKind kind;
    UmiWorkbenchSelectionProviderRuntimeState state;
    UmiWorkbenchSelectionKind selection_kind;
    UmiContextKind context_kind;
    uint64_t capabilities;
    uint64_t revision;
    bool enabled;
} UmiWorkbenchSelectionProviderDescriptor;

void umi_workbench_selection_provider_descriptor_init(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_descriptor_set_identity(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *application_id,
    const char *panel_id,
    const char *display_name);
UmiStatus umi_workbench_selection_provider_descriptor_set_routing(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *source_id,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_descriptor_validate(
    const UmiWorkbenchSelectionProviderDescriptor *descriptor);

#ifdef __cplusplus
}
#endif
#endif
