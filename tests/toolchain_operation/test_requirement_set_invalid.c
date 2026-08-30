/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_requirement_set_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module requirement_set.
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
#include "umicom/toolchain/requirement_set.h"
int main(void){ UmiToolchainRequirementSet s; umi_toolchain_requirement_set_init(&s); assert(umi_toolchain_requirement_set_add(&s,UMI_TOOL_GIT,1)==UMI_STATUS_OK); assert(umi_toolchain_requirement_set_add(&s,UMI_TOOL_GIT,1)==UMI_STATUS_ALREADY_EXISTS); return 0; }
