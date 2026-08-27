/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/er_model.c
 *
 * PURPOSE:
 *   Implement deterministic entity/relation projection from schema metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/er_model.h"

#include <string.h>

static size_t entity_index(const UmiDataErModel *model, const char *table_id)
{
    size_t index;
    for (index = 0U; index < model->entity_count; ++index) {
        if (strcmp(model->entities[index].table_id, table_id) == 0) return index;
    }
    return model->entity_count;
}

void umi_data_er_model_init(UmiDataErModel *model)
{
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->struct_size = (uint32_t)sizeof(*model);
    model->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    model->revision = 1U;
}

UmiStatus umi_data_er_model_build(
    UmiDataErModel *model,
    const UmiDataSchemaSnapshot *schema)
{
    size_t index;
    if (model == NULL || schema == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_data_er_model_init(model);
    if (schema->count > UMI_DATA_ENTERPRISE_MAX_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < schema->count; ++index) {
        UmiDataErEntity *entity = &model->entities[index];
        UmiStatus status = umi_data_workbench_copy_text(
            entity->table_id, sizeof(entity->table_id),
            schema->items[index].table_id);
        if (status == UMI_STATUS_OK) status = umi_data_workbench_copy_text(
            entity->label, sizeof(entity->label), schema->items[index].name);
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

UmiStatus umi_data_er_model_add_relation(
    UmiDataErModel *model,
    const UmiDataSchemaForeignKey *foreign_key)
{
    size_t source;
    size_t target;
    if (model == NULL ||
        umi_data_schema_foreign_key_validate(foreign_key) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (model->relation_count >= UMI_DATA_WORKBENCH_MAX_RELATIONS) {
        model->truncated = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    source = entity_index(model, foreign_key->source_table);
    target = entity_index(model, foreign_key->target_table);
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

UmiStatus umi_data_er_model_select(
    UmiDataErModel *model,
    size_t entity_index_value)
{
    size_t index;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (entity_index_value >= model->entity_count) return UMI_STATUS_NOT_FOUND;
    for (index = 0U; index < model->entity_count; ++index) {
        model->entities[index].selected = index == entity_index_value;
    }
    model->selected_entity = entity_index_value;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiDataErEntity *umi_data_er_model_entity_at(
    const UmiDataErModel *model,
    size_t entity_index_value)
{
    return model != NULL && entity_index_value < model->entity_count
        ? &model->entities[entity_index_value] : NULL;
}
