/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_maintenance_service_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for repository maintenance module maintenance_service.
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
#include "umicom/repository/maintenance_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ assert(umi_repository_maintenance_service_evaluate(NULL)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
