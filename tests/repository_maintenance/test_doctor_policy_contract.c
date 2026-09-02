/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_doctor_policy_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module doctor_policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/doctor_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiRepositoryDoctorPolicy p; umi_repository_doctor_policy_default(&p); assert(umi_repository_doctor_policy_validate(&p)==UMI_STATUS_OK); assert(p.require_initialised_submodules); return 0; }
