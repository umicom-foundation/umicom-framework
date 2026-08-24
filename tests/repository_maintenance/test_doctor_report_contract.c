/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_doctor_report_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module doctor_report.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/doctor_report.h"
int main(void){ UmiRepositoryDoctorReport r; umi_repository_doctor_report_init(&r); assert(umi_repository_doctor_issue_add(&r.issues,UMI_REPOSITORY_DOCTOR_WARNING,"w","warn")==UMI_STATUS_OK); assert(umi_repository_doctor_report_finalize(&r)==UMI_STATUS_OK); assert(r.warnings==1U); return 0; }
