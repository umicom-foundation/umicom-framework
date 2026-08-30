/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_query_builder.c
 *
 * PURPOSE:
 *   Verify canonical visual-query descriptors and review-only SQL generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/data/workbench/query_builder.h"

int main(void)
{
    UmiDataQueryBuilder builder;
    UmiDataQueryProjection projection;
    UmiDataQueryExpression predicate;
    UmiDataQueryOrder order;
    char sql[1024];
    assert(umi_data_query_builder_init(
               &builder, "builder-1", "orders") == UMI_STATUS_OK);
    assert(umi_data_query_projection_init(
               &projection, "p1", "order_id", "id") == UMI_STATUS_OK);
    assert(umi_data_query_expression_init(
               &predicate, "e1", "status", "=", "OPEN") == UMI_STATUS_OK);
    assert(umi_data_query_order_init(
               &order, "o1", "order_id", true) == UMI_STATUS_OK);
    assert(umi_data_query_builder_add_projection(&builder, &projection) ==
           UMI_STATUS_OK);
    assert(umi_data_query_builder_add_predicate(&builder, &predicate) ==
           UMI_STATUS_OK);
    assert(umi_data_query_builder_add_order(&builder, &order) == UMI_STATUS_OK);
    assert(umi_data_query_builder_set_limit(&builder, 100U) == UMI_STATUS_OK);
    assert(umi_data_query_builder_sql_preview(
               &builder, sql, sizeof(sql)) == UMI_STATUS_OK);
    assert(strstr(sql, "SELECT order_id AS id FROM orders") != NULL);
    assert(strstr(sql, "ORDER BY order_id DESC") != NULL);
    return 0;
}
