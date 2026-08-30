/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_doctor_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module doctor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/doctor.h"
int main(void){ UmiRepositoryStatusSummary s; UmiRepositoryDoctorPolicy p; UmiRepositoryDoctorReport r; umi_repository_status_summary_init(&s); umi_repository_doctor_policy_default(&p); assert(umi_repository_doctor_evaluate(&s,&p,&r)==UMI_STATUS_OK); assert(r.healthy); return 0; }
