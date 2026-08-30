/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/application_binding.h
 *
 * PURPOSE:
 *   Define the reusable application context binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_APPLICATION_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_APPLICATION_BINDING_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkApplicationBinding {
    uint32_t structure_size;
    char binding_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkApplicationBinding;

void umi_workbench_context_link_application_binding_init(UmiWorkbenchContextLinkApplicationBinding *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_application_binding_validate(
    const UmiWorkbenchContextLinkApplicationBinding *record);
UmiStatus umi_workbench_context_link_application_binding_copy(
    UmiWorkbenchContextLinkApplicationBinding *destination,
    const UmiWorkbenchContextLinkApplicationBinding *source);
uint64_t umi_workbench_context_link_application_binding_hash(
    const UmiWorkbenchContextLinkApplicationBinding *record);
UmiStatus umi_workbench_context_link_application_binding_set_primary(
    UmiWorkbenchContextLinkApplicationBinding *record,
    const char *value);
UmiStatus umi_workbench_context_link_application_binding_set_secondary(
    UmiWorkbenchContextLinkApplicationBinding *record,
    const char *value);
void umi_workbench_context_link_application_binding_touch(
    UmiWorkbenchContextLinkApplicationBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
