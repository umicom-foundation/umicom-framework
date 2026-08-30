/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/surveillance_alert.c
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
#include "umicom/trading/core/surveillance_alert.h"

#include <string.h>
UmiStatus umi_trading_surveillance_alert_init(UmiTradingSurveillanceAlert *alert,const char *alert_id,const UmiFinancialId *participant_id,const UmiFinancialId *instrument_id,UmiTradingCoreSeverity severity,const char *reason,int64_t event_time_ms){if(alert==NULL||participant_id==NULL||instrument_id==NULL||reason==NULL||event_time_ms<0||severity<UMI_TRADING_CORE_INFO||severity>UMI_TRADING_CORE_CRITICAL)return UMI_STATUS_INVALID_ARGUMENT;memset(alert,0,sizeof *alert);if(umi_trading_core_id_assign(&alert->alert_id,alert_id)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;alert->participant_id=*participant_id;alert->instrument_id=*instrument_id;alert->severity=severity;alert->event_time_ms=event_time_ms;return umi_trading_core_copy_text(alert->reason,sizeof alert->reason,reason);}
