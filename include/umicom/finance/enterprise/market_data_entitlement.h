/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/market_data_entitlement.h
 *
 * PURPOSE:
 *   Enforce topic-prefix read entitlements independently from feed vendors.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_ENTITLEMENT_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_ENTITLEMENT_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseMarketDataEntitlement { char principal_id[UMI_ENTERPRISE_ID_CAPACITY]; char topic_prefix[UMI_ENTERPRISE_ID_CAPACITY]; int can_read; } UmiEnterpriseMarketDataEntitlement;
/* Initialise one reusable market-data read entitlement. */
UmiStatus umi_enterprise_market_data_entitlement_init(UmiEnterpriseMarketDataEntitlement *entitlement,const char *principal_id,const char *topic_prefix,int can_read);
/* Return one when the entitlement covers the requested topic. */
int umi_enterprise_market_data_entitlement_allows(const UmiEnterpriseMarketDataEntitlement *entitlement,const char *topic);

#ifdef __cplusplus
}
#endif

#endif
