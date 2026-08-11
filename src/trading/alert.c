/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/alert.c
 *
 * PURPOSE:
 *   Evaluate whether a value crosses a configured threshold.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of alert. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/alert.h"
int umi_trading_alert_crossed(double previous,double current,double threshold){return (previous<threshold&&current>=threshold)||(previous>threshold&&current<=threshold);}
