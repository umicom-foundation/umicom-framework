/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/profile.h
 *
 * PURPOSE:
 *   Define the reusable source profile contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_PROFILE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_PROFILE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source profile data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextSourceProfile {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceProfile;

/**
 * Initialise workbench context source profile from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_profile_init(
    UmiWorkbenchContextSourceProfile *record,
    const char *record_id);
/**
 * Check that workbench context source profile satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_profile_validate(
    const UmiWorkbenchContextSourceProfile *record);
/**
 * Provide the workbench context source profile set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_profile_set_source(
    UmiWorkbenchContextSourceProfile *record,
    const char *source_id);
/**
 * Provide the workbench context source profile set panel operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_profile_set_panel(
    UmiWorkbenchContextSourceProfile *record,
    const char *panel_id);
/**
 * Provide the workbench context source profile set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_profile_set_subject(
    UmiWorkbenchContextSourceProfile *record,
    const char *subject_id);
/**
 * Provide the workbench context source profile set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_profile_set_group(
    UmiWorkbenchContextSourceProfile *record,
    const char *group_id);
/**
 * Provide the workbench context source profile set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_profile_set_label(
    UmiWorkbenchContextSourceProfile *record,
    const char *label);
/**
 * Provide the workbench context source profile hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_source_profile_hash(
    const UmiWorkbenchContextSourceProfile *record);
/**
 * Provide the workbench context source profile touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_source_profile_touch(
    UmiWorkbenchContextSourceProfile *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
