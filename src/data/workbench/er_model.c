/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/er_model.c
 *
 * PURPOSE:
 *   Implement deterministic entity/relation projection from schema metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/er_model.h"

#include <string.h>

/* Provide the entity index operation used by this module and its client applications. */
static size_t entity_index(const UmiDataErModel *model, const char *table_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->entity_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->entities[index].table_id, table_id) == 0) return index;
    }
    return model->entity_count;
}

/*
 * Initialise data er model from caller-provided values so later operations receive a known
 * state.
 */
void umi_data_er_model_init(UmiDataErModel *model)
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
}

/*
 * Provide the data er model build operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_er_model_build(
    UmiDataErModel *model,
    const UmiDataSchemaSnapshot *schema)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || schema == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_data_er_model_init(model);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (schema->count > UMI_DATA_ENTERPRISE_MAX_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < schema->count; ++index) {
        UmiDataErEntity *entity = &model->entities[index];
        UmiStatus status = umi_data_workbench_copy_text(
            entity->table_id, sizeof(entity->table_id),
            schema->items[index].table_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = umi_data_workbench_copy_text(
            entity->label, sizeof(entity->label), schema->items[index].name);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        entity->column_count = schema->items[index].column_count;
        entity->x = (int32_t)((index % 4U) * 280U);
        entity->y = (int32_t)((index / 4U) * 180U);
        entity->selected = index == 0U;
    }
    model->entity_count = schema->count;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the data er model add relation operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_er_model_add_relation(
    UmiDataErModel *model,
    const UmiDataSchemaForeignKey *foreign_key)
{
    size_t source;
    size_t target;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL ||
        umi_data_schema_foreign_key_validate(foreign_key) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->relation_count >= UMI_DATA_WORKBENCH_MAX_RELATIONS) {
        model->truncated = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    source = entity_index(model, foreign_key->source_table);
    target = entity_index(model, foreign_key->target_table);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (source >= model->entity_count || target >= model->entity_count) {
        return UMI_STATUS_NOT_FOUND;
    }
    model->relations[model->relation_count].foreign_key = *foreign_key;
    model->relations[model->relation_count].source_entity = source;
    model->relations[model->relation_count].target_entity = target;
    model->relation_count += 1U;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the data er model select operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_er_model_select(
    UmiDataErModel *model,
    size_t entity_index_value)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (entity_index_value >= model->entity_count) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->entity_count; ++index) {
        model->entities[index].selected = index == entity_index_value;
    }
    model->selected_entity = entity_index_value;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find data er model entity while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiDataErEntity *umi_data_er_model_entity_at(
    const UmiDataErModel *model,
    size_t entity_index_value)
{
    return model != NULL && entity_index_value < model->entity_count
        ? &model->entities[entity_index_value] : NULL;
}
