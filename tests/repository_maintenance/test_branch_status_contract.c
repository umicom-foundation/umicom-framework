/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_branch_status_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module branch_status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/branch_status.h"
int main(void){ UmiRepositoryBranchStatus s; assert(umi_repository_branch_status_set(&s,"main","origin/main",1U,0U,0)==UMI_STATUS_OK); assert(s.ahead==1U); return 0; }
