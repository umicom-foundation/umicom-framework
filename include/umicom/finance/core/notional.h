/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/notional.h
 *
 * PURPOSE:
 *   Represent positive notionals using UmiMoney.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_NOTIONAL_H
#define UMICOM_FINANCE_CORE_NOTIONAL_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiNotional { UmiMoney amount; } UmiNotional;
/* Initialize notional. */ UmiStatus umi_notional_init(UmiNotional *n,UmiMoney amount);
/* Validate notional. */ bool umi_notional_is_valid(const UmiNotional *n);
#ifdef __cplusplus
}
#endif

#endif
