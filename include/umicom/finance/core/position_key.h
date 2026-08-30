/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/position_key.h
 *
 * PURPOSE:
 *   Define canonical position aggregation dimensions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_POSITION_KEY_H
#define UMICOM_FINANCE_CORE_POSITION_KEY_H

#include "umicom/finance/core/product_id.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPositionKey { UmiFinancialId book_id; UmiFinancialId portfolio_id; UmiProductId product_id; } UmiPositionKey;
/* Initialize key. */ UmiStatus umi_position_key_init(UmiPositionKey *k,const char *book,const char *portfolio,const char *product);
/* Compare keys. */ bool umi_position_key_equal(const UmiPositionKey *a,const UmiPositionKey *b);
#ifdef __cplusplus
}
#endif

#endif
