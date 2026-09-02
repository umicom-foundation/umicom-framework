/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/navigation_service.c
 *
 * PURPOSE:
 *   Implement reusable navigation composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/navigation_service.h"

#include <stdlib.h>

/*
 * Initialise developer navigation service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_navigation_service_create(
    UmiDeveloperNavigationService **out_service)
{
    UmiDeveloperNavigationService *service;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;

    service = (UmiDeveloperNavigationService *)calloc(
        1U, sizeof(*service));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    umi_developer_navigation_history_init(&service->history);
    umi_developer_bookmark_store_init(&service->bookmarks);
    umi_developer_reference_set_init(&service->references);

    status = umi_developer_symbol_index_create(&service->symbols);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(service);
        return status;
    }

    service->revision = 1U;
    *out_service = service;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer navigation service so the same storage can be
 * reused safely.
 */
void umi_developer_navigation_service_destroy(
    UmiDeveloperNavigationService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    umi_developer_symbol_index_destroy(service->symbols);
    free(service);
}

/*
 * Provide the developer navigation service visit operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_navigation_service_visit(
    UmiDeveloperNavigationService *service,
    const UmiDeveloperProductivityLocation *location)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_developer_navigation_history_push(
        &service->history, location);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) service->revision += 1U;
    return status;
}
