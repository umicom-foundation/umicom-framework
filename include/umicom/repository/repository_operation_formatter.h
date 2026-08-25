/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/repository_operation_formatter.h
 *
 * PURPOSE:
 *   Format inspection and plan services into one operation result.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REPOSITORY_OPERATION_FORMATTER_H
#define INCLUDE_UMICOM_REPOSITORY_REPOSITORY_OPERATION_FORMATTER_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "umicom/repository/repository_operation.h"
#include "umicom/repository/repository_operation_result.h"
#include "umicom/repository/remediation_service.h"
/* Format a completed remediation/inspection service according to operation kind. */
UmiStatus umi_repository_operation_formatter_format(UmiRepositoryOperationKind kind,const UmiRepositoryRemediationService *service,UmiRepositoryOperationResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
