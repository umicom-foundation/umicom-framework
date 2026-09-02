/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_operation_types_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module operation_types.
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
#include "umicom/toolchain/operation_types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ assert(strcmp(umi_toolchain_operation_kind_text((UmiToolchainOperationKind)99), "unknown") == 0); assert(!umi_toolchain_operation_is_repository(UMI_TOOLCHAIN_OPERATION_BUILD_COMPILE)); return 0; }
