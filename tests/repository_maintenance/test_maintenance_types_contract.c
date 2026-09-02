/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_maintenance_types_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module maintenance_types.
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
#include "umicom/repository/maintenance_types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ assert(strcmp(umi_repository_maintenance_state_text(UMI_REPOSITORY_MAINTENANCE_CLEAN),"clean")==0); return 0; }
