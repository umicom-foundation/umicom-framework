/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/application_link_model.h
 *
 * PURPOSE:
 *   Define the reusable application link summary model contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_APPLICATION_LINK_MODEL_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_APPLICATION_LINK_MODEL_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link application link model data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextLinkApplicationLinkModel {
    uint32_t structure_size;
    char model_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    UmiContextKind context_kind;
    UmiContextChannelColour colour;
    UmiWorkbenchContextLinkMode mode;
    UmiWorkbenchContextLinkState state;
    UmiWorkbenchContextLinkOrigin origin;
    UmiWorkbenchContextLinkPriority priority;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextLinkApplicationLinkModel;

/**
 * Initialise workbench context link application link model from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_link_application_link_model_init(UmiWorkbenchContextLinkApplicationLinkModel *record,
                                           const char *identity);
/**
 * Check that workbench context link application link model satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_link_application_link_model_validate(
    const UmiWorkbenchContextLinkApplicationLinkModel *record);
/**
 * Copy workbench context link application link model into module-owned storage so callers
 * keep ownership of their input values.
 */
UmiStatus umi_workbench_context_link_application_link_model_copy(
    UmiWorkbenchContextLinkApplicationLinkModel *destination,
    const UmiWorkbenchContextLinkApplicationLinkModel *source);
/**
 * Provide the workbench context link application link model hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_link_application_link_model_hash(
    const UmiWorkbenchContextLinkApplicationLinkModel *record);
/**
 * Provide the workbench context link application link model set primary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_application_link_model_set_primary(
    UmiWorkbenchContextLinkApplicationLinkModel *record,
    const char *value);
/**
 * Provide the workbench context link application link model set secondary operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_context_link_application_link_model_set_secondary(
    UmiWorkbenchContextLinkApplicationLinkModel *record,
    const char *value);
/**
 * Provide the workbench context link application link model touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_link_application_link_model_touch(
    UmiWorkbenchContextLinkApplicationLinkModel *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
