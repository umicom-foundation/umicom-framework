/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/profile_catalogue.h
 *
 * PURPOSE:
 *   Maintain reusable application context-host profiles without embedding product logic in the router.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_PROFILE_CATALOGUE_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_PROFILE_CATALOGUE_H
#include "umicom/workbench_context_host/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILES 32U
/**
 * Represent the workbench context host profile catalogue data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostProfileCatalogue {
    UmiWorkbenchContextHostProfile *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextHostProfileCatalogue;
/**
 * Initialise workbench context host profile catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_profile_catalogue_init(UmiWorkbenchContextHostProfileCatalogue *catalogue);
/**
 * Release or reset state held by workbench context host profile catalogue so the same
 * storage can be reused safely.
 */
void umi_workbench_context_host_profile_catalogue_destroy(UmiWorkbenchContextHostProfileCatalogue *catalogue);
/**
 * Provide the workbench context host profile catalogue upsert operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_profile_catalogue_upsert(
    UmiWorkbenchContextHostProfileCatalogue *catalogue,const UmiWorkbenchContextHostProfile *profile);
/**
 * Find workbench context host profile catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiWorkbenchContextHostProfile *umi_workbench_context_host_profile_catalogue_find(
    const UmiWorkbenchContextHostProfileCatalogue *catalogue,const char *profile_id);
/**
 * Remove workbench context host profile catalogue while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_context_host_profile_catalogue_remove(
    UmiWorkbenchContextHostProfileCatalogue *catalogue,const char *profile_id);
#ifdef __cplusplus
}
#endif
#endif
