/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/repository_operation_request.h
 *
 * PURPOSE:
 *   Represent a repository operation request independently of CLI argument parsing.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REPOSITORY_OPERATION_REQUEST_H
#define INCLUDE_UMICOM_REPOSITORY_REPOSITORY_OPERATION_REQUEST_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "umicom/repository/repository_operation.h"
typedef struct UmiRepositoryOperationRequest { UmiRepositoryOperationKind kind; const char *repository_root; int json_output; int include_plan; } UmiRepositoryOperationRequest;
/* Initialise a request with a path and operation kind. */
void umi_repository_operation_request_init(UmiRepositoryOperationRequest *request,UmiRepositoryOperationKind kind,const char *repository_root);
#ifdef __cplusplus
}
#endif
#endif
