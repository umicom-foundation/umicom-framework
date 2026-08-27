/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/admin_model.h
 *
 * PURPOSE:
 *   Project Data Server and enterprise service health into a safe operational
 *   administration snapshot without exposing credentials or provider handles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_ADMIN_MODEL_H
#define UMICOM_DATA_WORKBENCH_ADMIN_MODEL_H

#include "umicom/data/data_server.h"
#include "umicom/data/enterprise/data_platform_service.h"
#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataAdminModel {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDataServerSnapshot server;
    UmiDataHealth health;
    size_t queued_operations;
    int service_started;
    int maintenance_mode;
    int transaction_active;
    int healthy;
    uint64_t revision;
} UmiDataAdminModel;

void umi_data_admin_model_init(UmiDataAdminModel *model);
UmiStatus umi_data_admin_model_refresh(
    UmiDataAdminModel *model,
    const UmiDataServer *server,
    const UmiDataPlatformService *service);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_ADMIN_MODEL_H */
