/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_requirement_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module requirement.
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
#include "umicom/toolchain/requirement.h"
int main(void){ assert(umi_toolchain_requirement_validate(NULL)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
