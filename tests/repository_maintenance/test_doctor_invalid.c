/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_doctor_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for repository maintenance module doctor.
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
#include "umicom/repository/doctor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiRepositoryDoctorReport r; assert(umi_repository_doctor_evaluate(NULL,NULL,&r)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
