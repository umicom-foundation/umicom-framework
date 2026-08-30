/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_query_predicate.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the query predicate enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_predicate.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataQueryPredicate registry;
    UmiDataQueryExpression item;
    UmiDataQueryExpression found;
    umi_data_query_predicate_init(&registry);
    CHECK(umi_data_query_expression_init(&item,"e1","status","=","OPEN") == UMI_STATUS_OK);
    CHECK(umi_data_query_predicate_add(&registry, &item) == UMI_STATUS_OK);
    CHECK(umi_data_query_predicate_add(&registry, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_data_query_predicate_find(&registry, item.expression_id, &found) == UMI_STATUS_OK);
    CHECK(strcmp(found.expression_id, item.expression_id) == 0);
    CHECK(umi_data_query_predicate_count(&registry) == 1U);
    return 0;
}
