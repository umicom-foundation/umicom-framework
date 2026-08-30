/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/notional.c
 *
 * PURPOSE:
 *   Implement positive notional validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/notional.h"

#include <string.h>
/* Initialize notional. */ UmiStatus umi_notional_init(UmiNotional *n,UmiMoney amount){if(n==NULL||amount.minor_units<=0||strlen(amount.currency.code)!=3U)return UMI_STATUS_INVALID_ARGUMENT;n->amount=amount;return UMI_STATUS_OK;}
/* Validate notional. */ bool umi_notional_is_valid(const UmiNotional *n){return n!=NULL&&n->amount.minor_units>0&&strlen(n->amount.currency.code)==3U;}
