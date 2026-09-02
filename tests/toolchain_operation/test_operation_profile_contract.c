/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_operation_profile_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module operation_profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/toolchain/operation_profile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiToolchainOperationProfile p; umi_toolchain_operation_profile_init(&p, UMI_TOOLCHAIN_OPERATION_REPOSITORY_READ,"repo-read","Repo read"); assert(umi_toolchain_requirement_set_add(&p.requirements,UMI_TOOL_GIT,1)==UMI_STATUS_OK); assert(umi_toolchain_operation_profile_validate(&p)==UMI_STATUS_OK); return 0; }
