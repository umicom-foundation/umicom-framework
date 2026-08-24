/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_maintenance_types_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module maintenance_types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/repository/maintenance_types.h"
int main(void){ assert(strcmp(umi_repository_maintenance_state_text(UMI_REPOSITORY_MAINTENANCE_CLEAN),"clean")==0); return 0; }
