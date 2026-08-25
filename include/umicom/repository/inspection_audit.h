/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/inspection_audit.h
 *
 * PURPOSE:
 *   Combine existing doctor report semantics with the richer inspection report.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_INSPECTION_AUDIT_H
#define INCLUDE_UMICOM_REPOSITORY_INSPECTION_AUDIT_H
#include <stdint.h>
#include "umicom/repository/inspection_service.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Combine existing doctor report semantics with the richer inspection report. */
int umi_repository_inspection_audit_ready(const UmiRepositoryInspectionService *service);
#ifdef __cplusplus
}
#endif
#endif
