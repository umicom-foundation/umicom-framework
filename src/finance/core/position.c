/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/position.c
 *
 * PURPOSE:
 *   Implement position accumulation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/position.h"

#include <math.h>
/* Initialize position. */ UmiStatus umi_position_init(UmiPosition *p,const UmiPositionKey *k,UmiQuantity q,UmiFinancialDate d){if(p==NULL||k==NULL||!isfinite(q.value)||!umi_financial_date_is_valid(d))return UMI_STATUS_INVALID_ARGUMENT;p->key=*k;p->quantity=q;p->as_of_date=d;return UMI_STATUS_OK;}
/* Add quantity. */ UmiStatus umi_position_add(UmiPosition *p,UmiQuantity delta){if(p==NULL||!isfinite(delta.value))return UMI_STATUS_INVALID_ARGUMENT;p->quantity.value+=delta.value;return isfinite(p->quantity.value)?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED;}
/* Validate position. */ bool umi_position_is_valid(const UmiPosition *p){return p!=NULL&&isfinite(p->quantity.value)&&umi_financial_date_is_valid(p->as_of_date);}
