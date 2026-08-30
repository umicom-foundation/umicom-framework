/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_status_probe.c
 *
 * PURPOSE:
 *   Verify the shared porcelain-v2 repository status probe remains exposed through Framework.
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

#include "umicom/repository/status_probe.h"

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    (void)&umi_repository_status_probe_read;
    return 0;
}
