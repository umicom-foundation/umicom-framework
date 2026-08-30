/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_tool_set_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module tool_set.
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
#include "umicom/toolchain/tool_set.h"
int main(void){ assert(umi_toolchain_tool_set_add(NULL,UMI_TOOL_GIT)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
