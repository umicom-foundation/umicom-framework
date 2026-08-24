/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_tool_probe_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module tool_probe.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/toolchain/tool_probe.h"
int main(void){ UmiToolchainToolProbeRequest r={0}; UmiToolInfo t; UmiToolchainProbeReport p; r.kind=UMI_TOOL_GIT; r.validate_version=0; { UmiStatus s=umi_toolchain_tool_probe(&r,&t,&p); assert(s==UMI_STATUS_OK || s==UMI_STATUS_NOT_FOUND); } return 0; }
