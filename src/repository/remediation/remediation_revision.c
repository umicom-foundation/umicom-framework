/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_revision.c
 *
 * PURPOSE:
 *   Implement expose one stable revision helper for remediation-plan observers.
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

#include "umicom/repository/remediation_revision.h"
/* Expose one stable revision helper for remediation-plan observers. */
uint64_t umi_repository_remediation_revision_value(const UmiRepositoryRemediationService *service)
{
    return service != NULL ? service->revision : 0U;
}
