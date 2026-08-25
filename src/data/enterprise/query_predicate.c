/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/query_predicate.c
 *
 * PURPOSE:
 *   Collect unique query expressions into a reusable predicate set.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_predicate.h"
#include <string.h>

/* Registry initialisation is deterministic and allocation-free. */
void umi_data_query_predicate_init(UmiDataQueryPredicate *registry) { if (registry != NULL) (void)memset(registry, 0, sizeof(*registry)); }

/* Duplicate identifiers are rejected to keep mapping semantics unambiguous. */
UmiStatus umi_data_query_predicate_add(UmiDataQueryPredicate *registry, const UmiDataQueryExpression *item) {
    size_t i;
    if (registry == NULL || item == NULL || item->expression_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < registry->count; ++i) if (strcmp(registry->items[i].expression_id, item->expression_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count >= UMI_DATA_ENTERPRISE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *item; registry->revision++;
    return UMI_STATUS_OK;
}

/* Lookup copies a snapshot so callers never borrow internal registry storage. */
UmiStatus umi_data_query_predicate_find(const UmiDataQueryPredicate *registry, const char *id, UmiDataQueryExpression *out_item) {
    size_t i;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < registry->count; ++i) if (strcmp(registry->items[i].expression_id, id) == 0) { *out_item = registry->items[i]; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}

/* Count exposes registry size without exposing mutable storage. */
size_t umi_data_query_predicate_count(const UmiDataQueryPredicate *registry) { return registry != NULL ? registry->count : 0U; }
