/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_maintenance_types_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for repository maintenance module maintenance_types.
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
int main(void){ assert(strcmp(umi_repository_doctor_severity_text((UmiRepositoryDoctorSeverity)99),"unknown")==0); return 0; }
