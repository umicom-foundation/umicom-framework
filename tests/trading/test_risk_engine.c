/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_risk_engine.c
 *
 * PURPOSE:
 *   Validate risk engine behaviour in the trading foundation.
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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){
    UmiRiskLimit l={5,1000000,10,5000};assert(umi_risk_limit_valid(&l));UmiOrderRequest r=test_order_request();
    UmiRiskDecision d=umi_pretrade_risk_evaluate(&r,&l,0,0);assert(d.allowed);
    r.quantity=100;d=umi_pretrade_risk_evaluate(&r,&l,0,0);assert(!d.allowed);return 0;
}
