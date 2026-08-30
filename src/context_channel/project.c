/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/project.c
 *
 * PURPOSE:
 *   Implement canonical project context validation and mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/project.h"
#include <string.h>
void umi_project_context_init(UmiProjectContext *context)
{
    if (context == NULL) return;
    memset(context, 0, sizeof(*context));
    context->structure_size = (uint32_t)sizeof(*context);
    context->revision = 1U;
}
UmiStatus umi_project_context_validate(const UmiProjectContext *context)
{
    if (context == NULL || context->structure_size != sizeof(*context)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->project_id, sizeof(context->project_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->repository_id, sizeof(context->repository_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->root_path, sizeof(context->root_path))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->target_id, sizeof(context->target_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->configuration_id, sizeof(context->configuration_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(context->language_id, sizeof(context->language_id))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_project_context_copy(UmiProjectContext *destination, const UmiProjectContext *source)
{
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_project_context_validate(source) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source;
    return UMI_STATUS_OK;
}
UmiStatus umi_project_context_set_project_id(UmiProjectContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->project_id, sizeof(context->project_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_project_context_set_repository_id(UmiProjectContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->repository_id, sizeof(context->repository_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_project_context_set_root_path(UmiProjectContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->root_path, sizeof(context->root_path), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_project_context_set_target_id(UmiProjectContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->target_id, sizeof(context->target_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_project_context_set_configuration_id(UmiProjectContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->configuration_id, sizeof(context->configuration_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
UmiStatus umi_project_context_set_language_id(UmiProjectContext *context, const char *value)
{
    UmiStatus status;
    if (context == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(context->language_id, sizeof(context->language_id), value);
    if (status == UMI_STATUS_OK) context->revision += 1U;
    return status;
}
