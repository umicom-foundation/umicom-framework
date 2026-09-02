/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/model.c
 *
 * PURPOSE:
 *   Implement canonical developer-project model initialisation and validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/model.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise developer project model from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_project_model_init(
    UmiDeveloperProjectModel *model,
    const char *project_id,
    const char *display_name)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;

    (void)memset(model, 0, sizeof(*model));
    model->structure_size = (uint32_t)sizeof(*model);
    model->api_version = UMI_DEVELOPER_PROJECT_API_VERSION;
    model->kind = UMI_DEVELOPER_PROJECT_EXECUTABLE;
    model->build_system = UMI_DEVELOPER_PROJECT_BUILD_CMAKE;
    model->revision = 1U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (project_id != NULL) {
        (void)copy_text(
            model->project_id, sizeof(model->project_id), project_id);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (display_name != NULL) {
        (void)copy_text(
            model->display_name,
            sizeof(model->display_name),
            display_name);
    }
}

/*
 * Check that developer project model satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_developer_project_model_validate(
    const UmiDeveloperProjectModel *model,
    char *out_message,
    size_t message_capacity)
{
    const char *message = "Project model is valid.";
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL ||
        model->structure_size != sizeof(*model) ||
        model->api_version != UMI_DEVELOPER_PROJECT_API_VERSION ||
        model->project_id[0] == '\0' ||
        model->display_name[0] == '\0' ||
        model->root[0] == '\0' ||
        model->primary_language_id[0] == '\0' ||
        model->kind < UMI_DEVELOPER_PROJECT_EXECUTABLE ||
        model->kind > UMI_DEVELOPER_PROJECT_META ||
        model->build_system < UMI_DEVELOPER_PROJECT_BUILD_NONE ||
        model->build_system > UMI_DEVELOPER_PROJECT_BUILD_CUSTOM ||
        model->secondary_language_count >
            UMI_DEVELOPER_PROJECT_LANGUAGE_CAPACITY) {
        message = "Project model is incomplete or invalid.";
        status = UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s", message);
    }

    return status;
}

/*
 * Provide the developer project model add language operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_model_add_language(
    UmiDeveloperProjectModel *model,
    const char *language_id)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || language_id == NULL || language_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(model->primary_language_id, language_id) == 0) {
        return UMI_STATUS_OK;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->secondary_language_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->secondary_language_ids[index], language_id) == 0) {
            return UMI_STATUS_OK;
        }
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->secondary_language_count >=
        UMI_DEVELOPER_PROJECT_LANGUAGE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = copy_text(
        model->secondary_language_ids[model->secondary_language_count],
        sizeof(model->secondary_language_ids[0]),
        language_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    model->secondary_language_count += 1U;
    model->revision += 1U;
    return UMI_STATUS_OK;
}
