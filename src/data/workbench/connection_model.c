/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/connection_model.c
 *
 * PURPOSE:
 *   Implement filterable connection-profile presentation over the canonical
 *   connection registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/connection_model.h"

#include <ctype.h>
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

UmiStatus umi_data_workbench_connection_model_refresh(
    UmiDataWorkbenchConnectionModel *model)
{
    size_t index;
    if (model == NULL || model->registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    model->visible_count = 0U;
    model->ready_count = 0U;
    model->failed_count = 0U;
    for (index = 0U; index < model->registry->count; ++index) {
        const UmiDatabaseConnectionProfile *profile =
            &model->registry->items[index];
        if (profile->state == UMI_DATABASE_READY) model->ready_count += 1U;
        if (profile->state == UMI_DATABASE_FAILED) model->failed_count += 1U;
        if (contains_case_insensitive(profile->display_name, model->filter) ||
            contains_case_insensitive(profile->database_name, model->filter) ||
            contains_case_insensitive(profile->endpoint, model->filter)) {
            model->visible_indices[model->visible_count++] = index;
        }
    }
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_data_workbench_connection_model_init(
    UmiDataWorkbenchConnectionModel *model,
    UmiDatabaseConnectionRegistry *registry)
{
    if (model == NULL || registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(model, 0, sizeof(*model));
    model->struct_size = (uint32_t)sizeof(*model);
    model->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    model->registry = registry;
    model->revision = 1U;
    return umi_data_workbench_connection_model_refresh(model);
}

UmiStatus umi_data_workbench_connection_model_set_filter(
    UmiDataWorkbenchConnectionModel *model,
    const char *filter)
{
    UmiStatus status;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_data_workbench_copy_text(
        model->filter, sizeof(model->filter), filter != NULL ? filter : "");
    return status == UMI_STATUS_OK
        ? umi_data_workbench_connection_model_refresh(model) : status;
}

UmiStatus umi_data_workbench_connection_model_activate(
    UmiDataWorkbenchConnectionModel *model,
    const char *connection_id)
{
    UmiStatus status;
    if (model == NULL || model->registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_database_connection_registry_activate(
        model->registry, connection_id);
    if (status == UMI_STATUS_OK) model->revision += 1U;
    return status;
}

const UmiDatabaseConnectionProfile *
umi_data_workbench_connection_model_visible_at(
    const UmiDataWorkbenchConnectionModel *model,
    size_t visible_index)
{
    if (model == NULL || model->registry == NULL ||
        visible_index >= model->visible_count) {
        return NULL;
    }
    return &model->registry->items[model->visible_indices[visible_index]];
}
