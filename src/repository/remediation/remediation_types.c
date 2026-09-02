/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_types.c
 *
 * PURPOSE:
 *   Provide a translation function for plan-only remediation risk values.
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

#include "umicom/repository/remediation_types.h"
/* Translate remediation risk to stable text for CLI/UI adapters. */
const char *umi_repository_remediation_risk_text(UmiRepositoryRemediationRisk risk)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch(risk){case UMI_REPOSITORY_REMEDIATION_SAFE:return "safe";case UMI_REPOSITORY_REMEDIATION_REVIEW:return "review";case UMI_REPOSITORY_REMEDIATION_DESTRUCTIVE:return "destructive";default:return "unknown";}
}
