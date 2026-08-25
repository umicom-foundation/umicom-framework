/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/market_data_service.c
 *
 * PURPOSE:
 *   Aggregate feed cache and freshness evidence into a reusable market-data service.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/market_data_service.h"

#include <string.h>
UmiStatus umi_enterprise_market_data_service_init(UmiEnterpriseMarketDataService *s,int64_t max_age){ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(s,0,sizeof *s); umi_enterprise_market_data_cache_init(&s->cache); return umi_enterprise_market_data_freshness_policy_init(&s->freshness,max_age); }
UmiStatus umi_enterprise_market_data_service_publish(UmiEnterpriseMarketDataService *s,const UmiEnterpriseMarketDataUpdate *u){ UmiStatus st; if(s==NULL||u==NULL)return UMI_STATUS_INVALID_ARGUMENT; st=umi_enterprise_market_data_cache_put(&s->cache,u); if(st==UMI_STATUS_OK)s->accepted_updates++; else s->rejected_updates++; return st; }
