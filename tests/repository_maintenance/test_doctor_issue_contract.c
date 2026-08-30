/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_doctor_issue_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module doctor_issue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/doctor_issue.h"
int main(void){ UmiRepositoryDoctorIssueList l; umi_repository_doctor_issue_list_init(&l); assert(umi_repository_doctor_issue_add(&l,UMI_REPOSITORY_DOCTOR_WARNING,"x","message")==UMI_STATUS_OK); assert(l.count==1U); return 0; }
