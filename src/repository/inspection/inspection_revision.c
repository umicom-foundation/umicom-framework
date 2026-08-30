/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/inspection_revision.c
 *
 * PURPOSE:
 *   Implement expose one stable revision helper for observers and workbench refresh logic.
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

#include "umicom/repository/inspection_revision.h"
/* Expose one stable revision helper for observers and workbench refresh logic. */
uint64_t umi_repository_inspection_revision_value(const UmiRepositoryInspectionService *service)
{
    return service != NULL ? service->revision : 0U;
}
