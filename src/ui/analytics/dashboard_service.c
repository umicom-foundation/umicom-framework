/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_service.c
 *
 * PURPOSE:
 *   Register Framework-owned dashboards for reuse across thin applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_service.h"

#include <string.h>
/*
 * Initialise analytics dashboard service from caller-provided values so later operations
 * receive a known state.
 */
void umi_analytics_dashboard_service_init(UmiAnalyticsDashboardService *service){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(service!=NULL)memset(service,0,sizeof *service);}
/*
 * Provide the analytics dashboard service contains operation used by this module and its
 * client applications.
 */
int umi_analytics_dashboard_service_contains(const UmiAnalyticsDashboardService *service,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(service==NULL||id==NULL)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<service->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(service->ids[i],id)==0)return 1;return 0;}
/*
 * Add analytics dashboard service only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_analytics_dashboard_service_register(UmiAnalyticsDashboardService *service,const char *id){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(service==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_analytics_dashboard_service_contains(service,id))return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(service->count>=UMI_ANALYTICS_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;s=umi_analytics_copy_text(service->ids[service->count],UMI_ANALYTICS_ID_CAPACITY,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;service->count++;service->revision++;return UMI_STATUS_OK;}
