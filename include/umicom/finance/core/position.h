/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/position.h
 *
 * PURPOSE:
 *   Represent signed product positions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_POSITION_H
#define UMICOM_FINANCE_CORE_POSITION_H

#include "umicom/finance/core/position_key.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPosition { UmiPositionKey key; UmiQuantity quantity; UmiFinancialDate as_of_date; } UmiPosition;
/* Initialize position. */ UmiStatus umi_position_init(UmiPosition *p,const UmiPositionKey *k,UmiQuantity q,UmiFinancialDate d);
/* Add quantity. */ UmiStatus umi_position_add(UmiPosition *p,UmiQuantity delta);
/* Validate position. */ bool umi_position_is_valid(const UmiPosition *p);
#ifdef __cplusplus
}
#endif

#endif
