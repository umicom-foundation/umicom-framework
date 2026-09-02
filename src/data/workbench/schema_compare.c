/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/schema_compare.c
 *
 * PURPOSE:
 *   Project aggregate canonical schema diff results into per-table changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/schema_compare.h"

#include <string.h>

/* Provide the table equal operation used by this module and its client applications. */
static int table_equal(
    const UmiDataSchemaTable *before,
    const UmiDataSchemaTable *after)
{
    size_t index;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(before->name, after->name) != 0 ||
        before->column_count != after->column_count ||
        before->temporary != after->temporary) {
        return 0;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < before->column_count; ++index) {
        const UmiDataSchemaColumn *left = &before->columns[index];
        const UmiDataSchemaColumn *right = &after->columns[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(left->column_id, right->column_id) != 0 ||
            strcmp(left->name, right->name) != 0 || left->kind != right->kind ||
            left->ordinal != right->ordinal ||
            left->nullable != right->nullable ||
            left->generated != right->generated) {
            return 0;
        }
    }
    return 1;
}

/* Provide the add change operation used by this module and its client applications. */
static UmiStatus add_change(
    UmiDataSchemaCompareModel *model,
    const UmiDataSchemaTable *before,
    const UmiDataSchemaTable *after,
    UmiDataSchemaChangeKind kind)
{
    UmiDataSchemaTableChange *change;
    const char *identifier = before != NULL ? before->table_id : after->table_id;
    UmiStatus status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->change_count >= UMI_DATA_WORKBENCH_MAX_SCHEMA_CHANGES) {
        model->truncated = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    change = &model->changes[model->change_count];
    (void)memset(change, 0, sizeof(*change));
    status = umi_data_workbench_copy_text(
        change->table_id, sizeof(change->table_id), identifier);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && before != NULL) {
        status = umi_data_workbench_copy_text(
            change->before_name, sizeof(change->before_name), before->name);
        change->before_columns = before->column_count;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && after != NULL) {
        status = umi_data_workbench_copy_text(
            change->after_name, sizeof(change->after_name), after->name);
        change->after_columns = after->column_count;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    change->kind = kind;
    change->selected = model->change_count == 0U;
    model->change_count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Initialise data schema compare model from caller-provided values so later operations
 * receive a known state.
 */
void umi_data_schema_compare_model_init(UmiDataSchemaCompareModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->struct_size = (uint32_t)sizeof(*model);
    model->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    model->revision = 1U;
    umi_data_schema_snapshot_init(&model->before);
    umi_data_schema_snapshot_init(&model->after);
}

/*
 * Provide the data schema compare model build operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_schema_compare_model_build(
    UmiDataSchemaCompareModel *model,
    const UmiDataSchemaSnapshot *before,
    const UmiDataSchemaSnapshot *after)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || before == NULL || after == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_data_schema_compare_model_init(model);
    model->before = *before;
    model->after = *after;
    status = umi_data_schema_diff_compute(before, after, &model->summary);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < before->count; ++index) {
        UmiDataSchemaTable current;
        status = umi_data_schema_snapshot_find(
            after, before->items[index].table_id, &current);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) {
            status = add_change(model, &before->items[index], NULL,
                                UMI_DATA_SCHEMA_REMOVED);
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) {
            status = add_change(
                model, &before->items[index], &current,
                table_equal(&before->items[index], &current)
                    ? UMI_DATA_SCHEMA_UNCHANGED : UMI_DATA_SCHEMA_CHANGED);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < after->count; ++index) {
        UmiDataSchemaTable previous;
        status = umi_data_schema_snapshot_find(
            before, after->items[index].table_id, &previous);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) {
            status = add_change(model, NULL, &after->items[index],
                                UMI_DATA_SCHEMA_ADDED);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    model->ready = 1;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the data schema compare model select operation used by this module and its
 * client applications.
 */
UmiStatus umi_data_schema_compare_model_select(
    UmiDataSchemaCompareModel *model,
    size_t change_index)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !model->ready) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (change_index >= model->change_count) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->change_count; ++index) {
        model->changes[index].selected = index == change_index;
    }
    model->selected_index = change_index;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find data schema compare model change while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDataSchemaTableChange *umi_data_schema_compare_model_change_at(
    const UmiDataSchemaCompareModel *model,
    size_t change_index)
{
    return model != NULL && change_index < model->change_count
        ? &model->changes[change_index] : NULL;
}
