/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/portfolio_snapshot.c
 *
 * PURPOSE:
 *   Capture a coherent bounded set of portfolio positions at one time.
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

#include "umicom/finance/enterprise/portfolio_snapshot.h"

#include <string.h>
/*
 * Initialise enterprise portfolio snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_enterprise_portfolio_snapshot_init(UmiEnterprisePortfolioSnapshot *s,const char *id,int64_t asof){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||asof<0)return UMI_STATUS_INVALID_ARGUMENT; memset(s,0,sizeof *s); s->as_of_ms=asof; return umi_quant_copy_text(s->portfolio_id,sizeof s->portfolio_id,id); }
/*
 * Add enterprise portfolio snapshot only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_enterprise_portfolio_snapshot_add(UmiEnterprisePortfolioSnapshot *s,const UmiEnterprisePortfolioPosition *p){ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||p==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(s->positions[i].product_id.value,p->product_id.value)==0)return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_ENTERPRISE_MAX_POSITIONS)return UMI_STATUS_CAPACITY_EXCEEDED; s->positions[s->count++]=*p;return UMI_STATUS_OK; }
