/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_types.h
 *
 * PURPOSE:
 *   Define explicit plan-only remediation states and action risk classes.
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

#ifndef UMICOM_REPOSITORY_REMEDIATION_TYPES_H
#define UMICOM_REPOSITORY_REMEDIATION_TYPES_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_REPOSITORY_REMEDIATION_TEXT_CAPACITY 512U
#define UMI_REPOSITORY_REMEDIATION_ACTION_CAPACITY 64U
typedef enum UmiRepositoryRemediationRisk { UMI_REPOSITORY_REMEDIATION_SAFE=0, UMI_REPOSITORY_REMEDIATION_REVIEW=1, UMI_REPOSITORY_REMEDIATION_DESTRUCTIVE=2 } UmiRepositoryRemediationRisk;
typedef enum UmiRepositoryRemediationKind { UMI_REMEDIATION_CONFIGURE_UPSTREAM=1, UMI_REMEDIATION_REVIEW_DIVERGENCE, UMI_REMEDIATION_REVIEW_WORKTREE, UMI_REMEDIATION_REVIEW_INDEX, UMI_REMEDIATION_INIT_SUBMODULES, UMI_REMEDIATION_REVIEW_SUBMODULE_HEADS, UMI_REMEDIATION_REVIEW_GITLINKS, UMI_REMEDIATION_ADD_GITIGNORE, UMI_REMEDIATION_ADD_GITATTRIBUTES } UmiRepositoryRemediationKind;
/* Translate one remediation risk value to stable display text. */
const char *umi_repository_remediation_risk_text(UmiRepositoryRemediationRisk risk);
#ifdef __cplusplus
}
#endif
#endif
