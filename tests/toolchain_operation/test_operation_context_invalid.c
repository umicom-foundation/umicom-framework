/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_operation_context_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module operation_context.
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
#include "umicom/toolchain/operation_context.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ assert(umi_toolchain_operation_context_prepare(UMI_TOOLCHAIN_OPERATION_UNKNOWN,NULL,NULL,NULL,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
