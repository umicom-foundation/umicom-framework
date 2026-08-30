/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_probe_report_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for toolchain operation module probe_report.
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
#include "umicom/toolchain/probe_report.h"
int main(void){ UmiToolchainProbeReport r; umi_toolchain_probe_report_init(&r,UMI_TOOL_GIT); r.validated=1; assert(umi_toolchain_probe_report_validate(&r)==UMI_STATUS_INVALID_STATE); return 0; }
