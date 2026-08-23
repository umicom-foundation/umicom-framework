/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/navigation_service.c
 *
 * PURPOSE:
 *   Implement reusable navigation composition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/navigation_service.h"

#include <stdlib.h>

UmiStatus umi_developer_navigation_service_create(
    UmiDeveloperNavigationService **out_service)
{
    UmiDeveloperNavigationService *service;
    UmiStatus status;

    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;

    service = (UmiDeveloperNavigationService *)calloc(
        1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    umi_developer_navigation_history_init(&service->history);
    umi_developer_bookmark_store_init(&service->bookmarks);
    umi_developer_reference_set_init(&service->references);

    status = umi_developer_symbol_index_create(&service->symbols);
    if (status != UMI_STATUS_OK) {
        free(service);
        return status;
    }

    service->revision = 1U;
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_developer_navigation_service_destroy(
    UmiDeveloperNavigationService *service)
{
    if (service == NULL) return;
    umi_developer_symbol_index_destroy(service->symbols);
    free(service);
}

UmiStatus umi_developer_navigation_service_visit(
    UmiDeveloperNavigationService *service,
    const UmiDeveloperProductivityLocation *location)
{
    UmiStatus status;

    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_developer_navigation_history_push(
        &service->history, location);
    if (status == UMI_STATUS_OK) service->revision += 1U;
    return status;
}
