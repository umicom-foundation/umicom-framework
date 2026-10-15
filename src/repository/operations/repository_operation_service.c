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
#include <stdlib.h>
/* Compose the inspection/remediation services so applications do not duplicate repository orchestration. */
/* Preserve the stack-owned operation for review. The active implementation
 * validates the request before probing and keeps large service state on the heap. */
#if 0
UmiStatus umi_repository_operation_service_execute(const UmiRepositoryOperationRequest *request,UmiRepositoryOperationResult *out_result){UmiRepositoryInspectionContext context;UmiRepositoryRemediationService service;UmiStatus s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!request||!out_result)return UMI_STATUS_INVALID_ARGUMENT;umi_repository_inspection_context_init(&context,request->repository_root);umi_repository_remediation_service_init(&service);s=umi_repository_remediation_service_plan(&service,&context);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK){umi_repository_operation_result_init(out_result);out_result->status=s;return s;}return umi_repository_operation_formatter_format(request->kind,&service,out_result);}

#endif

UmiStatus umi_repository_operation_service_execute(const UmiRepositoryOperationRequest *request,
    UmiRepositoryOperationResult *out_result)
{
    if (out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_repository_operation_result_init(out_result);
    if (request == NULL || request->repository_root == NULL || request->repository_root[0] == '\0' ||
        request->kind < UMI_REPOSITORY_OPERATION_STATUS || request->kind > UMI_REPOSITORY_OPERATION_PLAN) {
        out_result->status = UMI_STATUS_INVALID_ARGUMENT;
        return out_result->status;
    }
    /* Inspection and remediation contain bounded but sizeable report arrays.
     * Allocating their owner on the heap keeps CLI and GTK worker stack needs
     * independent of the number of supported findings and proposals. */
    UmiRepositoryRemediationService *service = malloc(sizeof *service);
    if (service == NULL) {
        out_result->status = UMI_STATUS_OUT_OF_MEMORY;
        return out_result->status;
    }
    UmiRepositoryInspectionContext context;
    umi_repository_inspection_context_init(&context, request->repository_root);
    umi_repository_remediation_service_init(service);
    UmiStatus status = umi_repository_remediation_service_plan(service, &context);
    if (status == UMI_STATUS_OK)
        status = umi_repository_operation_formatter_format(request->kind, service, out_result);
    out_result->status = status;
    free(service);
    return status;
}
