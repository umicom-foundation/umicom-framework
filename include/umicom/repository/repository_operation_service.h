/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/repository_operation_service.h
 *
 * PURPOSE:
 *   Expose status/doctor/audit/plan through one reusable Framework backend.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REPOSITORY_OPERATION_SERVICE_H
#define INCLUDE_UMICOM_REPOSITORY_REPOSITORY_OPERATION_SERVICE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "umicom/repository/repository_operation_request.h"
#include "umicom/repository/repository_operation_formatter.h"
/* Execute one read-only repository operation and optionally build a non-mutating plan. */
UmiStatus umi_repository_operation_service_execute(const UmiRepositoryOperationRequest *request,UmiRepositoryOperationResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
