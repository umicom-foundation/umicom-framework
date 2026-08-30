/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/repository_operation_service.c
 *
 * PURPOSE:
 *   Implement expose status/doctor/audit/plan through one reusable Framework backend.
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

#include "umicom/repository/repository_operation_service.h"
#include "umicom/repository/remediation_service.h"
/* Compose the inspection/remediation services so applications do not duplicate repository orchestration. */
UmiStatus umi_repository_operation_service_execute(const UmiRepositoryOperationRequest *request,UmiRepositoryOperationResult *out_result){UmiRepositoryInspectionContext context;UmiRepositoryRemediationService service;UmiStatus s;if(!request||!out_result)return UMI_STATUS_INVALID_ARGUMENT;umi_repository_inspection_context_init(&context,request->repository_root);umi_repository_remediation_service_init(&service);s=umi_repository_remediation_service_plan(&service,&context);if(s!=UMI_STATUS_OK){umi_repository_operation_result_init(out_result);out_result->status=s;return s;}return umi_repository_operation_formatter_format(request->kind,&service,out_result);}
