/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/surveillance_alert.h
 *
 * PURPOSE:
 *   Capture explainable surveillance findings with stable participant and instrument identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_SURVEILLANCE_ALERT_H
#define UMICOM_TRADING_CORE_SURVEILLANCE_ALERT_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the trading surveillance alert data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingSurveillanceAlert { UmiFinancialId alert_id; UmiFinancialId participant_id; UmiFinancialId instrument_id; UmiTradingCoreSeverity severity; char reason[UMI_TRADING_CORE_TEXT_CAPACITY]; int64_t event_time_ms; } UmiTradingSurveillanceAlert;
/**
 * Initialise trading surveillance alert from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_surveillance_alert_init(UmiTradingSurveillanceAlert *alert,const char *alert_id,const UmiFinancialId *participant_id,const UmiFinancialId *instrument_id,UmiTradingCoreSeverity severity,const char *reason,int64_t event_time_ms);
#ifdef __cplusplus
}
#endif
#endif
