/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_path.c
 *
 * PURPOSE:
 *   Verify store a bounded time path of stress multipliers.
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

#include "umicom/finance/prudential/stress_path.h"

int main(void) { UmiPrudentialStressPath p={0}; if(umi_pru_stress_path_add(&p,0.9)!=UMI_STATUS_OK)return 1; if(umi_pru_stress_path_add(&p,0.8)!=UMI_STATUS_OK)return 2; return p.count==2U&&p.multipliers[1]==0.8?0:3; }
