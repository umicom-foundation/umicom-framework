/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/active_context.c
 *
 * PURPOSE:
 *   Project canonical Framework services into one active IDE snapshot.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/active_context.h"

#include <string.h>

UmiStatus umi_ide_active_context_snapshot(
    const UmiIdeIntegrationBindings *bindings,
    const char *workspace_root,
    UmiIdeActiveContext *out_context)
{
    size_t root_length;

    if (bindings == NULL || workspace_root == NULL ||
        out_context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    root_length = strlen(workspace_root);
    if (root_length >= sizeof(out_context->workspace_root)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(out_context, 0, sizeof(*out_context));
    (void)memcpy(
        out_context->workspace_root,
        workspace_root,
        root_length + 1U);

    if (bindings->documents != NULL &&
        umi_document_coordinator_active_snapshot(
            bindings->documents,
            &out_context->document) == UMI_STATUS_OK) {
        out_context->has_document = 1;
    }

    if (bindings->problems != NULL) {
        out_context->problems =
            umi_developer_problem_store_snapshot(bindings->problems);
        out_context->has_problems = 1;
    }

    if (bindings->tests != NULL &&
        umi_test_platform_service_snapshot(
            bindings->tests,
            &out_context->tests) == UMI_STATUS_OK) {
        out_context->has_tests = 1;
    }

    if (bindings->source_control != NULL &&
        umi_developer_source_control_snapshot(
            bindings->source_control,
            &out_context->source_control) == UMI_STATUS_OK) {
        out_context->has_source_control = 1;
    }

    if (bindings->debug_runtime != NULL &&
        umi_debug_runtime_platform_snapshot(
            bindings->debug_runtime,
            &out_context->debug) == UMI_STATUS_OK) {
        out_context->has_debug = 1;
    }

    if (bindings->ai_developer != NULL &&
        umi_ai_developer_experience_platform_snapshot(
            bindings->ai_developer,
            &out_context->ai) == UMI_STATUS_OK) {
        out_context->has_ai = 1;
    }

    out_context->revision = bindings->revision + 1U;
    return UMI_STATUS_OK;
}
