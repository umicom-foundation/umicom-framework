/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/product_id.h
 *
 * PURPOSE:
 *   Provide a strongly typed product id wrapper over the existing financial identifier.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_PRODUCT_ID_H
#define UMICOM_FINANCE_CORE_PRODUCT_ID_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductId { UmiFinancialId id; } UmiProductId;
/* Assign identifier. */ UmiStatus umi_product_id_set(UmiProductId *id,const char *value);
/* Validate identifier. */ bool umi_product_id_is_valid(const UmiProductId *id);
#ifdef __cplusplus
}
#endif

#endif
