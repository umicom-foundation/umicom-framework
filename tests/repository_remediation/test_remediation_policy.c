/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_remediation/test_remediation_policy.c
 *
 * PURPOSE:
 *   Provide a focused contract smoke test for the remediation policy capability.
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

#include "umicom/repository/remediation_policy.h"

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    UmiRepositoryRemediationPolicy p; umi_repository_remediation_policy_init(&p); return p.allow_destructive_automatic==0?0:1;
}
