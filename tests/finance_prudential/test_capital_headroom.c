/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_capital_headroom.c
 *
 * PURPOSE:
 *   Verify measure eligible capital headroom over the aggregate requirement.
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

#include "umicom/finance/prudential/capital_headroom.h"

int main(void) { UmiPrudentialCapitalHeadroom r; if(umi_pru_capital_headroom_calculate(&r,120.0,100.0)!=UMI_STATUS_OK)return 1; if(r.headroom!=20.0||r.breached!=0)return 2; if(umi_pru_capital_headroom_calculate(&r,80.0,100.0)!=UMI_STATUS_OK||r.breached!=1)return 3; return 0; }
