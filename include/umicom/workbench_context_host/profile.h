/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/profile.h
 *
 * PURPOSE:
 *   Define a data-driven application profile containing context-link groups and panel endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_PROFILE_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_PROFILE_H

#include "umicom/workbench_context_host/endpoint.h"
#include "umicom/workbench_context_host/group_definition.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context host profile data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise workbench context host profile from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_profile_init(
    UmiWorkbenchContextHostProfile *profile,
    const char *profile_id,
    const char *application_id);
/**
 * Provide the workbench context host profile set title operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_profile_set_title(
    UmiWorkbenchContextHostProfile *profile,
    const char *title);
/**
 * Provide the workbench context host profile add group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_profile_add_group(
    UmiWorkbenchContextHostProfile *profile,
    const UmiWorkbenchContextHostGroupDefinition *definition);
/**
 * Provide the workbench context host profile add endpoint operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_profile_add_endpoint(
    UmiWorkbenchContextHostProfile *profile,
    const UmiWorkbenchContextHostEndpoint *endpoint);
/**
 * Check that workbench context host profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_host_profile_validate(
    const UmiWorkbenchContextHostProfile *profile);
/**
 * Provide the workbench context host profile default group operation used by this module
 * and its client applications.
 */
const char *umi_workbench_context_host_profile_default_group(
    const UmiWorkbenchContextHostProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
