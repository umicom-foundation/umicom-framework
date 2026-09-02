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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/market_data_service.h"

#include <string.h>
/*
 * Initialise enterprise market data service from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_enterprise_market_data_service_init(UmiEnterpriseMarketDataService *s,int64_t max_age){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(s,0,sizeof *s); umi_enterprise_market_data_cache_init(&s->cache); return umi_enterprise_market_data_freshness_policy_init(&s->freshness,max_age); }
/*
 * Provide the enterprise market data service publish operation used by this module and its
 * client applications.
 */
UmiStatus umi_enterprise_market_data_service_publish(UmiEnterpriseMarketDataService *s,const UmiEnterpriseMarketDataUpdate *u){ UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||u==NULL)return UMI_STATUS_INVALID_ARGUMENT; st=umi_enterprise_market_data_cache_put(&s->cache,u); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)s->accepted_updates++; /* Use this fallback path when the earlier condition does not apply. */ else s->rejected_updates++; return st; }
