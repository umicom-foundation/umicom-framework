/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_status_record.c
 *
 * PURPOSE:
 *   Provide a focused contract smoke test for the status record capability.
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

#include "umicom/repository/status_record.h"

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    UmiRepositoryStatusRecord r; return umi_repository_status_record_parse("? new.c",&r)==UMI_STATUS_OK && r.kind==UMI_REPOSITORY_STATUS_UNTRACKED ? 0:1;
}
