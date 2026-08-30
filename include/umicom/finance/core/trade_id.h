/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/trade_id.h
 *
 * PURPOSE:
 *   Provide a strongly typed trade id wrapper over the existing financial identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_TRADE_ID_H
#define UMICOM_FINANCE_CORE_TRADE_ID_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradeId { UmiFinancialId id; } UmiTradeId;
/* Assign identifier. */ UmiStatus umi_trade_id_set(UmiTradeId *id,const char *value);
/* Validate identifier. */ bool umi_trade_id_is_valid(const UmiTradeId *id);
#ifdef __cplusplus
}
#endif

#endif
