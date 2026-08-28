/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/product_id.c
 *
 * PURPOSE:
 *   Implement typed product id assignment.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/product_id.h"

/* Assign identifier. */ UmiStatus umi_product_id_set(UmiProductId *id,const char *value){if(id==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_financial_id_assign(&id->id,value);}
/* Validate identifier. */ bool umi_product_id_is_valid(const UmiProductId *id){return id!=NULL&&umi_financial_id_is_valid(&id->id);}
