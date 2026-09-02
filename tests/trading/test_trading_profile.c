/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_trading_profile.c
 *
 * PURPOSE:
 *   Validate trading profile behaviour in the trading foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include <string.h>
#include "umicom/trading/trading.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){assert(umi_trading_profile_capability_count()>=7U);assert(strcmp(umi_trading_profile_capability_at(0),"umicom.finance")==0);assert(umi_trading_weekday_open(1U));assert(!umi_trading_weekday_open(7U));return 0;}
