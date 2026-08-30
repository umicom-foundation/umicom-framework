/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_operation_service_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module operation_service.
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
#include "umicom/toolchain/operation_service.h"
int main(void){ assert(umi_toolchain_operation_service_prepare(NULL,UMI_TOOLCHAIN_OPERATION_RUN,NULL,NULL,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
