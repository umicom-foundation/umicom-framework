/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_repository_operation.c
 *
 * PURPOSE:
 *   Provide a focused contract smoke test for the repository operation capability.
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

#include "umicom/repository/repository_operation.h"
#include <string.h>

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    return strcmp(umi_repository_operation_text(UMI_REPOSITORY_OPERATION_DOCTOR),"doctor")==0?0:1;
}
