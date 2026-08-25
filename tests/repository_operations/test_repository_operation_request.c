/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_repository_operation_request.c
 *
 * PURPOSE:
 *   Provide a focused contract smoke test for the repository operation request capability.
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

#include "umicom/repository/repository_operation_request.h"

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    UmiRepositoryOperationRequest r; umi_repository_operation_request_init(&r,UMI_REPOSITORY_OPERATION_STATUS,"."); return r.repository_root!=NULL?0:1;
}
