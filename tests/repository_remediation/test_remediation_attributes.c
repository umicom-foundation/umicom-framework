/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_remediation/test_remediation_attributes.c
 *
 * PURPOSE:
 *   Provide a focused contract smoke test for the remediation attributes capability.
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

#include "umicom/repository/remediation_attributes.h"

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    (void)&umi_repository_remediation_attributes_plan; return 0;
}
