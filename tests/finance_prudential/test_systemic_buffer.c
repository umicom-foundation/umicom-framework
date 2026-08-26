/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_systemic_buffer.c
 *
 * PURPOSE:
 *   Verify calculate a systemic capital buffer amount from RWA and configured rate.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/systemic_buffer.h"

int main(void) { UmiPrudentialSystemicBuffer r; if(umi_pru_systemic_buffer_calculate(&r,"BUF",0.025,1000.0)!=UMI_STATUS_OK)return 1; return r.required_amount==25.0?0:2; }
