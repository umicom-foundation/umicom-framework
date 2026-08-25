/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/query_predicate.h
 *
 * PURPOSE:
 *   Collect unique query expressions into a reusable predicate set.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_QUERY_PREDICATE_H
#define UMICOM_DATA_ENTERPRISE_QUERY_PREDICATE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/query_expression.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataQueryPredicate {
    UmiDataQueryExpression items[UMI_DATA_ENTERPRISE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiDataQueryPredicate;

/* Reset the bounded query predicate registry. */
void umi_data_query_predicate_init(UmiDataQueryPredicate *registry);
/* Add a unique item by stable identifier. */
UmiStatus umi_data_query_predicate_add(UmiDataQueryPredicate *registry, const UmiDataQueryExpression *item);
/* Find a registry item by stable identifier. */
UmiStatus umi_data_query_predicate_find(const UmiDataQueryPredicate *registry, const char *id, UmiDataQueryExpression *out_item);
/* Return the number of registered items. */
size_t umi_data_query_predicate_count(const UmiDataQueryPredicate *registry);

#ifdef __cplusplus
}
#endif
#endif
