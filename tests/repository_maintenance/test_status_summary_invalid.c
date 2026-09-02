/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_status_summary_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for repository maintenance module status_summary.
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
#include "umicom/repository/status_summary.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ assert(umi_repository_status_summary_refresh(NULL)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
