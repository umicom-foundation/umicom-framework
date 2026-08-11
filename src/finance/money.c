/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/money.c
 *
 * PURPOSE:
 *   Perform safe same-currency money addition and subtraction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The implementation is deliberately small and deterministic so callers can test money behaviour without starting a complete product.
 */

#include <string.h>
#include "umicom/finance/money.h"
static int same_money_shape(const UmiMoney *a,const UmiMoney *b){return a!=NULL && b!=NULL && a->scale==b->scale && strcmp(a->currency.code,b->currency.code)==0;}
UmiStatus umi_money_add(const UmiMoney *left,const UmiMoney *right,UmiMoney *out){if(out==NULL||!same_money_shape(left,right))return UMI_STATUS_INVALID_ARGUMENT;*out=*left;out->minor_units=left->minor_units+right->minor_units;return UMI_STATUS_OK;}
UmiStatus umi_money_subtract(const UmiMoney *left,const UmiMoney *right,UmiMoney *out){if(out==NULL||!same_money_shape(left,right))return UMI_STATUS_INVALID_ARGUMENT;*out=*left;out->minor_units=left->minor_units-right->minor_units;return UMI_STATUS_OK;}
