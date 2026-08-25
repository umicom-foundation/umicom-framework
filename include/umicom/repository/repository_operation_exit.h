/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/repository_operation_exit.h
 *
 * PURPOSE:
 *   Translate operation results into deterministic CLI-style exit codes.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REPOSITORY_OPERATION_EXIT_H
#define INCLUDE_UMICOM_REPOSITORY_REPOSITORY_OPERATION_EXIT_H
#include <stdint.h>
#include "umicom/repository/repository_operation_result.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Translate operation results into deterministic CLI-style exit codes. */
int umi_repository_operation_exit_code(const UmiRepositoryOperationResult *result);
#ifdef __cplusplus
}
#endif
#endif
