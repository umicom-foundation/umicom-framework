/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_status_summary_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for repository maintenance module status_summary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/repository/status_summary.h"
int main(void){ assert(umi_repository_status_summary_refresh(NULL)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
