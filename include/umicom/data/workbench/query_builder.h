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

/**
 * Represent the data query builder data shared with callers of this public contract.
 */
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

/**
 * Initialise data query builder from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_data_query_builder_init(
    UmiDataQueryBuilder *builder,
    const char *builder_id,
    const char *root_table);
/**
 * Provide the data query builder add projection operation used by this module and its
 * client applications.
 */
UmiStatus umi_data_query_builder_add_projection(
    UmiDataQueryBuilder *builder,
    const UmiDataQueryProjection *projection);
/**
 * Provide the data query builder add predicate operation used by this module and its
 * client applications.
 */
UmiStatus umi_data_query_builder_add_predicate(
    UmiDataQueryBuilder *builder,
    const UmiDataQueryExpression *predicate);
/**
 * Provide the data query builder add join operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_query_builder_add_join(
    UmiDataQueryBuilder *builder,
    const UmiDataQueryJoin *join);
/**
 * Provide the data query builder add order operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_query_builder_add_order(
    UmiDataQueryBuilder *builder,
    const UmiDataQueryOrder *order);
/**
 * Provide the data query builder set limit operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_query_builder_set_limit(
    UmiDataQueryBuilder *builder,
    uint64_t row_limit);
/**
 * Provide the data query builder sql preview operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_query_builder_sql_preview(
    const UmiDataQueryBuilder *builder,
    char *sql,
    size_t capacity);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_QUERY_BUILDER_H */
