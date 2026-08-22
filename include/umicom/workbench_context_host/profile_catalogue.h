/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/profile_catalogue.h
 *
 * PURPOSE:
 *   Maintain reusable application context-host profiles without embedding product logic in the router.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_PROFILE_CATALOGUE_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_PROFILE_CATALOGUE_H
#include "umicom/workbench_context_host/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILES 32U
typedef struct UmiWorkbenchContextHostProfileCatalogue {
    UmiWorkbenchContextHostProfile *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextHostProfileCatalogue;
void umi_workbench_context_host_profile_catalogue_init(UmiWorkbenchContextHostProfileCatalogue *catalogue);
void umi_workbench_context_host_profile_catalogue_destroy(UmiWorkbenchContextHostProfileCatalogue *catalogue);
UmiStatus umi_workbench_context_host_profile_catalogue_upsert(
    UmiWorkbenchContextHostProfileCatalogue *catalogue,const UmiWorkbenchContextHostProfile *profile);
const UmiWorkbenchContextHostProfile *umi_workbench_context_host_profile_catalogue_find(
    const UmiWorkbenchContextHostProfileCatalogue *catalogue,const char *profile_id);
UmiStatus umi_workbench_context_host_profile_catalogue_remove(
    UmiWorkbenchContextHostProfileCatalogue *catalogue,const char *profile_id);
#ifdef __cplusplus
}
#endif
#endif
