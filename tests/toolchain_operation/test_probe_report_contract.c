/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_probe_report_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module probe_report.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/probe_report.h"
int main(void){ UmiToolchainProbeReport r; umi_toolchain_probe_report_init(&r,UMI_TOOL_GIT); r.found=1; r.validated=1; strcpy(r.path,"git"); assert(umi_toolchain_probe_report_validate(&r)==UMI_STATUS_OK); return 0; }
