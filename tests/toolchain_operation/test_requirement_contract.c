/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_requirement_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module requirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/toolchain/requirement.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiToolchainRequirement r; umi_toolchain_requirement_init(&r, UMI_TOOL_GIT, 1); assert(umi_toolchain_requirement_validate(&r)==UMI_STATUS_OK); assert(r.required); return 0; }
