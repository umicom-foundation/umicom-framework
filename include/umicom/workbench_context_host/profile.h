/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/profile.h
 *
 * PURPOSE:
 *   Define a data-driven application profile containing context-link groups and panel endpoints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_PROFILE_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_PROFILE_H

#include "umicom/workbench_context_host/endpoint.h"
#include "umicom/workbench_context_host/group_definition.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextHostProfile {
    uint32_t structure_size;
    char profile_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char title[UMI_WORKBENCH_CONTEXT_HOST_TITLE_CAPACITY];
    UmiWorkbenchContextHostGroupDefinition groups[
        UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILE_GROUPS];
    size_t group_count;
    UmiWorkbenchContextHostEndpoint endpoints[
        UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILE_ENDPOINTS];
    size_t endpoint_count;
    uint64_t revision;
} UmiWorkbenchContextHostProfile;

void umi_workbench_context_host_profile_init(
    UmiWorkbenchContextHostProfile *profile,
    const char *profile_id,
    const char *application_id);
UmiStatus umi_workbench_context_host_profile_set_title(
    UmiWorkbenchContextHostProfile *profile,
    const char *title);
UmiStatus umi_workbench_context_host_profile_add_group(
    UmiWorkbenchContextHostProfile *profile,
    const UmiWorkbenchContextHostGroupDefinition *definition);
UmiStatus umi_workbench_context_host_profile_add_endpoint(
    UmiWorkbenchContextHostProfile *profile,
    const UmiWorkbenchContextHostEndpoint *endpoint);
UmiStatus umi_workbench_context_host_profile_validate(
    const UmiWorkbenchContextHostProfile *profile);
const char *umi_workbench_context_host_profile_default_group(
    const UmiWorkbenchContextHostProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
