/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/permission.h
 *
 * PURPOSE:
 *   Define the reusable context-link permission record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PERMISSION_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PERMISSION_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkPermission {
    uint32_t structure_size;
    char permission_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char action_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkPermission;

void umi_workbench_context_link_permission_init(UmiWorkbenchContextLinkPermission *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_permission_validate(
    const UmiWorkbenchContextLinkPermission *record);
UmiStatus umi_workbench_context_link_permission_copy(
    UmiWorkbenchContextLinkPermission *destination,
    const UmiWorkbenchContextLinkPermission *source);
uint64_t umi_workbench_context_link_permission_hash(
    const UmiWorkbenchContextLinkPermission *record);
UmiStatus umi_workbench_context_link_permission_set_primary(
    UmiWorkbenchContextLinkPermission *record,
    const char *value);
UmiStatus umi_workbench_context_link_permission_set_secondary(
    UmiWorkbenchContextLinkPermission *record,
    const char *value);
void umi_workbench_context_link_permission_touch(
    UmiWorkbenchContextLinkPermission *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
