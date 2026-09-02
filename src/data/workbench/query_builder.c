/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/query_builder.c
 *
 * PURPOSE:
 *   Compose canonical query descriptors and render a review-only SQL preview.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/query_builder.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef struct SqlWriter {
    char *text;
    size_t capacity;
    size_t length;
} SqlWriter;

/* Provide the append operation used by this module and its client applications. */
static UmiStatus append(SqlWriter *writer, const char *format, ...)
{
    int written;
    va_list arguments;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (writer->length >= writer->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    va_start(arguments, format);
    written = vsnprintf(writer->text + writer->length,
                        writer->capacity - writer->length,
                        format, arguments);
    va_end(arguments);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= writer->capacity - writer->length) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    writer->length += (size_t)written;
    return UMI_STATUS_OK;
}

/* Provide the projection exists operation used by this module and its client applications. */
static int projection_exists(
    const UmiDataQueryBuilder *builder,
    const char *identifier)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < builder->projection_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(builder->projections[index].projection_id, identifier) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
 * Initialise data query builder from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_data_query_builder_init(
    UmiDataQueryBuilder *builder,
    const char *builder_id,
    const char *root_table)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (builder == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(builder, 0, sizeof(*builder));
    builder->struct_size = (uint32_t)sizeof(*builder);
    builder->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    status = umi_data_query_plan_init(&builder->plan, builder_id, root_table);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) builder->revision = 1U;
    return status;
}

/*
 * Provide the data query builder add projection operation used by this module and its
 * client applications.
 */
UmiStatus umi_data_query_builder_add_projection(
    UmiDataQueryBuilder *builder,
    const UmiDataQueryProjection *projection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (builder == NULL ||
        umi_data_query_projection_validate(projection) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (projection_exists(builder, projection->projection_id)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Create this optional product surface only when its build option is enabled. */
    if (builder->projection_count >= UMI_DATA_WORKBENCH_MAX_BUILDER_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    builder->projections[builder->projection_count++] = *projection;
    builder->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the data query builder add predicate operation used by this module and its
 * client applications.
 */
UmiStatus umi_data_query_builder_add_predicate(
    UmiDataQueryBuilder *builder,
    const UmiDataQueryExpression *predicate)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (builder == NULL ||
        umi_data_query_expression_validate(predicate) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < builder->predicate_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(builder->predicates[index].expression_id,
                   predicate->expression_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Create this optional product surface only when its build option is enabled. */
    if (builder->predicate_count >= UMI_DATA_WORKBENCH_MAX_BUILDER_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    builder->predicates[builder->predicate_count++] = *predicate;
    builder->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the data query builder add join operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_query_builder_add_join(
    UmiDataQueryBuilder *builder,
    const UmiDataQueryJoin *join)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (builder == NULL || umi_data_query_join_validate(join) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < builder->join_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(builder->joins[index].join_id, join->join_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Create this optional product surface only when its build option is enabled. */
    if (builder->join_count >= UMI_DATA_WORKBENCH_MAX_BUILDER_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    builder->joins[builder->join_count++] = *join;
    builder->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the data query builder add order operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_query_builder_add_order(
    UmiDataQueryBuilder *builder,
    const UmiDataQueryOrder *order)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (builder == NULL ||
        umi_data_query_order_validate(order) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < builder->order_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(builder->orders[index].order_id, order->order_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Create this optional product surface only when its build option is enabled. */
    if (builder->order_count >= UMI_DATA_WORKBENCH_MAX_BUILDER_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    builder->orders[builder->order_count++] = *order;
    builder->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the data query builder set limit operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_query_builder_set_limit(
    UmiDataQueryBuilder *builder,
    uint64_t row_limit)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (builder == NULL || row_limit == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_data_query_plan_shape(
        &builder->plan, builder->predicate_count, builder->projection_count,
        builder->join_count, builder->order_count, row_limit);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) builder->revision += 1U;
    return status;
}

/*
 * Provide the data query builder sql preview operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_query_builder_sql_preview(
    const UmiDataQueryBuilder *builder,
    char *sql,
    size_t capacity)
{
    SqlWriter writer = {sql, capacity, 0U};
    UmiStatus status;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (builder == NULL || sql == NULL || capacity == 0U ||
        umi_data_query_plan_validate(&builder->plan) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    sql[0] = '\0';
    status = append(&writer, "SELECT ");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (builder->projection_count == 0U) {
        status = append(&writer, "*");
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK && index < builder->projection_count;
         ++index) {
        const UmiDataQueryProjection *projection = &builder->projections[index];
        status = append(&writer, "%s%s%s%s",
                        index > 0U ? ", " : "", projection->field,
                        projection->alias[0] != '\0' ? " AS " : "",
                        projection->alias);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = append(&writer, " FROM %s", builder->plan.root_table);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK && index < builder->join_count;
         ++index) {
        const UmiDataQueryJoin *join = &builder->joins[index];
        status = append(&writer, " %sJOIN %s ON %s",
                        join->outer_join ? "LEFT " : "",
                        join->right_table, join->condition);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK && index < builder->predicate_count;
         ++index) {
        const UmiDataQueryExpression *predicate = &builder->predicates[index];
        status = append(&writer, "%s%s %s %s",
                        index == 0U ? " WHERE " : " AND ",
                        predicate->field, predicate->operation,
                        predicate->parameterized ? "?" : predicate->value);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK && index < builder->order_count;
         ++index) {
        const UmiDataQueryOrder *order = &builder->orders[index];
        status = append(&writer, "%s%s %s%s",
                        index == 0U ? " ORDER BY " : ", ",
                        order->field, order->descending ? "DESC" : "ASC",
                        order->nulls_last ? " NULLS LAST" : "");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && builder->plan.row_limit > 0U) {
        status = append(&writer, " LIMIT %llu",
                        (unsigned long long)builder->plan.row_limit);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append(&writer, ";");
    return status;
}
