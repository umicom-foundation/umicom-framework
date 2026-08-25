/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/repository_operation_result.h
 *
 * PURPOSE:
 *   Represent operation output and structured counts for CLI and workbench consumers.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REPOSITORY_OPERATION_RESULT_H
#define INCLUDE_UMICOM_REPOSITORY_REPOSITORY_OPERATION_RESULT_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRepositoryOperationResult { UmiStatus status; size_t issue_count; size_t warning_count; size_t error_count; size_t action_count; char text[UMI_REPOSITORY_INSPECTION_OUTPUT_CAPACITY]; } UmiRepositoryOperationResult;
/* Initialise an empty operation result. */
void umi_repository_operation_result_init(UmiRepositoryOperationResult *result);
#ifdef __cplusplus
}
#endif
#endif
