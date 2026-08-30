/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_service.c
 *
 * PURPOSE:
 *   Aggregate market-data readiness and valuation-grid capacity into service health.
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

#include "umicom/finance/enterprise/valuation_service.h"

#include <string.h>
UmiStatus umi_enterprise_valuation_service_init(UmiEnterpriseValuationService *s,int64_t freshness){ UmiStatus st; if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(s,0,sizeof *s); st=umi_enterprise_market_data_service_init(&s->market_data,freshness); if(st!=UMI_STATUS_OK)return st; umi_enterprise_valuation_grid_init(&s->grid); return UMI_STATUS_OK; }
int umi_enterprise_valuation_service_ready(const UmiEnterpriseValuationService *s){ if(s==NULL)return 0; return (s->grid.workers.count>0U&&s->market_data.accepted_updates>0U)?1:0; }
