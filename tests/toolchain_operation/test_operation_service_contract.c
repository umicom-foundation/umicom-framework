/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_operation_service_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module operation_service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/toolchain/operation_service.h"
int main(void){ UmiToolchainOperationService s; umi_toolchain_operation_service_init(&s); assert(umi_toolchain_operation_service_prepare(&s,UMI_TOOLCHAIN_OPERATION_RUN,NULL,NULL,NULL,NULL)==UMI_STATUS_OK); assert(umi_toolchain_operation_service_ready(&s)); return 0; }
