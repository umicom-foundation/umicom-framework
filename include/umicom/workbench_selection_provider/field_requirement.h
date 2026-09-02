/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/field_requirement.h
 *
 * PURPOSE:
 *   Define the reusable structured field requirement contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_FIELD_REQUIREMENT_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_FIELD_REQUIREMENT_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider field requirement data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderFieldRequirement {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char provider_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY];
    UmiWorkbenchSelectionProviderKind provider_kind;
    UmiWorkbenchSelectionProviderRuntimeState state;
    UmiWorkbenchSelectionKind selection_kind;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionProviderFieldRequirement;

/**
 * Initialise workbench selection provider field requirement from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_field_requirement_init(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *record_id);
/**
 * Check that workbench selection provider field requirement satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_field_requirement_validate(
    const UmiWorkbenchSelectionProviderFieldRequirement *record);
/**
 * Provide the workbench selection provider field requirement set provider operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_field_requirement_set_provider(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider field requirement set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_field_requirement_set_source(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *source_id);
/**
 * Provide the workbench selection provider field requirement set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_field_requirement_set_subject(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider field requirement set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_field_requirement_set_related(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *related_id);
/**
 * Provide the workbench selection provider field requirement set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_field_requirement_set_group(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *group_id);
/**
 * Provide the workbench selection provider field requirement set description operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_field_requirement_set_description(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    const char *description);
/**
 * Provide the workbench selection provider field requirement hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_field_requirement_hash(
    const UmiWorkbenchSelectionProviderFieldRequirement *record);
/**
 * Provide the workbench selection provider field requirement touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_field_requirement_touch(
    UmiWorkbenchSelectionProviderFieldRequirement *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
