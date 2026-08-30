/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_inspection_policy.c
 *
 * PURPOSE:
 *   Provide a focused contract smoke test for the inspection policy capability.
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

#include "umicom/repository/inspection_policy.h"

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    UmiRepositoryInspectionPolicy p; umi_repository_inspection_policy_init(&p); return p.require_gitlink_alignment ? 0 : 1;
}
