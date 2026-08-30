/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/inspection_health.h
 *
 * PURPOSE:
 *   Project inspection service state into a simple healthy/degraded/error code.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_INSPECTION_HEALTH_H
#define INCLUDE_UMICOM_REPOSITORY_INSPECTION_HEALTH_H
#include <stdint.h>
#include "umicom/repository/inspection_service.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Project inspection service state into a simple healthy/degraded/error code. */
int umi_repository_inspection_health_code(const UmiRepositoryInspectionService *service);
#ifdef __cplusplus
}
#endif
#endif
