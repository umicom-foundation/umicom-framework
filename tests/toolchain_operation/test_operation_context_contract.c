/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_operation_context_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module operation_context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/toolchain/operation_context.h"
int main(void){ UmiToolchainOperationContext c; assert(umi_toolchain_operation_context_prepare(UMI_TOOLCHAIN_OPERATION_RUN,NULL,NULL,NULL,NULL,&c)==UMI_STATUS_OK); assert(c.ready); return 0; }
