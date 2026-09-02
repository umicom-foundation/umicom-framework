/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_xva_profile.c
 *
 * PURPOSE:
 *   Implement the test xva profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include <math.h>
#include "umicom/finance/quant/xva_profile.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiQuantXvaProfile p; CHECK(umi_quant_xva_profile_init(&p,10.0,2.0,3.0,1.0)==UMI_STATUS_OK); CHECK(umi_quant_xva_profile_net(&p)<-11.9&&umi_quant_xva_profile_net(&p)>-12.1);
    return 0;
}
