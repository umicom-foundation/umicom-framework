/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_reverse_stress_test.c
 *
 * PURPOSE:
 *   Verify estimate the linear shock required to exhaust available headroom.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/reverse_stress_test.h"

int main(void) { UmiPrudentialReverseStressTest r; if(umi_pru_reverse_stress_test_calculate(&r,20.0,100.0,0.5)!=UMI_STATUS_OK)return 1; return r.required_shock==0.4?0:2; }
