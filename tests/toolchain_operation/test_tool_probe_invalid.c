/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_tool_probe_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module tool_probe.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/tool_probe.h"
int main(void){ UmiToolInfo t; UmiToolchainProbeReport p; assert(umi_toolchain_tool_probe(NULL,&t,&p)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
