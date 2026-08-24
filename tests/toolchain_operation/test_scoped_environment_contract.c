/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_scoped_environment_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module scoped_environment.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/toolchain/scoped_environment.h"
#include "umicom/toolchain/operation_catalogue.h"
int main(void){ UmiToolchainProfile p; UmiToolchainOperationProfile op; UmiEnvironmentPlan env; umi_toolchain_profile_init(&p); assert(umi_toolchain_operation_catalogue_profile(UMI_TOOLCHAIN_OPERATION_RUN,&op)==UMI_STATUS_OK); assert(umi_toolchain_scoped_environment(&p,&op,&env)==UMI_STATUS_OK); return 0; }
