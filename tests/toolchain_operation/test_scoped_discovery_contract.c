/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_scoped_discovery_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module scoped_discovery.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/toolchain/scoped_discovery.h"
#include "umicom/toolchain/operation_catalogue.h"
int main(void){ UmiToolchainOperationProfile p; UmiToolchainScopedDiscoveryRequest r={0}; UmiToolchainScopedDiscoveryReport o; assert(umi_toolchain_operation_catalogue_profile(UMI_TOOLCHAIN_OPERATION_RUN,&p)==UMI_STATUS_OK); r.operation=&p; assert(umi_toolchain_discover_scoped(&r,&o)==UMI_STATUS_OK); assert(o.complete); return 0; }
