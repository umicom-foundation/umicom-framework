/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/group_profile.h
 *
 * PURPOSE:
 *   Define the reusable group profile contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_PROFILE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_PROFILE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkGroupProfile {
    uint32_t structure_size;
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char title[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char owner_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkGroupProfile;

void umi_workbench_context_link_group_profile_init(UmiWorkbenchContextLinkGroupProfile *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_group_profile_validate(
    const UmiWorkbenchContextLinkGroupProfile *record);
UmiStatus umi_workbench_context_link_group_profile_copy(
    UmiWorkbenchContextLinkGroupProfile *destination,
    const UmiWorkbenchContextLinkGroupProfile *source);
uint64_t umi_workbench_context_link_group_profile_hash(
    const UmiWorkbenchContextLinkGroupProfile *record);
UmiStatus umi_workbench_context_link_group_profile_set_primary(
    UmiWorkbenchContextLinkGroupProfile *record,
    const char *value);
UmiStatus umi_workbench_context_link_group_profile_set_secondary(
    UmiWorkbenchContextLinkGroupProfile *record,
    const char *value);
void umi_workbench_context_link_group_profile_touch(
    UmiWorkbenchContextLinkGroupProfile *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
