/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/repository_operation_formatter.c
 *
 * PURPOSE:
 *   Implement format inspection and plan services into one operation result.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/repository_operation_formatter.h"
#include <stdio.h>
#include "umicom/repository/inspection_text.h"
#include "umicom/repository/remediation_text.h"
/* Reuse inspection/remediation formatters instead of duplicating presentation logic. */
UmiStatus umi_repository_operation_formatter_format(UmiRepositoryOperationKind kind,const UmiRepositoryRemediationService *service,UmiRepositoryOperationResult *out_result){UmiStatus s;if(!service||!out_result)return UMI_STATUS_INVALID_ARGUMENT;umi_repository_operation_result_init(out_result);out_result->issue_count=service->inspection.maintenance.report.issues.count+service->inspection.report.count;out_result->warning_count=service->inspection.maintenance.report.warnings+service->inspection.report.warnings;out_result->error_count=service->inspection.maintenance.report.errors+service->inspection.report.errors;out_result->action_count=service->plan.actions.count;if(kind==UMI_REPOSITORY_OPERATION_PLAN){s=umi_repository_remediation_text_format(&service->plan,out_result->text,sizeof(out_result->text));}else{s=umi_repository_inspection_text_format(&service->inspection,out_result->text,sizeof(out_result->text));}out_result->status=s;return s;}
