/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_positions_pnl.c
 *
 * PURPOSE:
 *   Validate positions pnl behaviour in the trading foundation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include "test_trading_common.h"
int main(void){
    UmiPosition p={0};p.instrument=test_instrument();assert(umi_position_apply_fill(&p,UMI_SIDE_BUY,2,25000)==UMI_STATUS_OK);
    assert(p.quantity==2);assert(p.average_price==25000);assert(umi_position_unrealised_pnl(&p,25010)==400);
    assert(umi_position_apply_fill(&p,UMI_SIDE_SELL,1,25020)==UMI_STATUS_OK);assert(p.realised_pnl==20);
    return 0;
}
