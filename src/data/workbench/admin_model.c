/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/admin_model.c
 *
 * PURPOSE:
 *   Implement safe Data Server and enterprise-service administration snapshots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/admin_model.h"

#include <string.h>

void umi_data_admin_model_init(UmiDataAdminModel *model)
{
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->struct_size = (uint32_t)sizeof(*model);
    model->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    model->revision = 1U;
}

UmiStatus umi_data_admin_model_refresh(
    UmiDataAdminModel *model,
    const UmiDataServer *server,
    const UmiDataPlatformService *service)
{
    UmiStatus status;
    if (model == NULL || server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_data_server_snapshot(server, &model->server);
    if (status != UMI_STATUS_OK) return status;
    model->transaction_active = model->server.transaction_active;
    if (service != NULL) {
        model->health = service->health;
        model->queued_operations = service->queue.count;
        model->service_started = service->started;
        model->maintenance_mode = service->maintenance_mode;
        model->healthy = service->started && !service->maintenance_mode &&
            service->health.overall_state == UMI_DATA_ENTERPRISE_READY;
    } else {
        model->health.overall_state = UMI_DATA_ENTERPRISE_UNKNOWN;
        model->healthy = model->server.last_error[0] == '\0';
    }
    model->revision += 1U;
    return UMI_STATUS_OK;
}
