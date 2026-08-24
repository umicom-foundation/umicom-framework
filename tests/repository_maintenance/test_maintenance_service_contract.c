/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_maintenance_service_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module maintenance_service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/maintenance_service.h"
int main(void){ UmiRepositoryMaintenanceService s; umi_repository_maintenance_service_init(&s); assert(umi_repository_maintenance_service_evaluate(&s)==UMI_STATUS_OK); assert(s.report.healthy); return 0; }
