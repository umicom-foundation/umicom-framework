/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_survival_horizon.c
 *
 * PURPOSE:
 *   Verify calculate survival horizon from daily net flows and opening liquidity.
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

#include "umicom/finance/prudential/survival_horizon.h"

int main(void) { UmiPrudentialSurvivalHorizon r; double f[3]={-20.0,-30.0,-60.0}; if(umi_pru_survival_horizon_calculate(&r,100.0,f,3U)!=UMI_STATUS_OK)return 1; if(r.exhausted!=1||r.survived_days!=2)return 2; return 0; }
