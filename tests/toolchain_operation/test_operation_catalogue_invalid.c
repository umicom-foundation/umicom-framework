/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_operation_catalogue_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module operation_catalogue.
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
#include "umicom/toolchain/operation_catalogue.h"
int main(void){ UmiToolchainOperationProfile p; assert(umi_toolchain_operation_catalogue_profile(UMI_TOOLCHAIN_OPERATION_UNKNOWN,&p)==UMI_STATUS_NOT_FOUND); return 0; }
