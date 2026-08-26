/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_operational_rwa.c
 *
 * PURPOSE:
 *   Verify calculate operational risk-weighted assets from business indicator and loss multiplier.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/operational_rwa.h"

int main(void) { UmiPrudentialOperationalRwa r; if(umi_pru_operational_rwa_calculate(&r,8.0,1.0,12.5)!=UMI_STATUS_OK)return 1; return r.rwa==100.0?0:2; }
