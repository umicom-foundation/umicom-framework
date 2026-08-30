/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/market_data_quality.c
 *
 * PURPOSE:
 *   Score completeness and anomaly evidence for a market-data cut.
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

#include "umicom/finance/enterprise/market_data_quality.h"

UmiStatus umi_enterprise_market_data_quality_init(UmiEnterpriseMarketDataQuality *quality,size_t expected){ if(quality==NULL||expected==0U)return UMI_STATUS_INVALID_ARGUMENT; quality->expected=expected; quality->observed=0U; quality->invalid=0U; quality->outliers=0U; return UMI_STATUS_OK; }
double umi_enterprise_market_data_quality_score(const UmiEnterpriseMarketDataQuality *q){ double good; if(q==NULL||q->expected==0U)return 0.0; good=(double)q->observed-(double)q->invalid-(double)q->outliers; if(good<0.0)good=0.0; if(good>(double)q->expected)good=(double)q->expected; return good/(double)q->expected; }
