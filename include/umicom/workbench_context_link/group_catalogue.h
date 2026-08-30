/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/group_catalogue.h
 *
 * PURPOSE:
 *   Manage deterministic colour-group profiles with bounded dynamically allocated storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_CATALOGUE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_CATALOGUE_H

#include "umicom/workbench_context_link/group_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkGroupCatalogue {
    UmiWorkbenchContextLinkGroupProfile *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextLinkGroupCatalogue;

void umi_workbench_context_link_group_catalogue_init(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue);
void umi_workbench_context_link_group_catalogue_destroy(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue);
UmiStatus umi_workbench_context_link_group_catalogue_upsert(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const UmiWorkbenchContextLinkGroupProfile *profile);
UmiStatus umi_workbench_context_link_group_catalogue_remove(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const char *group_id);
UmiWorkbenchContextLinkGroupProfile *
umi_workbench_context_link_group_catalogue_find(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const char *group_id);
const UmiWorkbenchContextLinkGroupProfile *
umi_workbench_context_link_group_catalogue_find_const(
    const UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const char *group_id);
size_t umi_workbench_context_link_group_catalogue_count(
    const UmiWorkbenchContextLinkGroupCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
