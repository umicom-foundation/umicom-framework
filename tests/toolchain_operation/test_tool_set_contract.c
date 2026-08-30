/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_tool_set_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module tool_set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/toolchain/tool_set.h"
int main(void){ UmiToolchainToolSet s; umi_toolchain_tool_set_init(&s); assert(umi_toolchain_tool_set_add(&s, UMI_TOOL_GIT)==UMI_STATUS_OK); assert(umi_toolchain_tool_set_contains(&s, UMI_TOOL_GIT)); assert(umi_toolchain_tool_set_count(&s)==1U); return 0; }
