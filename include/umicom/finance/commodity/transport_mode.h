/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/transport_mode.h
 *
 * PURPOSE:
 *   Define a reusable physical transport mode such as vessel, pipeline, rail or truck.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_TRANSPORT_MODE_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_TRANSPORT_MODE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityTransportMode {
    char code[UMI_COMMODITY_CODE_CAPACITY];
    char name[UMI_COMMODITY_NAME_CAPACITY];
    bool supports_bulk;
    bool active;
} UmiCommodityTransportMode;

/* Initialise a bounded transport mode record for reusable Framework workflows. */
UmiStatus umi_commodity_transport_mode_init(UmiCommodityTransportMode *value, const char *code, const char *name, bool supports_bulk);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_transport_mode_valid(const UmiCommodityTransportMode *value);

#ifdef __cplusplus
}
#endif

#endif
