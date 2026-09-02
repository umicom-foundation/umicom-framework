/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_research_movement.c
 *
 * PURPOSE:
 *   Validate research movement behaviour in the trading foundation.
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
#include "umicom/trading/trading.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){
    UmiMovementEvent e=umi_movement_event_measure(30000,30120,0,30*60*1000,100,30*60*1000);assert(e.qualifies);assert(e.absolute_points==120);
    UmiResearchStats s;umi_research_stats_init(&s);umi_research_stats_add(&s,&e);assert(s.qualifying_moves==1U);assert(umi_research_stats_mean_points(&s)==120);return 0;
}
