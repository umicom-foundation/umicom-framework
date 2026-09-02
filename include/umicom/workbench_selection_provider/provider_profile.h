/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/provider_profile.h
 *
 * PURPOSE:
 *   Define the reusable provider profile contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PROVIDER_PROFILE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PROVIDER_PROFILE_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider provider profile data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderProviderProfile {
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
} UmiWorkbenchSelectionProviderProviderProfile;

/**
 * Initialise workbench selection provider provider profile from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_provider_profile_init(
    UmiWorkbenchSelectionProviderProviderProfile *record,
    const char *record_id);
/**
 * Check that workbench selection provider provider profile satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_provider_profile_validate(
    const UmiWorkbenchSelectionProviderProviderProfile *record);
/**
 * Provide the workbench selection provider provider profile set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_provider_profile_set_provider(
    UmiWorkbenchSelectionProviderProviderProfile *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider provider profile set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_provider_profile_set_source(
    UmiWorkbenchSelectionProviderProviderProfile *record,
    const char *source_id);
/**
 * Provide the workbench selection provider provider profile set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_provider_profile_set_subject(
    UmiWorkbenchSelectionProviderProviderProfile *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider provider profile set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_provider_profile_set_related(
    UmiWorkbenchSelectionProviderProviderProfile *record,
    const char *related_id);
/**
 * Provide the workbench selection provider provider profile set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_provider_profile_set_group(
    UmiWorkbenchSelectionProviderProviderProfile *record,
    const char *group_id);
/**
 * Provide the workbench selection provider provider profile set description operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_provider_profile_set_description(
    UmiWorkbenchSelectionProviderProviderProfile *record,
    const char *description);
/**
 * Provide the workbench selection provider provider profile hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_provider_profile_hash(
    const UmiWorkbenchSelectionProviderProviderProfile *record);
/**
 * Provide the workbench selection provider provider profile touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_provider_profile_touch(
    UmiWorkbenchSelectionProviderProviderProfile *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
