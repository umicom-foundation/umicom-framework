/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/inspection_revision.h
 *
 * PURPOSE:
 *   Expose one stable revision helper for observers and workbench refresh logic.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_INSPECTION_REVISION_H
#define INCLUDE_UMICOM_REPOSITORY_INSPECTION_REVISION_H
#include <stdint.h>
#include "umicom/repository/inspection_service.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Expose one stable revision helper for observers and workbench refresh logic. */
uint64_t umi_repository_inspection_revision_value(const UmiRepositoryInspectionService *service);
#ifdef __cplusplus
}
#endif
#endif
