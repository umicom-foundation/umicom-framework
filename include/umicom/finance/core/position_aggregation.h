/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/position_aggregation.h
 *
 * PURPOSE:
 *   Aggregate position quantities over canonical dimensions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_POSITION_AGGREGATION_H
#define UMICOM_FINANCE_CORE_POSITION_AGGREGATION_H

#include "umicom/finance/core/position_book.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Sum all positions. */ UmiStatus
umi_position_aggregation_total(const UmiFinancialPositionBook *book, double *out);
/* Sum positions for one product. */ UmiStatus
umi_position_aggregation_product(const UmiFinancialPositionBook *book, const char *product,
                                 double *out);
#ifdef __cplusplus
}
#endif

#endif
