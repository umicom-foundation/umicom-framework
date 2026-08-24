/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_doctor_policy_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module doctor_policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/doctor_policy.h"
int main(void){ UmiRepositoryDoctorPolicy p; umi_repository_doctor_policy_default(&p); assert(umi_repository_doctor_policy_validate(&p)==UMI_STATUS_OK); assert(p.require_initialised_submodules); return 0; }
