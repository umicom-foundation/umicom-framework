/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_branch_status_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for repository maintenance module branch_status.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/repository/branch_status.h"
int main(void){ assert(umi_repository_branch_status_set(NULL,"main",NULL,0U,0U,0)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
