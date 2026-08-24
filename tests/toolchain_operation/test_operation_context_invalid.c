/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_operation_context_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module operation_context.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/operation_context.h"
int main(void){ assert(umi_toolchain_operation_context_prepare(UMI_TOOLCHAIN_OPERATION_UNKNOWN,NULL,NULL,NULL,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
