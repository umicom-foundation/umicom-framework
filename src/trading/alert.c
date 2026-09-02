/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/alert.c
 *
 * PURPOSE:
 *   Evaluate whether a value crosses a configured threshold.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of alert. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/alert.h"
/*
 * Provide the trading alert crossed operation used by this module and its client
 * applications.
 */
int umi_trading_alert_crossed(double previous,double current,double threshold){return (previous<threshold&&current>=threshold)||(previous>threshold&&current<=threshold);}
