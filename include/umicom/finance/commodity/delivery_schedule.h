/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/delivery_schedule.h
 *
 * PURPOSE:
 *   Define an ordered bounded schedule of physical delivery obligations.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_DELIVERY_SCHEDULE_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_DELIVERY_SCHEDULE_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"
#include "umicom/finance/commodity/delivery_obligation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityDeliverySchedule {
    UmiCommodityId contract_id;
    UmiCommodityDeliveryObligation obligations[UMI_COMMODITY_MAX_ITEMS];
    size_t count;
} UmiCommodityDeliverySchedule;

/* Initialise an empty delivery schedule for a contract. */
UmiStatus umi_commodity_delivery_schedule_init(UmiCommodityDeliverySchedule *value, const char *contract_id);

/* Add an obligation while enforcing non-decreasing due times. */
UmiStatus umi_commodity_delivery_schedule_add(UmiCommodityDeliverySchedule *value, const UmiCommodityDeliveryObligation *obligation);

#ifdef __cplusplus
}
#endif

#endif
