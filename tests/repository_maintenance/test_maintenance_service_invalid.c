/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_maintenance_service_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for repository maintenance module maintenance_service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/repository/maintenance_service.h"
int main(void){ assert(umi_repository_maintenance_service_evaluate(NULL)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
