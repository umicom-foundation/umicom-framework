/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/business_date_adjustment.c
 *
 * PURPOSE:
 *   Implement reusable date-adjustment policy metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/business_date_adjustment.h"

/* Initialize conservative defaults. */ void umi_business_date_adjustment_default(UmiBusinessDateAdjustment *p){if(p!=NULL){p->flags=0xFFFFFFFFU;p->limit=100U;p->strict=true;}}
/* Evaluate a bounded numeric request. */ bool umi_business_date_adjustment_allows(const UmiBusinessDateAdjustment *p,uint32_t value,uint32_t required_flags){return p!=NULL&&value<=p->limit&&(p->flags&required_flags)==required_flags;}
