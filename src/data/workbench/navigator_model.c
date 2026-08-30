/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/navigator_model.c
 *
 * PURPOSE:
 *   Project canonical connection and schema catalogues into a searchable tree.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/navigator_model.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int contains_case_insensitive(const char *text, const char *query)
{
    size_t text_index;
    size_t query_index;
    if (query[0] == '\0') return 1;
    for (text_index = 0U; text[text_index] != '\0'; ++text_index) {
        for (query_index = 0U; query[query_index] != '\0'; ++query_index) {
            unsigned char left = (unsigned char)text[text_index + query_index];
            unsigned char right = (unsigned char)query[query_index];
            if (left == '\0' || tolower(left) != tolower(right)) break;
        }
        if (query[query_index] == '\0') return 1;
    }
    return 0;
}

static UmiStatus add_node(
    UmiDataNavigatorModel *model,
    const char *node_id,
    const char *parent_id,
    const char *label,
    const char *detail,
    UmiDataNavigatorNodeKind kind,
    size_t depth,
    size_t source_index)
{
    UmiDataNavigatorNode *node;
    UmiStatus status;
    if (model->count >= UMI_DATA_WORKBENCH_MAX_NAVIGATOR_NODES) {
        model->truncated = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    node = &model->nodes[model->count];
    (void)memset(node, 0, sizeof(*node));
    node->struct_size = (uint32_t)sizeof(*node);
    node->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    status = umi_data_workbench_copy_text(
        node->node_id, sizeof(node->node_id), node_id);
    if (status == UMI_STATUS_OK) status = umi_data_workbench_copy_text(
        node->parent_id, sizeof(node->parent_id), parent_id);
    if (status == UMI_STATUS_OK) status = umi_data_workbench_copy_text(
        node->label, sizeof(node->label), label);
    if (status == UMI_STATUS_OK) status = umi_data_workbench_copy_text(
        node->detail, sizeof(node->detail), detail);
    if (status != UMI_STATUS_OK) return status;
    node->kind = kind;
    node->depth = depth;
    node->source_index = source_index;
    node->expanded = depth < 2U;
    model->count += 1U;
    return UMI_STATUS_OK;
}

static size_t find_node(const UmiDataNavigatorModel *model, const char *node_id)
{
    size_t index;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->nodes[index].node_id, node_id) == 0) return index;
    }
    return model->count;
}

static void rebuild_visible(UmiDataNavigatorModel *model)
{
    size_t index;
    model->visible_count = 0U;
    for (index = 0U; index < model->count; ++index) {
        if (contains_case_insensitive(model->nodes[index].label, model->filter) ||
            contains_case_insensitive(model->nodes[index].detail, model->filter)) {
            model->visible_indices[model->visible_count++] = index;
        }
    }
    if (model->selected_index >= model->visible_count) {
        model->selected_index = 0U;
    }
}

void umi_data_navigator_model_init(UmiDataNavigatorModel *model)
{
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->struct_size = (uint32_t)sizeof(*model);
    model->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    model->revision = 1U;
}

UmiStatus umi_data_navigator_model_build(
    UmiDataNavigatorModel *model,
    const UmiDatabaseExplorer *explorer)
{
    size_t index;
    UmiStatus status = UMI_STATUS_OK;
    if (model == NULL || explorer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_data_navigator_model_init(model);
    for (index = 0U; index < explorer->connections.count; ++index) {
        char id[UMI_DATABASE_ID_CAPACITY];
        int written = snprintf(id, sizeof(id), "connection:%s",
                               explorer->connections.items[index].id);
        if (written < 0 || (size_t)written >= sizeof(id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = add_node(model, id, "",
                          explorer->connections.items[index].display_name,
                          explorer->connections.items[index].endpoint,
                          UMI_DATA_NAVIGATOR_CONNECTION, 0U, index);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < explorer->schema.table_count; ++index) {
        const UmiDatabaseTableInfo *table = &explorer->schema.tables[index];
        char schema_id[UMI_DATABASE_ID_CAPACITY];
        char table_id[UMI_DATABASE_ID_CAPACITY];
        size_t column;
        const char *schema_name = table->schema_name[0] != '\0'
            ? table->schema_name : "default";
        int written = snprintf(schema_id, sizeof(schema_id), "schema:%s",
                               schema_name);
        if (written < 0 || (size_t)written >= sizeof(schema_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        if (find_node(model, schema_id) == model->count) {
            status = add_node(model, schema_id, "", schema_name, "Schema",
                              UMI_DATA_NAVIGATOR_SCHEMA, 0U, index);
            if (status != UMI_STATUS_OK) return status;
        }
        written = snprintf(table_id, sizeof(table_id), "table:%s.%s",
                           schema_name, table->name);
        if (written < 0 || (size_t)written >= sizeof(table_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = add_node(model, table_id, schema_id, table->name,
                          table->view ? "View" : "Table",
                          table->view ? UMI_DATA_NAVIGATOR_VIEW
                                      : UMI_DATA_NAVIGATOR_TABLE,
                          1U, index);
        if (status != UMI_STATUS_OK) return status;
        for (column = 0U; column < table->column_count; ++column) {
            char column_id[UMI_DATABASE_ID_CAPACITY];
            written = snprintf(column_id, sizeof(column_id), "column:%s.%s.%s",
                               schema_name, table->name,
                               table->columns[column].name);
            if (written < 0 || (size_t)written >= sizeof(column_id)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            status = add_node(model, column_id, table_id,
                              table->columns[column].name,
                              table->columns[column].type_name,
                              UMI_DATA_NAVIGATOR_COLUMN, 2U, column);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    rebuild_visible(model);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_data_navigator_model_set_filter(
    UmiDataNavigatorModel *model,
    const char *filter)
{
    UmiStatus status;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_data_workbench_copy_text(
        model->filter, sizeof(model->filter), filter != NULL ? filter : "");
    if (status != UMI_STATUS_OK) return status;
    rebuild_visible(model);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_data_navigator_model_select(
    UmiDataNavigatorModel *model,
    size_t visible_index)
{
    size_t index;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (visible_index >= model->visible_count) return UMI_STATUS_NOT_FOUND;
    for (index = 0U; index < model->count; ++index) {
        model->nodes[index].selected = 0;
    }
    model->selected_index = visible_index;
    model->nodes[model->visible_indices[visible_index]].selected = 1;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_data_navigator_model_set_expanded(
    UmiDataNavigatorModel *model,
    const char *node_id,
    int expanded)
{
    size_t index;
    if (model == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_node(model, node_id);
    if (index >= model->count) return UMI_STATUS_NOT_FOUND;
    model->nodes[index].expanded = expanded != 0;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiDataNavigatorNode *umi_data_navigator_model_visible_at(
    const UmiDataNavigatorModel *model,
    size_t visible_index)
{
    return model != NULL && visible_index < model->visible_count
        ? &model->nodes[model->visible_indices[visible_index]] : NULL;
}
