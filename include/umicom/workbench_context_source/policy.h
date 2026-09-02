/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/policy.h
 *
 * PURPOSE:
 *   Define the reusable source policy contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_POLICY_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source policy data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextSourcePolicy {
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
} UmiWorkbenchContextSourcePolicy;

/**
 * Initialise workbench context source policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_policy_init(
    UmiWorkbenchContextSourcePolicy *record,
    const char *record_id);
/**
 * Check that workbench context source policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_source_policy_validate(
    const UmiWorkbenchContextSourcePolicy *record);
/**
 * Provide the workbench context source policy set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_policy_set_source(
    UmiWorkbenchContextSourcePolicy *record,
    const char *source_id);
/**
 * Provide the workbench context source policy set panel operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_policy_set_panel(
    UmiWorkbenchContextSourcePolicy *record,
    const char *panel_id);
/**
 * Provide the workbench context source policy set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_policy_set_subject(
    UmiWorkbenchContextSourcePolicy *record,
    const char *subject_id);
/**
 * Provide the workbench context source policy set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_policy_set_group(
    UmiWorkbenchContextSourcePolicy *record,
    const char *group_id);
/**
 * Provide the workbench context source policy set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_policy_set_label(
    UmiWorkbenchContextSourcePolicy *record,
    const char *label);
/**
 * Provide the workbench context source policy hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_source_policy_hash(
    const UmiWorkbenchContextSourcePolicy *record);
/**
 * Provide the workbench context source policy touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_source_policy_touch(
    UmiWorkbenchContextSourcePolicy *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
