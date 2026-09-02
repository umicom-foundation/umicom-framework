/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/money.c
 *
 * PURPOSE:
 *   Perform safe same-currency money addition and subtraction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The implementation is deliberately small and deterministic so callers can test money behaviour without starting a complete product.
 */

#include <string.h>
#include "umicom/finance/money.h"
/* Provide the same money shape operation used by this module and its client applications. */
static int same_money_shape(const UmiMoney *a,const UmiMoney *b){return a!=NULL && b!=NULL && a->scale==b->scale && strcmp(a->currency.code,b->currency.code)==0;}
/* Add money only after its inputs and available capacity have been checked. */
UmiStatus umi_money_add(const UmiMoney *left,const UmiMoney *right,UmiMoney *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL||!same_money_shape(left,right))return UMI_STATUS_INVALID_ARGUMENT;*out=*left;out->minor_units=left->minor_units+right->minor_units;return UMI_STATUS_OK;}
/* Provide the money subtract operation used by this module and its client applications. */
UmiStatus umi_money_subtract(const UmiMoney *left,const UmiMoney *right,UmiMoney *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL||!same_money_shape(left,right))return UMI_STATUS_INVALID_ARGUMENT;*out=*left;out->minor_units=left->minor_units-right->minor_units;return UMI_STATUS_OK;}
