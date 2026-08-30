/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_branch_probe.c
 *
 * PURPOSE:
 *   Provide a focused contract smoke test for the branch probe capability.
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

#include "umicom/repository/branch_probe.h"

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    (void)&umi_repository_branch_probe_read; return 0;
}
