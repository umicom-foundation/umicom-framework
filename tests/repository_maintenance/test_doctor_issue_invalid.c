/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_doctor_issue_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for repository maintenance module doctor_issue.
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
#include "umicom/repository/doctor_issue.h"
int main(void){ assert(umi_repository_doctor_issue_add(NULL,UMI_REPOSITORY_DOCTOR_ERROR,"x","y")==UMI_STATUS_INVALID_ARGUMENT); return 0; }
