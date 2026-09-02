/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/commodity_descriptor.h
 *
 * PURPOSE:
 *   Define reference-data metadata for a physical or financially settled commodity.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_COMMODITY_DESCRIPTOR_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_COMMODITY_DESCRIPTOR_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity descriptor data shared with callers of this public contract.
 */
typedef struct UmiCommodityDescriptor {
    UmiCommodityId id;
    char name[UMI_COMMODITY_NAME_CAPACITY];
    char code[UMI_COMMODITY_CODE_CAPACITY];
    UmiCommodityKind kind;
    UmiCurrency settlement_currency;
    bool physical_delivery;
    bool active;
} UmiCommodityDescriptor;

/* Initialise a bounded commodity descriptor record for reusable Framework workflows. */
UmiStatus umi_commodity_commodity_descriptor_init(UmiCommodityDescriptor *value, const char *id, const char *name, const char *code, UmiCommodityKind kind, const UmiCurrency *currency, bool physical_delivery);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_commodity_descriptor_valid(const UmiCommodityDescriptor *value);

#ifdef __cplusplus
}
#endif

#endif
