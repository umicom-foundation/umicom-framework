/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_branch_status_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for repository maintenance module branch_status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/repository/branch_status.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ assert(umi_repository_branch_status_set(NULL,"main",NULL,0U,0U,0)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
