/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/profile_apply.h
 *
 * PURPOSE:
 *   Apply a data-driven host profile to the canonical context-link service and endpoint registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_PROFILE_APPLY_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_PROFILE_APPLY_H
#include "umicom/workbench_context_host/profile.h"
#include "umicom/workbench_context_host/endpoint_registry.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Perform workbench context host profile through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_workbench_context_host_profile_apply(
    const UmiWorkbenchContextHostProfile *profile,
    UmiWorkbenchContextLinkService *link_service,
    UmiWorkbenchContextHostEndpointRegistry *endpoints,
    char *out_default_group,
    size_t out_default_group_capacity);
#ifdef __cplusplus
}
#endif
#endif
