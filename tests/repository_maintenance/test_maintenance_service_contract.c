/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_maintenance_service_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module maintenance_service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/maintenance_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiRepositoryMaintenanceService s; umi_repository_maintenance_service_init(&s); assert(umi_repository_maintenance_service_evaluate(&s)==UMI_STATUS_OK); assert(s.report.healthy); return 0; }
