/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/data_platform_service.h
 *
 * PURPOSE:
 *   Coordinate enterprise Data Server readiness, maintenance mode, snapshot revision and operation admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_DATA_PLATFORM_SERVICE_H
#define UMICOM_DATA_ENTERPRISE_DATA_PLATFORM_SERVICE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/data_server_profile.h"
#include "umicom/data/enterprise/data_health.h"
#include "umicom/data/enterprise/data_operation_queue.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataPlatformService { char service_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; UmiDataServerProfile profile; UmiDataHealth health; UmiDataOperationQueue queue; bool started; bool maintenance_mode; uint64_t revision; } UmiDataPlatformService;
/* Initialise the aggregate service around an existing Data Server profile. */ UmiStatus umi_data_data_platform_service_init(UmiDataPlatformService *service,const char *service_id,const UmiDataServerProfile *profile);
/* Start the service only when profile invariants are valid. */ UmiStatus umi_data_data_platform_service_start(UmiDataPlatformService *service);
/* Enter or leave maintenance mode; admission is blocked while enabled. */ UmiStatus umi_data_data_platform_service_set_maintenance(UmiDataPlatformService *service,bool enabled);
/* Admit an operation when service and health gates allow it. */ UmiStatus umi_data_data_platform_service_submit(UmiDataPlatformService *service,const UmiDataOperation *operation);
/* Update aggregate health evidence and service revision. */ UmiStatus umi_data_data_platform_service_update_health(UmiDataPlatformService *service,const UmiDataHealth *health);

#ifdef __cplusplus
}
#endif
#endif
