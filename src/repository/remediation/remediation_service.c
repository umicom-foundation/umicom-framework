/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_service.c
 *
 * PURPOSE:
 *   Implement compose inspection evidence and plan-only remediation into one Framework service.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/remediation_service.h"
#include <string.h>
#include "umicom/repository/remediation_builder.h"
/* Initialise composed inspection and remediation state. */
void umi_repository_remediation_service_init(UmiRepositoryRemediationService *service){if(!service)return;(void)memset(service,0,sizeof(*service));umi_repository_inspection_service_init(&service->inspection);umi_repository_remediation_policy_init(&service->policy);umi_repository_remediation_plan_init(&service->plan);}
/* Build an explicit proposal set only; mutations remain outside this service. */
UmiStatus umi_repository_remediation_service_plan(UmiRepositoryRemediationService *service,const UmiRepositoryInspectionContext *context){UmiStatus s;if(!service||!context)return UMI_STATUS_INVALID_ARGUMENT;s=umi_repository_inspection_service_refresh(&service->inspection,context);if(s==UMI_STATUS_OK)s=umi_repository_remediation_builder_build(&service->inspection.maintenance.report,&service->inspection.report,&service->plan);if(s==UMI_STATUS_OK)s=umi_repository_remediation_report_build(&service->plan,&service->policy,&service->report);service->last_status=s;if(s==UMI_STATUS_OK)++service->revision;return s;}
