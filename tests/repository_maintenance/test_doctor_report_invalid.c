/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_doctor_report_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for repository maintenance module doctor_report.
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
#include "umicom/repository/doctor_report.h"
int main(void){ assert(umi_repository_doctor_report_finalize(NULL)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
