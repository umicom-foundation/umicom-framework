/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/types.h
 *
 * PURPOSE:
 *   Define canonical commodity, physical-delivery and logistics value types shared by
 *   TMS, Trader, Exchange and future commodity applications.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCE_COMMODITY_TYPES_H
#define UMICOM_FINANCE_COMMODITY_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_COMMODITY_ID_CAPACITY 64U
#define UMI_COMMODITY_NAME_CAPACITY 96U
#define UMI_COMMODITY_CODE_CAPACITY 24U
#define UMI_COMMODITY_MAX_ITEMS 64U
#define UMI_COMMODITY_MAX_LEGS 16U

typedef enum UmiCommodityKind {
    UMI_COMMODITY_KIND_UNKNOWN = 0,
    UMI_COMMODITY_KIND_ENERGY = 1,
    UMI_COMMODITY_KIND_METAL = 2,
    UMI_COMMODITY_KIND_AGRICULTURAL = 3,
    UMI_COMMODITY_KIND_EMISSIONS = 4,
    UMI_COMMODITY_KIND_OTHER = 5
} UmiCommodityKind;

typedef enum UmiCommodityDeliveryState {
    UMI_COMMODITY_DELIVERY_PLANNED = 0,
    UMI_COMMODITY_DELIVERY_NOMINATED = 1,
    UMI_COMMODITY_DELIVERY_IN_TRANSIT = 2,
    UMI_COMMODITY_DELIVERY_DELIVERED = 3,
    UMI_COMMODITY_DELIVERY_FAILED = 4,
    UMI_COMMODITY_DELIVERY_CANCELLED = 5
} UmiCommodityDeliveryState;

typedef enum UmiCommodityShipmentState {
    UMI_COMMODITY_SHIPMENT_PLANNED = 0,
    UMI_COMMODITY_SHIPMENT_LOADING = 1,
    UMI_COMMODITY_SHIPMENT_IN_TRANSIT = 2,
    UMI_COMMODITY_SHIPMENT_UNLOADING = 3,
    UMI_COMMODITY_SHIPMENT_COMPLETE = 4,
    UMI_COMMODITY_SHIPMENT_CANCELLED = 5
} UmiCommodityShipmentState;

typedef struct UmiCommodityId {
    char value[UMI_COMMODITY_ID_CAPACITY];
} UmiCommodityId;

typedef struct UmiCommodityQuantity {
    int64_t units;
    int32_t scale;
    char unit_code[UMI_COMMODITY_CODE_CAPACITY];
} UmiCommodityQuantity;

/* Copy text into fixed Framework buffers while preserving NUL termination. */
UmiStatus umi_commodity_copy_text(char *destination, size_t capacity, const char *source);

/* Return true when a fixed-buffer identifier contains a non-empty value. */
bool umi_commodity_text_valid(const char *text);

#ifdef __cplusplus
}
#endif
#endif
