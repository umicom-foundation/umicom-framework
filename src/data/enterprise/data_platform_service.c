/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/data_platform_service.c
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
#include "umicom/data/enterprise/data_platform_service.h"
#include <string.h>

/* Service init composes existing Data Server behaviour rather than replacing backend execution. */ UmiStatus umi_data_data_platform_service_init(UmiDataPlatformService *service,const char *service_id,const UmiDataServerProfile *profile){UmiStatus s;if(service==NULL||service_id==NULL||profile==NULL||umi_data_data_server_profile_validate(profile)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(service,0,sizeof(*service));s=umi_data_enterprise_copy_text(service->service_id,sizeof(service->service_id),service_id);if(s!=UMI_STATUS_OK)return s;service->profile=*profile;umi_data_data_operation_queue_init(&service->queue);service->health.overall_state=UMI_DATA_ENTERPRISE_UNKNOWN;return UMI_STATUS_OK;}
/* Start transitions once and does not create hidden threads or network listeners. */ UmiStatus umi_data_data_platform_service_start(UmiDataPlatformService *service){if(service==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(service->started)return UMI_STATUS_ALREADY_EXISTS;service->started=true;service->revision++;return UMI_STATUS_OK;}
/* Maintenance mode is explicit so migrations/restores can quiesce normal admission. */ UmiStatus umi_data_data_platform_service_set_maintenance(UmiDataPlatformService *service,bool enabled){if(service==NULL||!service->started)return UMI_STATUS_INVALID_STATE;service->maintenance_mode=enabled;service->revision++;return UMI_STATUS_OK;}
/* Admission blocks when maintenance or failed/blocked health state is active. */ UmiStatus umi_data_data_platform_service_submit(UmiDataPlatformService *service,const UmiDataOperation *operation){if(service==NULL||operation==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(!service->started||service->maintenance_mode)return UMI_STATUS_UNAVAILABLE;if(service->health.overall_state==UMI_DATA_ENTERPRISE_FAILED||service->health.overall_state==UMI_DATA_ENTERPRISE_BLOCKED)return UMI_STATUS_UNAVAILABLE;service->revision++;return umi_data_data_operation_queue_push(&service->queue,operation);}
/* Health updates are snapshots supplied by canonical pool/replication/integrity services. */ UmiStatus umi_data_data_platform_service_update_health(UmiDataPlatformService *service,const UmiDataHealth *health){if(service==NULL||health==NULL)return UMI_STATUS_INVALID_ARGUMENT;service->health=*health;service->revision++;return UMI_STATUS_OK;}
