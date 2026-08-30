/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/repository_operation.h
 *
 * PURPOSE:
 *   Define repository operation kinds exposed by the shared command backend.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REPOSITORY_OPERATION_H
#define INCLUDE_UMICOM_REPOSITORY_REPOSITORY_OPERATION_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiRepositoryOperationKind { UMI_REPOSITORY_OPERATION_STATUS=1, UMI_REPOSITORY_OPERATION_DOCTOR, UMI_REPOSITORY_OPERATION_AUDIT, UMI_REPOSITORY_OPERATION_GRAPH, UMI_REPOSITORY_OPERATION_PLAN } UmiRepositoryOperationKind;
/* Translate one operation kind to stable command text. */
const char *umi_repository_operation_text(UmiRepositoryOperationKind kind);
#ifdef __cplusplus
}
#endif
#endif
