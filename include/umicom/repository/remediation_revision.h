/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_revision.h
 *
 * PURPOSE:
 *   Expose one stable revision helper for remediation-plan observers.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_REVISION_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_REVISION_H
#include <stdint.h>
#include "umicom/repository/remediation_service.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Expose one stable revision helper for remediation-plan observers. */
uint64_t umi_repository_remediation_revision_value(const UmiRepositoryRemediationService *service);
#ifdef __cplusplus
}
#endif
#endif
