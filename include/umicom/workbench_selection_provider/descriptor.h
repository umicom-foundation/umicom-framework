/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/descriptor.h
 *
 * PURPOSE:
 *   Describe one reusable structured-selection provider and its source/context capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_DESCRIPTOR_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_DESCRIPTOR_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider descriptor data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise workbench selection provider descriptor from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_provider_descriptor_init(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *provider_id);
/**
 * Provide the workbench selection provider descriptor set identity operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_descriptor_set_identity(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *application_id,
    const char *panel_id,
    const char *display_name);
/**
 * Provide the workbench selection provider descriptor set routing operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_descriptor_set_routing(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *source_id,
    const char *group_id);
/**
 * Check that workbench selection provider descriptor satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_provider_descriptor_validate(
    const UmiWorkbenchSelectionProviderDescriptor *descriptor);

#ifdef __cplusplus
}
#endif
#endif
