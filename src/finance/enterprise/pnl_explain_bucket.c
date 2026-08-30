/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/pnl_explain_bucket.c
 *
 * PURPOSE:
 *   Represent one named P&L explanation bucket.
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

#include "umicom/finance/enterprise/pnl_explain_bucket.h"

#include <string.h>
UmiStatus umi_enterprise_pnl_explain_bucket_init(UmiEnterprisePnlExplainBucket *b,const char *name,double pnl){UmiStatus s;if(b==NULL||!umi_quant_number_valid(pnl))return UMI_STATUS_INVALID_ARGUMENT;memset(b,0,sizeof *b);s=umi_quant_copy_text(b->name,sizeof b->name,name);if(s!=UMI_STATUS_OK)return s;b->pnl=pnl;return UMI_STATUS_OK;}
