/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_position_book_portfolio.c
 *
 * PURPOSE:
 *   Validate position book portfolio behaviour in the trading foundation.
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
#include "test_trading_common.h"
int main(void){
    UmiPositionBook b;umi_position_book_init(&b);UmiInstrument i=test_instrument();UmiPosition *p=NULL;assert(umi_position_book_get(&b,&i,1,&p)==UMI_STATUS_OK);assert(p!=NULL);p->quantity=-3;
    assert(umi_portfolio_gross_quantity(&b)==3);UmiPosition *again=NULL;assert(umi_position_book_get(&b,&i,0,&again)==UMI_STATUS_OK);assert(again==p);return 0;
}
