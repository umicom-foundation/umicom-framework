/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/stable_funding_item.c
 *
 * PURPOSE:
 *   Calculate available stable funding from balance and ASF factor.
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

#include "umicom/finance/prudential/stable_funding_item.h"

#include <string.h>
/*
 * Provide the pru stable funding item calculate operation used by this module and its
 * client applications.
 */
UmiStatus umi_pru_stable_funding_item_calculate(UmiPrudentialStableFundingItem *result,const char *item_id,double base_amount,double factor) { UmiStatus s; double stressed; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(result==NULL||item_id==NULL||factor<0.0||factor>1.0)return UMI_STATUS_INVALID_ARGUMENT; s=umi_pru_weighted_amount(base_amount,factor,&stressed); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; memset(result,0,sizeof *result); s=umi_pru_copy_text(result->item_id,sizeof result->item_id,item_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; result->base_amount=base_amount; result->factor=factor; result->stressed_amount=stressed; return UMI_STATUS_OK; }
