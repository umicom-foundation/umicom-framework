/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/query_order.h
 *
 * PURPOSE:
 *   Describe deterministic query ordering independent of SQL dialect.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_QUERY_ORDER_H
#define UMICOM_DATA_ENTERPRISE_QUERY_ORDER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataQueryOrder {
    char order_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char field[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    bool descending;
    bool nulls_last;
} UmiDataQueryOrder;

/* Initialise a validated query order descriptor. */
UmiStatus umi_data_query_order_init(UmiDataQueryOrder *item, const char *order_id, const char *field, bool descending);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_query_order_validate(const UmiDataQueryOrder *item);

#ifdef __cplusplus
}
#endif
#endif
