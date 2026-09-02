/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/generation_request.c
 *
 * PURPOSE:
 *   Validate New Project generation input and derive standard template values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/generation_request.h"

#include "umicom/base/text.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/*
 * Initialise developer project generation request from caller-provided values so later
 * operations receive a known state.
 */
void umi_developer_project_generation_request_init(
    UmiDeveloperProjectGenerationRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request != NULL) {
        (void)memset(request, 0, sizeof(*request));
    }
}

/* Provide the valid target name operation used by this module and its client applications. */
static int valid_target_name(const char *text)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || text[0] == '\0') return 0;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!(isalpha((unsigned char)text[0]) || text[0] == '_')) return 0;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; text[index] != '\0'; ++index) {
        const unsigned char value = (unsigned char)text[index];

        /* Apply this branch only when its contract condition is satisfied. */
        if (!(isalnum(value) || value == '_')) {
            return 0;
        }
    }

    return 1;
}

/*
 * Check that developer project generation request satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_developer_project_generation_request_validate(
    const UmiDeveloperProjectGenerationRequest *request,
    char *out_message,
    size_t message_capacity)
{
    const char *message = "Project generation request is valid.";
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL ||
        request->template_id[0] == '\0' ||
        request->application_name[0] == '\0' ||
        request->application_id[0] == '\0' ||
        request->repository_name[0] == '\0' ||
        request->project_root[0] == '\0' ||
        !valid_target_name(request->target_name)) {
        message =
            "Template, identity, repository, target and project root are required.";
        status = UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message != NULL && message_capacity > 0U) {
        (void)umi_text_copy_truncated(out_message, message_capacity, message);
    }

    return status;
}

/*
 * Provide the make upper identifier operation used by this module and its client
 * applications.
 */
static void make_upper_identifier(
    const char *text,
    char *out_text,
    size_t capacity)
{
    size_t index = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_text == NULL || capacity == 0U) return;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (text != NULL && text[index] != '\0' &&
           index + 1U < capacity) {
        const unsigned char value = (unsigned char)text[index];
        out_text[index] =
            (char)(isalnum(value)
                ? toupper(value)
                : '_');
        ++index;
    }

    out_text[index] = '\0';
}

/*
 * Provide the developer project generation request variables operation used by this module
 * and its client applications.
 */
UmiStatus umi_developer_project_generation_request_variables(
    const UmiDeveloperProjectGenerationRequest *request,
    UmiDeveloperProjectVariableSet *out_variables)
{
    char target_upper[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    char header_guard[UMI_DEVELOPER_PROJECT_VARIABLE_VALUE_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_variables == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_developer_project_generation_request_validate(
        request, NULL, 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_developer_project_variable_set_init(out_variables);

    status = umi_developer_project_variable_set(
        out_variables, "APPLICATION_NAME", request->application_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_developer_project_variable_set(
        out_variables, "APPLICATION_ID", request->application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_developer_project_variable_set(
        out_variables, "REPOSITORY_NAME", request->repository_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_developer_project_variable_set(
        out_variables, "PROJECT_NAME", request->application_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_developer_project_variable_set(
        out_variables, "TARGET_NAME", request->target_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    make_upper_identifier(
        request->target_name, target_upper, sizeof(target_upper));
    status = umi_developer_project_variable_set(
        out_variables, "TARGET_UPPER", target_upper);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_text_copy(header_guard, sizeof(header_guard), target_upper);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_text_append(header_guard, sizeof(header_guard), "_GENERATED_H");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_developer_project_variable_set(
        out_variables, "HEADER_GUARD", header_guard);
}
