/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/market_data_entitlement.c
 *
 * PURPOSE:
 *   Enforce topic-prefix read entitlements independently from feed vendors.
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

#include "umicom/finance/enterprise/market_data_entitlement.h"

#include <string.h>
UmiStatus umi_enterprise_market_data_entitlement_init(UmiEnterpriseMarketDataEntitlement *e,const char *principal,const char *prefix,int can_read){ UmiStatus s; if(e==NULL||(can_read!=0&&can_read!=1))return UMI_STATUS_INVALID_ARGUMENT; memset(e,0,sizeof *e); s=umi_quant_copy_text(e->principal_id,sizeof e->principal_id,principal); if(s!=UMI_STATUS_OK)return s; s=umi_quant_copy_text(e->topic_prefix,sizeof e->topic_prefix,prefix); if(s==UMI_STATUS_OK)e->can_read=can_read; return s; }
int umi_enterprise_market_data_entitlement_allows(const UmiEnterpriseMarketDataEntitlement *e,const char *topic){ size_t n; if(e==NULL||topic==NULL||e->can_read==0)return 0; n=strlen(e->topic_prefix); return strncmp(topic,e->topic_prefix,n)==0 ? 1:0; }
