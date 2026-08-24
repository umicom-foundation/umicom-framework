/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_status_summary_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module status_summary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/status_summary.h"
int main(void){ UmiRepositoryStatusSummary s; umi_repository_status_summary_init(&s); assert(umi_repository_status_summary_refresh(&s)==UMI_STATUS_OK); assert(s.state==UMI_REPOSITORY_MAINTENANCE_CLEAN); return 0; }
