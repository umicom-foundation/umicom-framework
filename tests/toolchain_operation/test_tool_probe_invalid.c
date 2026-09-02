/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_tool_probe_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module tool_probe.
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
#include "umicom/toolchain/tool_probe.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiToolInfo t; UmiToolchainProbeReport p; assert(umi_toolchain_tool_probe(NULL,&t,&p)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
