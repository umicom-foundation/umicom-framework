/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_service.c
 *
 * PURPOSE:
 *   Register Framework-owned dashboards for reuse across thin applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_service.h"

#include <string.h>
void umi_analytics_dashboard_service_init(UmiAnalyticsDashboardService *service){if(service!=NULL)memset(service,0,sizeof *service);}
int umi_analytics_dashboard_service_contains(const UmiAnalyticsDashboardService *service,const char *id){size_t i;if(service==NULL||id==NULL)return 0;for(i=0;i<service->count;i++)if(strcmp(service->ids[i],id)==0)return 1;return 0;}
UmiStatus umi_analytics_dashboard_service_register(UmiAnalyticsDashboardService *service,const char *id){UmiStatus s;if(service==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_analytics_dashboard_service_contains(service,id))return UMI_STATUS_ALREADY_EXISTS;if(service->count>=UMI_ANALYTICS_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;s=umi_analytics_copy_text(service->ids[service->count],UMI_ANALYTICS_ID_CAPACITY,id);if(s!=0)return s;service->count++;service->revision++;return UMI_STATUS_OK;}
