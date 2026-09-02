/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/admin_model.h
 *
 * PURPOSE:
 *   Project Data Server and enterprise service health into a safe operational
 *   administration snapshot without exposing credentials or provider handles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_ADMIN_MODEL_H
#define UMICOM_DATA_WORKBENCH_ADMIN_MODEL_H

#include "umicom/data/data_server.h"
#include "umicom/data/enterprise/data_platform_service.h"
#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data admin model data shared with callers of this public contract.
 */
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

/**
 * Initialise data admin model from caller-provided values so later operations receive a
 * known state.
 */
void umi_data_admin_model_init(UmiDataAdminModel *model);
/**
 * Provide the data admin model refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_admin_model_refresh(
    UmiDataAdminModel *model,
    const UmiDataServer *server,
    const UmiDataPlatformService *service);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_ADMIN_MODEL_H */
