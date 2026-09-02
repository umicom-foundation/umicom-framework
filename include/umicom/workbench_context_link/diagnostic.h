/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/diagnostic.h
 *
 * PURPOSE:
 *   Define the reusable context-link diagnostic record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_DIAGNOSTIC_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_DIAGNOSTIC_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link diagnostic data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkDiagnostic {
    uint32_t structure_size;
    char diagnostic_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char message[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkDiagnostic;

/**
 * Initialise workbench context link diagnostic from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_diagnostic_init(UmiWorkbenchContextLinkDiagnostic *record,
                                           const char *identity);
/**
 * Check that workbench context link diagnostic satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_diagnostic_validate(
    const UmiWorkbenchContextLinkDiagnostic *record);
/**
 * Copy workbench context link diagnostic into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_diagnostic_copy(
    UmiWorkbenchContextLinkDiagnostic *destination,
    const UmiWorkbenchContextLinkDiagnostic *source);
/**
 * Provide the workbench context link diagnostic hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_diagnostic_hash(
    const UmiWorkbenchContextLinkDiagnostic *record);
/**
 * Provide the workbench context link diagnostic set primary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_diagnostic_set_primary(
    UmiWorkbenchContextLinkDiagnostic *record,
    const char *value);
/**
 * Provide the workbench context link diagnostic set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_diagnostic_set_secondary(
    UmiWorkbenchContextLinkDiagnostic *record,
    const char *value);
/**
 * Provide the workbench context link diagnostic touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_diagnostic_touch(
    UmiWorkbenchContextLinkDiagnostic *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
