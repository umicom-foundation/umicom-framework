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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/pnl_explain_report.h"

#include <string.h>
/*
 * Initialise enterprise pnl explain report from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_enterprise_pnl_explain_report_init(UmiEnterprisePnlExplainReport *r,const char *id,double actual){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||!umi_quant_number_valid(actual))return UMI_STATUS_INVALID_ARGUMENT;memset(r,0,sizeof *r);r->actual_pnl=actual;return umi_quant_copy_text(r->portfolio_id,sizeof r->portfolio_id,id);}
/*
 * Add enterprise pnl explain report only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_enterprise_pnl_explain_report_add(UmiEnterprisePnlExplainReport *r,const UmiEnterprisePnlExplainBucket *b){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||b==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(r->buckets[i].name,b->name)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=32U)return UMI_STATUS_CAPACITY_EXCEEDED;r->buckets[r->count++]=*b;return UMI_STATUS_OK;}
/*
 * Provide the enterprise pnl explain report residual operation used by this module and its
 * client applications.
 */
double umi_enterprise_pnl_explain_report_residual(const UmiEnterprisePnlExplainReport *r){size_t i;double explained=0.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return 0.0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)explained+=r->buckets[i].pnl;return r->actual_pnl-explained;}
