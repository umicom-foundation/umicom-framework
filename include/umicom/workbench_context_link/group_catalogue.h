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

/**
 * Represent the workbench context link group catalogue data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkGroupCatalogue {
    UmiWorkbenchContextLinkGroupProfile *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextLinkGroupCatalogue;

/**
 * Initialise workbench context link group catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_group_catalogue_init(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue);
/**
 * Release or reset state held by workbench context link group catalogue so the same
 * storage can be reused safely.
 */
void umi_workbench_context_link_group_catalogue_destroy(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue);
/**
 * Provide the workbench context link group catalogue upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_group_catalogue_upsert(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const UmiWorkbenchContextLinkGroupProfile *profile);
/**
 * Remove workbench context link group catalogue while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_context_link_group_catalogue_remove(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const char *group_id);
/**
 * Find workbench context link group catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiWorkbenchContextLinkGroupProfile *
umi_workbench_context_link_group_catalogue_find(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const char *group_id);
/**
 * Provide the workbench context link group catalogue find const operation used by this
 * module and its client applications.
 */
const UmiWorkbenchContextLinkGroupProfile *
umi_workbench_context_link_group_catalogue_find_const(
    const UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const char *group_id);
/**
 * Return the number of records represented by workbench context link group catalogue
 * without changing their state.
 */
size_t umi_workbench_context_link_group_catalogue_count(
    const UmiWorkbenchContextLinkGroupCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
