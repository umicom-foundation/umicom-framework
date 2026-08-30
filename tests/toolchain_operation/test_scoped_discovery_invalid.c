/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_scoped_discovery_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module scoped_discovery.
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
#include "umicom/toolchain/scoped_discovery.h"
int main(void){ UmiToolchainScopedDiscoveryReport o; assert(umi_toolchain_discover_scoped(NULL,&o)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
