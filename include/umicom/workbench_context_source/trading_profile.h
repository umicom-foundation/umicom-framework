/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/trading_profile.h
 *
 * PURPOSE:
 *   Register reusable Trader and TMS interaction sources for instrument, account and trade linking.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_TRADING_PROFILE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_TRADING_PROFILE_H
#include "umicom/workbench_context_source/service.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceTradingProfile {
    const char *trader_application_id;
    const char *tms_application_id;
    const char *trading_group_id;
    const char *operations_group_id;
} UmiWorkbenchContextSourceTradingProfile;

UmiWorkbenchContextSourceTradingProfile
umi_workbench_context_source_trading_profile_default(void);
UmiStatus umi_workbench_context_source_register_trading_profile(
    UmiWorkbenchContextSourceService *service,
    const UmiWorkbenchContextSourceTradingProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
