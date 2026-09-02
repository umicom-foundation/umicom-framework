/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/portfolio_position.c
 *
 * PURPOSE:
 *   Represent a quantity and valuation contribution for one portfolio product.
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

#include "umicom/finance/enterprise/portfolio_position.h"

#include <string.h>
/*
 * Initialise enterprise portfolio position from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_enterprise_portfolio_position_init(UmiEnterprisePortfolioPosition *p,const char *id,double q,double unit){ UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||!umi_quant_number_valid(q)||!umi_quant_number_valid(unit))return UMI_STATUS_INVALID_ARGUMENT; memset(p,0,sizeof *p); s=umi_quant_copy_text(p->product_id.value,sizeof p->product_id.value,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; p->quantity=q;p->unit_value=unit;p->market_value=q*unit; return umi_quant_number_valid(p->market_value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT; }
