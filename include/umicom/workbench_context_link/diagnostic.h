/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/diagnostic.h
 *
 * PURPOSE:
 *   Define the reusable context-link diagnostic record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_DIAGNOSTIC_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_DIAGNOSTIC_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_context_link_diagnostic_init(UmiWorkbenchContextLinkDiagnostic *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_diagnostic_validate(
    const UmiWorkbenchContextLinkDiagnostic *record);
UmiStatus umi_workbench_context_link_diagnostic_copy(
    UmiWorkbenchContextLinkDiagnostic *destination,
    const UmiWorkbenchContextLinkDiagnostic *source);
uint64_t umi_workbench_context_link_diagnostic_hash(
    const UmiWorkbenchContextLinkDiagnostic *record);
UmiStatus umi_workbench_context_link_diagnostic_set_primary(
    UmiWorkbenchContextLinkDiagnostic *record,
    const char *value);
UmiStatus umi_workbench_context_link_diagnostic_set_secondary(
    UmiWorkbenchContextLinkDiagnostic *record,
    const char *value);
void umi_workbench_context_link_diagnostic_touch(
    UmiWorkbenchContextLinkDiagnostic *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
