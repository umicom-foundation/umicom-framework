/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_requirement_set_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module requirement_set.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/toolchain/requirement_set.h"
int main(void){ UmiToolchainRequirementSet s; umi_toolchain_requirement_set_init(&s); assert(umi_toolchain_requirement_set_add(&s, UMI_TOOL_GIT,1)==UMI_STATUS_OK); assert(umi_toolchain_requirement_set_contains(&s,UMI_TOOL_GIT)); return 0; }
