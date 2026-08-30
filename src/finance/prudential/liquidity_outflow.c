/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/liquidity_outflow.c
 *
 * PURPOSE:
 *   Calculate stressed liquidity outflow after applying run-off rate.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_outflow.h"

#include <string.h>
UmiStatus umi_pru_liquidity_outflow_calculate(UmiPrudentialLiquidityOutflow *result,const char *item_id,double base_amount,double factor) { UmiStatus s; double stressed; if(result==NULL||item_id==NULL||factor<0.0||factor>1.0)return UMI_STATUS_INVALID_ARGUMENT; s=umi_pru_weighted_amount(base_amount,factor,&stressed); if(s!=UMI_STATUS_OK)return s; memset(result,0,sizeof *result); s=umi_pru_copy_text(result->item_id,sizeof result->item_id,item_id); if(s!=UMI_STATUS_OK)return s; result->base_amount=base_amount; result->factor=factor; result->stressed_amount=stressed; return UMI_STATUS_OK; }
