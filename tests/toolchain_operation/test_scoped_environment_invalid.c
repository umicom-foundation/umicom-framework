/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_scoped_environment_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module scoped_environment.
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
#include "umicom/toolchain/scoped_environment.h"
int main(void){ UmiEnvironmentPlan e; assert(umi_toolchain_scoped_environment(NULL,NULL,&e)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
