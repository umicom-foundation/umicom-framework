/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_prudential_snapshot.c
 *
 * PURPOSE:
 *   Verify capture aggregate capital, liquidity and stress readiness evidence.
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

#include "umicom/finance/prudential/prudential_snapshot.h"

int main(void) { UmiPrudentialSnapshot s; if(umi_pru_prudential_snapshot_init(&s,1U,0.12,0.16,1.2,1.1,0U,0U)!=UMI_STATUS_OK)return 1; return s.cet1_ratio==0.12&&s.lcr==1.2?0:2; }
