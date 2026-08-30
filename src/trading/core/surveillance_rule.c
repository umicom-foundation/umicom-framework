/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/surveillance_rule.c
 *
 * PURPOSE:
 *   Define reusable market-surveillance thresholds and alert severity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/surveillance_rule.h"
#include <string.h>
/* Initialise and validate define reusable market-surveillance thresholds and alert severity.. */
UmiStatus umi_trading_surveillance_rule_init(UmiTradingSurveillanceRule *value,uint32_t threshold, uint32_t window_seconds, UmiTradingCoreSeverity severity) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->threshold=threshold;
    value->window_seconds=window_seconds;
    value->severity=severity;
    return umi_trading_surveillance_rule_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_surveillance_rule_valid(const UmiTradingSurveillanceRule *value) { return value!=NULL && (value->threshold>0U && value->window_seconds>0U && value->severity>=UMI_TRADING_CORE_INFO && value->severity<=UMI_TRADING_CORE_CRITICAL); }
