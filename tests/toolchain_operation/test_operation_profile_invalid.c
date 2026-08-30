/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_operation_profile_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module operation_profile.
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
#include "umicom/toolchain/operation_profile.h"
int main(void){ assert(umi_toolchain_operation_profile_validate(NULL)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
