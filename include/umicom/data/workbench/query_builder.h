/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/query_builder.h
 *
 * PURPOSE:
 *   Coordinate canonical backend-neutral query descriptors and generate a
 *   reviewable SQL preview without executing it or assuming a vendor driver.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_QUERY_BUILDER_H
#define UMICOM_DATA_WORKBENCH_QUERY_BUILDER_H

#include "umicom/data/enterprise/query_expression.h"
#include "umicom/data/enterprise/query_join.h"
#include "umicom/data/enterprise/query_order.h"
#include "umicom/data/enterprise/query_plan.h"
#include "umicom/data/enterprise/query_projection.h"
#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataQueryBuilder {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDataQueryPlan plan;
    UmiDataQueryProjection projections[UMI_DATA_WORKBENCH_MAX_BUILDER_ITEMS];
    UmiDataQueryExpression predicates[UMI_DATA_WORKBENCH_MAX_BUILDER_ITEMS];
    UmiDataQueryJoin joins[UMI_DATA_WORKBENCH_MAX_BUILDER_ITEMS];
    UmiDataQueryOrder orders[UMI_DATA_WORKBENCH_MAX_BUILDER_ITEMS];
    size_t projection_count;
    size_t predicate_count;
    size_t join_count;
    size_t order_count;
    uint64_t revision;
} UmiDataQueryBuilder;

UmiStatus umi_data_query_builder_init(
    UmiDataQueryBuilder *builder,
    const char *builder_id,
    const char *root_table);
UmiStatus umi_data_query_builder_add_projection(
    UmiDataQueryBuilder *builder,
    const UmiDataQueryProjection *projection);
UmiStatus umi_data_query_builder_add_predicate(
    UmiDataQueryBuilder *builder,
    const UmiDataQueryExpression *predicate);
UmiStatus umi_data_query_builder_add_join(
    UmiDataQueryBuilder *builder,
    const UmiDataQueryJoin *join);
UmiStatus umi_data_query_builder_add_order(
    UmiDataQueryBuilder *builder,
    const UmiDataQueryOrder *order);
UmiStatus umi_data_query_builder_set_limit(
    UmiDataQueryBuilder *builder,
    uint64_t row_limit);
UmiStatus umi_data_query_builder_sql_preview(
    const UmiDataQueryBuilder *builder,
    char *sql,
    size_t capacity);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_QUERY_BUILDER_H */
