/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_submodule_probe.c
 *
 * PURPOSE:
 *   Provide a focused contract smoke test for the submodule probe capability.
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

#include "umicom/repository/submodule_probe.h"

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    (void)&umi_repository_submodule_probe_read; return 0;
}
