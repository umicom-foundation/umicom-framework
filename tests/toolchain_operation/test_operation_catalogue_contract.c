/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_operation_catalogue_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module operation_catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/toolchain/operation_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiToolchainOperationProfile p; assert(umi_toolchain_operation_catalogue_profile(UMI_TOOLCHAIN_OPERATION_REPOSITORY_READ,&p)==UMI_STATUS_OK); assert(p.requirements.count==1U); assert(p.requirements.items[0].kind==UMI_TOOL_GIT); assert(!p.requires_compiler); assert(!p.run_compile_probe); return 0; }
