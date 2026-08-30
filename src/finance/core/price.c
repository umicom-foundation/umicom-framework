/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/price.c
 *
 * PURPOSE:
 *   Implement financial price validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/price.h"

#include <math.h>
/* Initialize price. */ UmiStatus umi_price_init(UmiFinancialPrice *p,double value,uint8_t scale){if(p==NULL||!isfinite(value)||value<0.0||scale>12U)return UMI_STATUS_INVALID_ARGUMENT;p->value=value;p->scale=scale;return UMI_STATUS_OK;}
/* Validate price. */ bool umi_price_is_valid(const UmiFinancialPrice *p){return p!=NULL&&isfinite(p->value)&&p->value>=0.0&&p->scale<=12U;}
