/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_countercyclical_buffer.c
 *
 * PURPOSE:
 *   Verify calculate a countercyclical capital buffer amount from RWA and configured rate.
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

#include "umicom/finance/prudential/countercyclical_buffer.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialCountercyclicalBuffer r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_countercyclical_buffer_calculate(&r,"BUF",0.025,1000.0)!=UMI_STATUS_OK)return 1; return r.required_amount==25.0?0:2; }
