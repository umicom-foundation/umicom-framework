/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_inspection_types.c
 *
 * PURPOSE:
 *   Provide a focused contract smoke test for the inspection types capability.
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

#include "umicom/repository/inspection_types.h"

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    UmiRepositoryInspectionSnapshot s; umi_repository_inspection_snapshot_init(&s); return s.revision==0U ? 0 : 1;
}
