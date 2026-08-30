/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_operation_health_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module operation_health.
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
#include "umicom/toolchain/operation_health.h"
int main(void){ UmiToolchainOperationHealth h; assert(umi_toolchain_operation_health_from_context(NULL,&h)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
