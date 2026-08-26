/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_capital_projection.c
 *
 * PURPOSE:
 *   Verify project capital after earnings, distributions, issuance and deductions.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/capital_projection.h"

int main(void) { UmiPrudentialCapitalProjection r; if(umi_pru_capital_projection_calculate(&r,100.0,10.0,5.0,3.0,2.0)!=UMI_STATUS_OK)return 1; return r.closing_capital==106.0?0:2; }
