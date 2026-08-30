/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_credit_rwa.c
 *
 * PURPOSE:
 *   Verify calculate credit risk-weighted assets from exposure and risk weight.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/credit_rwa.h"

int main(void) { UmiPrudentialCreditRwa r; if(umi_pru_credit_rwa_calculate(&r,100.0,0.75)!=UMI_STATUS_OK)return 1; return r.rwa==75.0?0:2; }
