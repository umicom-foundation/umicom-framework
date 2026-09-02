/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_tool_probe_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module tool_probe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/toolchain/tool_probe.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiToolchainToolProbeRequest r={0}; UmiToolInfo t; UmiToolchainProbeReport p; r.kind=UMI_TOOL_GIT; r.validate_version=0; { UmiStatus s=umi_toolchain_tool_probe(&r,&t,&p); assert(s==UMI_STATUS_OK || s==UMI_STATUS_NOT_FOUND); } return 0; }
