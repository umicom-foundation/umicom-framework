/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/command_router.c
 *
 * PURPOSE:
 *   Implement cross-domain IDE commands over canonical Framework services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/command_router.h"

#include <stdio.h>
#include <string.h>

static UmiIdeIntegrationBindings *bindings(UmiIdeCommandRouter *router)
{
    return umi_ide_integration_platform_bindings(router->platform);
}

static UmiIdeCrossNavigation *navigation(UmiIdeCommandRouter *router)
{
    return umi_ide_integration_platform_navigation(router->platform);
}

static UmiIdeInlineController *inline_controller(UmiIdeCommandRouter *router)
{
    return umi_ide_integration_platform_inline(router->platform);
}

static UmiIdeWorkflowService *workflow(UmiIdeCommandRouter *router)
{
    return umi_ide_integration_platform_workflow(router->platform);
}

static int has_active_document(UmiIdeCommandRouter *router)
{
    UmiIdeWorkflowService *service = workflow(router);
    return service != NULL && service->context.has_document;
}

static int ai_ids_ready(const UmiIdeCommandContext *context)
{
    return context->ai_session_id[0] != '\0';
}

UmiStatus umi_ide_command_router_init(
    UmiIdeCommandRouter *router,
    UmiIdeIntegrationPlatform *platform)
{
    if (router == NULL || platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(router, 0, sizeof(*router));
    router->platform = platform;
    router->sequence = 1U;
    umi_ide_command_context_init(&router->context);
    return UMI_STATUS_OK;
}

UmiStatus umi_ide_command_router_set_context(
    UmiIdeCommandRouter *router,
    const UmiIdeCommandContext *context)
{
    if (router == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    router->context = *context;
    return UMI_STATUS_OK;
}

int umi_ide_command_router_enabled(
    UmiIdeCommandRouter *router,
    const char *command_id)
{
    const UmiIdeCommandDescriptor *descriptor;
    UmiIdeIntegrationBindings *b;
    UmiIdeInlineController *inline_owner;

    if (router == NULL || command_id == NULL) return 0;

    descriptor = umi_ide_command_find(command_id);
    if (descriptor == NULL) return 0;

    b = bindings(router);
    if (b == NULL) return 0;

    if (descriptor->requires_document && !has_active_document(router)) return 0;
    if (descriptor->requires_problems && b->problems == NULL) return 0;
    if (descriptor->requires_tests && b->tests == NULL) return 0;
    if (descriptor->requires_source_control && b->source_control == NULL) return 0;
    if (descriptor->requires_debug && b->debug_runtime == NULL) return 0;
    if (descriptor->requires_language && b->language == NULL) return 0;
    if (descriptor->requires_ai && b->ai_developer == NULL) return 0;

    if (descriptor->requires_inline) {
        inline_owner = inline_controller(router);
        if (inline_owner == NULL) return 0;

        if (strcmp(command_id, "ide.inline.accept") == 0 ||
            strcmp(command_id, "ide.inline.reject") == 0) {
            return inline_owner->active.state == UMI_IDE_INLINE_READY;
        }

        if (strcmp(command_id, "ide.inline.cancel") == 0) {
            return inline_owner->active.state == UMI_IDE_INLINE_REQUESTED ||
                inline_owner->active.state == UMI_IDE_INLINE_READY;
        }

        if (strcmp(command_id, "ide.inline.request") == 0) {
            return router->context.has_editor_selection &&
                inline_owner->active.state != UMI_IDE_INLINE_REQUESTED &&
                inline_owner->active.state != UMI_IDE_INLINE_READY;
        }
    }

    if (strncmp(command_id, "ide.ai.", 7U) == 0 &&
        strcmp(command_id, "ide.ai.open-overview") != 0 &&
        strcmp(command_id, "ide.ai.open-review") != 0) {
        if (!router->context.has_editor_selection ||
            !ai_ids_ready(&router->context)) {
            return 0;
        }
    }

    return 1;
}

static void ensure_ai_ids(UmiIdeCommandRouter *router)
{
    if (router->context.ai_task_id[0] == '\0') {
        (void)snprintf(
            router->context.ai_task_id,
            sizeof(router->context.ai_task_id),
            "ide.ai.task.%llu",
            (unsigned long long)router->sequence);
    }

    if (router->context.ai_request_id[0] == '\0') {
        (void)snprintf(
            router->context.ai_request_id,
            sizeof(router->context.ai_request_id),
            "ide.ai.request.%llu",
            (unsigned long long)router->sequence);
    }

    router->sequence += 1U;
}

static UmiStatus open_target_command(
    UmiIdeCommandRouter *router,
    const char *command_id)
{
    UmiIdeCrossNavigation *nav = navigation(router);
    char view_id[128];
    size_t offset = 0U;

    if (nav == NULL) return UMI_STATUS_INVALID_STATE;

    if (strcmp(command_id, "ide.navigate.back") == 0) {
        return umi_ide_cross_navigation_back(
            nav, view_id, sizeof(view_id), &offset);
    }
    if (strcmp(command_id, "ide.navigate.forward") == 0) {
        return umi_ide_cross_navigation_forward(
            nav, view_id, sizeof(view_id), &offset);
    }
    if (strcmp(command_id, "ide.open.problem") == 0) {
        return umi_ide_cross_navigation_problem(
            nav,
            router->context.problem_index,
            view_id,
            sizeof(view_id),
            &offset);
    }
    if (strcmp(command_id, "ide.open.test") == 0) {
        return umi_ide_cross_navigation_test(
            nav,
            router->context.test_item_id,
            view_id,
            sizeof(view_id),
            &offset);
    }
    if (strcmp(command_id, "ide.open.source-control") == 0) {
        return umi_ide_cross_navigation_source_control(
            nav,
            router->context.source_control_path,
            router->context.source_control_line,
            view_id,
            sizeof(view_id),
            &offset);
    }
    if (strcmp(command_id, "ide.open.symbol") == 0) {
        return umi_ide_cross_navigation_symbol(
            nav,
            router->context.symbol_id,
            view_id,
            sizeof(view_id),
            &offset);
    }
    if (strcmp(command_id, "ide.open.diagnostic") == 0) {
        return umi_ide_cross_navigation_diagnostic(
            nav,
            router->context.diagnostic_index,
            view_id,
            sizeof(view_id),
            &offset);
    }
    if (strcmp(command_id, "ide.open.debug-frame") == 0) {
        return umi_ide_cross_navigation_debug_frame(
            nav,
            view_id,
            sizeof(view_id),
            &offset);
    }

    return UMI_STATUS_NOT_IMPLEMENTED;
}

static UmiStatus inline_command(
    UmiIdeCommandRouter *router,
    const char *command_id)
{
    UmiIdeInlineController *owner = inline_controller(router);

    if (owner == NULL) return UMI_STATUS_INVALID_STATE;

    if (strcmp(command_id, "ide.inline.request") == 0) {
        return umi_ide_inline_controller_request(
            owner,
            &router->context.editor_selection);
    }
    if (strcmp(command_id, "ide.inline.accept") == 0) {
        return umi_ide_inline_controller_accept(owner);
    }
    if (strcmp(command_id, "ide.inline.reject") == 0) {
        return umi_ide_inline_controller_reject(owner);
    }
    if (strcmp(command_id, "ide.inline.cancel") == 0) {
        return umi_ide_inline_controller_cancel(owner);
    }

    return UMI_STATUS_NOT_IMPLEMENTED;
}

static UmiStatus ai_command(
    UmiIdeCommandRouter *router,
    const char *command_id)
{
    UmiIdeIntegrationBindings *b = bindings(router);

    if (b == NULL || b->ai_developer == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    if (strcmp(command_id, "ide.ai.open-overview") == 0) {
        return umi_ai_developer_experience_platform_select_pane(
            b->ai_developer,
            UMI_AI_DEVELOPER_PANE_OVERVIEW);
    }

    if (strcmp(command_id, "ide.ai.open-review") == 0) {
        return umi_ai_developer_experience_platform_select_pane(
            b->ai_developer,
            UMI_AI_DEVELOPER_PANE_PATCH_REVIEW);
    }

    ensure_ai_ids(router);

    if (strcmp(command_id, "ide.ai.explain-selection") == 0) {
        return umi_ide_ai_run_selection(
            router->platform,
            &router->context.editor_selection,
            UMI_AI_CODING_TASK_EXPLAIN,
            router->context.ai_task_id,
            router->context.ai_request_id,
            router->context.ai_session_id,
            "Explain the selected code using repository context. Do not modify files.");
    }

    if (strcmp(command_id, "ide.ai.refactor-selection") == 0) {
        return umi_ide_ai_run_selection(
            router->platform,
            &router->context.editor_selection,
            UMI_AI_CODING_TASK_REFACTOR,
            router->context.ai_task_id,
            router->context.ai_request_id,
            router->context.ai_session_id,
            "Refactor the selected code while preserving behavior, comments and unrelated code. Produce a governed complete-file patch.");
    }

    if (strcmp(command_id, "ide.ai.generate-tests") == 0) {
        return umi_ide_ai_run_selection(
            router->platform,
            &router->context.editor_selection,
            UMI_AI_CODING_TASK_GENERATE_TESTS,
            router->context.ai_task_id,
            router->context.ai_request_id,
            router->context.ai_session_id,
            "Generate focused tests for the selected code using existing project conventions. Produce a governed complete-file patch.");
    }

    if (strcmp(command_id, "ide.ai.fix-problem") == 0) {
        return umi_ide_ai_fix_problem(
            router->platform,
            &router->context.editor_selection,
            router->context.problem_index,
            router->context.ai_task_id,
            router->context.ai_request_id,
            router->context.ai_session_id);
    }

    return UMI_STATUS_NOT_IMPLEMENTED;
}

static UmiStatus workflow_command(
    UmiIdeCommandRouter *router,
    const char *command_id)
{
    UmiIdeWorkflowService *service = workflow(router);
    UmiIdeWorkflowPolicy policy;
    UmiStatus status;

    if (service == NULL) return UMI_STATUS_INVALID_STATE;

    if (strcmp(command_id, "ide.workflow.refresh") == 0 ||
        strcmp(command_id, "ide.workflow.ready") == 0) {
        return umi_ide_workflow_service_refresh(service);
    }

    if (strcmp(command_id, "ide.workflow.profile.development") == 0) {
        status = umi_ide_workflow_profile_development(&policy);
    } else if (strcmp(command_id, "ide.workflow.profile.pre-commit") == 0) {
        status = umi_ide_workflow_profile_pre_commit(&policy);
    } else if (strcmp(command_id, "ide.workflow.profile.self-host") == 0 ||
               strcmp(command_id, "ide.self-host.verify") == 0) {
        status = umi_ide_workflow_profile_self_host(&policy);
    } else if (strcmp(command_id, "ide.workflow.profile.release") == 0) {
        status = umi_ide_workflow_profile_release(&policy);
    } else if (strcmp(command_id, "ide.workflow.profile.autonomous-ai") == 0) {
        status = umi_ide_workflow_profile_autonomous_ai(&policy);
    } else {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    if (status != UMI_STATUS_OK) return status;
    return umi_ide_workflow_service_set_policy(service, &policy);
}

static UmiStatus document_command(
    UmiIdeCommandRouter *router,
    const char *command_id,
    int *out_changed)
{
    UmiIdeIntegrationBindings *b = bindings(router);

    if (b == NULL || b->documents == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    if (strcmp(command_id, "ide.document.save") == 0) {
        return umi_document_coordinator_save_active(b->documents);
    }
    if (strcmp(command_id, "ide.document.sync") == 0) {
        return umi_document_coordinator_sync_active(b->documents);
    }
    if (strcmp(command_id, "ide.document.check-external") == 0) {
        return umi_document_coordinator_check_external_change(
            b->documents,
            out_changed);
    }

    return UMI_STATUS_NOT_IMPLEMENTED;
}

UmiStatus umi_ide_command_router_execute(
    UmiIdeCommandRouter *router,
    const char *command_id,
    char *out_message,
    size_t message_capacity)
{
    UmiStatus status;
    int changed = 0;

    if (router == NULL || command_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!umi_ide_command_router_enabled(router, command_id)) {
        return UMI_STATUS_INVALID_STATE;
    }

    if (strncmp(command_id, "ide.navigate.", 13U) == 0 ||
        strncmp(command_id, "ide.open.", 9U) == 0) {
        status = open_target_command(router, command_id);
    } else if (strncmp(command_id, "ide.inline.", 11U) == 0) {
        status = inline_command(router, command_id);
    } else if (strncmp(command_id, "ide.ai.", 7U) == 0) {
        status = ai_command(router, command_id);
    } else if (strncmp(command_id, "ide.workflow.", 13U) == 0 ||
               strcmp(command_id, "ide.self-host.verify") == 0) {
        status = workflow_command(router, command_id);
    } else if (strncmp(command_id, "ide.document.", 13U) == 0) {
        status = document_command(router, command_id, &changed);
    } else {
        status = UMI_STATUS_NOT_IMPLEMENTED;
    }

    if (out_message != NULL && message_capacity > 0U) {
        const UmiIdeCommandDescriptor *descriptor =
            umi_ide_command_find(command_id);

        if (strcmp(command_id, "ide.workflow.ready") == 0 ||
            strcmp(command_id, "ide.self-host.verify") == 0) {
            UmiIdeWorkflowService *service = workflow(router);

            (void)snprintf(
                out_message,
                message_capacity,
                "%s: %s (%zu passed, %zu warnings, %zu blocked)",
                descriptor != NULL ? descriptor->label : command_id,
                service != NULL && service->report.ready ? "ready" : "blocked",
                service != NULL ? service->report.passed_count : 0U,
                service != NULL ? service->report.warning_count : 0U,
                service != NULL ? service->report.blocked_count : 0U);
        } else if (strcmp(
                       command_id,
                       "ide.document.check-external") == 0 &&
                   status == UMI_STATUS_OK) {
            (void)snprintf(
                out_message,
                message_capacity,
                "External document change: %s",
                changed ? "detected" : "not detected");
        } else {
            (void)snprintf(
                out_message,
                message_capacity,
                "%s: %s",
                descriptor != NULL ? descriptor->label : command_id,
                status == UMI_STATUS_OK ? "completed" : "failed");
        }
    }

    return status;
}
