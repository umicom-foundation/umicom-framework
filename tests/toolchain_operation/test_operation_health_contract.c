/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_operation_health_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module operation_health.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/toolchain/operation_health.h"
int main(void){ UmiToolchainOperationContext c; UmiToolchainOperationHealth h; assert(umi_toolchain_operation_context_prepare(UMI_TOOLCHAIN_OPERATION_RUN,NULL,NULL,NULL,NULL,&c)==UMI_STATUS_OK); assert(umi_toolchain_operation_health_from_context(&c,&h)==UMI_STATUS_OK); assert(h.healthy); return 0; }
