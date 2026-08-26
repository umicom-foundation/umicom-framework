/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_projection.c
 *
 * PURPOSE:
 *   Verify project a base value across a bounded stress path.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_projection.h"

int main(void) { UmiPrudentialStressProjection r; double m[2]={1.0,0.8}; if(umi_pru_stress_projection_calculate(&r,100.0,m,2U)!=UMI_STATUS_OK)return 1; return r.values[1]==80.0?0:2; }
