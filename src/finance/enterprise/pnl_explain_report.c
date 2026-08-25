/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/pnl_explain_report.c
 *
 * PURPOSE:
 *   Aggregate named P&L explain buckets and reconciliation residual.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/pnl_explain_report.h"

#include <string.h>
UmiStatus umi_enterprise_pnl_explain_report_init(UmiEnterprisePnlExplainReport *r,const char *id,double actual){if(r==NULL||!umi_quant_number_valid(actual))return UMI_STATUS_INVALID_ARGUMENT;memset(r,0,sizeof *r);r->actual_pnl=actual;return umi_quant_copy_text(r->portfolio_id,sizeof r->portfolio_id,id);}
UmiStatus umi_enterprise_pnl_explain_report_add(UmiEnterprisePnlExplainReport *r,const UmiEnterprisePnlExplainBucket *b){size_t i;if(r==NULL||b==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<r->count;++i)if(strcmp(r->buckets[i].name,b->name)==0)return UMI_STATUS_ALREADY_EXISTS;if(r->count>=32U)return UMI_STATUS_CAPACITY_EXCEEDED;r->buckets[r->count++]=*b;return UMI_STATUS_OK;}
double umi_enterprise_pnl_explain_report_residual(const UmiEnterprisePnlExplainReport *r){size_t i;double explained=0.0;if(r==NULL)return 0.0;for(i=0U;i<r->count;++i)explained+=r->buckets[i].pnl;return r->actual_pnl-explained;}
