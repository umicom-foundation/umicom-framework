/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_capital_buffer.c
 *
 * PURPOSE:
 *   Verify represent a capital buffer expressed as a required ratio and amount.
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

#include "umicom/finance/prudential/capital_buffer.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialCapitalBuffer r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_capital_buffer_calculate(&r,"BUF",0.025,1000.0)!=UMI_STATUS_OK)return 1; return r.required_amount==25.0?0:2; }
