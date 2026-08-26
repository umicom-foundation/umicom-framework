/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_prudential_limit.c
 *
 * PURPOSE:
 *   Verify define a prudential minimum or maximum control limit.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/prudential_limit.h"

int main(void) { UmiPrudentialLimit l; if(umi_pru_prudential_limit_init(&l,"LCR",1.0,1,UMI_PRU_SEVERITY_BREACH)!=UMI_STATUS_OK)return 1; return l.is_minimum==1?0:2; }
