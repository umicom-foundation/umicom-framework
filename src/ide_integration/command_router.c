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

/* Provide the bindings operation used by this module and its client applications. */
static UmiIdeIntegrationBindings *bindings(UmiIdeCommandRouter *router)
{
    return umi_ide_integration_platform_bindings(router->platform);
}

/* Provide the navigation operation used by this module and its client applications. */
static UmiIdeCrossNavigation *navigation(UmiIdeCommandRouter *router)
{
    return umi_ide_integration_platform_navigation(router->platform);
}

/* Provide the inline controller operation used by this module and its client applications. */
static UmiIdeInlineController *inline_controller(UmiIdeCommandRouter *router)
{
    return umi_ide_integration_platform_inline(router->platform);
}

/* Provide the workflow operation used by this module and its client applications. */
static UmiIdeWorkflowService *workflow(UmiIdeCommandRouter *router)
{
    return umi_ide_integration_platform_workflow(router->platform);
}

/*
 * Provide the has active document operation used by this module and its client
 * applications.
 */
static int has_active_document(UmiIdeCommandRouter *router)
{
    UmiIdeWorkflowService *service = workflow(router);
    return service != NULL && service->context.has_document;
}

/* Provide the ai ids ready operation used by this module and its client applications. */
static int ai_ids_ready(const UmiIdeCommandContext *context)
{
    return context->ai_session_id[0] != '\0';
}

/*
 * Initialise ide command router from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ide_command_router_init(
    UmiIdeCommandRouter *router,
    UmiIdeIntegrationPlatform *platform)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL || platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(router, 0, sizeof(*router));
    router->platform = platform;
    router->sequence = 1U;
    umi_ide_command_context_init(&router->context);
    return UMI_STATUS_OK;
}

/*
 * Provide the ide command router set context operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_command_router_set_context(
    UmiIdeCommandRouter *router,
    const UmiIdeCommandContext *context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    router->context = *context;
    return UMI_STATUS_OK;
}

/*
 * Provide the ide command router enabled operation used by this module and its client
 * applications.
 */
int umi_ide_command_router_enabled(
    UmiIdeCommandRouter *router,
    const char *command_id)
{
    const UmiIdeCommandDescriptor *descriptor;
    UmiIdeIntegrationBindings *b;
    UmiIdeInlineController *inline_owner;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL || command_id == NULL) return 0;

    descriptor = umi_ide_command_find(command_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL) return 0;

    b = bindings(router);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (b == NULL) return 0;

    /* Apply this operation only while the related capability or state is available. */
    if (descriptor->requires_document && !has_active_document(router)) return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor->requires_problems && b->problems == NULL) return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor->requires_tests && b->tests == NULL) return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor->requires_source_control && b->source_control == NULL) return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor->requires_debug && b->debug_runtime == NULL) return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor->requires_language && b->language == NULL) return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor->requires_ai && b->ai_developer == NULL) return 0;

    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->requires_inline) {
        inline_owner = inline_controller(router);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (inline_owner == NULL) return 0;

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(command_id, "ide.inline.accept") == 0 ||
            strcmp(command_id, "ide.inline.reject") == 0) {
            return inline_owner->active.state == UMI_IDE_INLINE_READY;
        }

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(command_id, "ide.inline.cancel") == 0) {
            return inline_owner->active.state == UMI_IDE_INLINE_REQUESTED ||
                inline_owner->active.state == UMI_IDE_INLINE_READY;
        }

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(command_id, "ide.inline.request") == 0) {
            return router->context.has_editor_selection &&
                inline_owner->active.state != UMI_IDE_INLINE_REQUESTED &&
                inline_owner->active.state != UMI_IDE_INLINE_READY;
        }
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(command_id, "ide.ai.", 7U) == 0 &&
        strcmp(command_id, "ide.ai.open-overview") != 0 &&
        strcmp(command_id, "ide.ai.open-review") != 0) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!router->context.has_editor_selection ||
            !ai_ids_ready(&router->context)) {
            return 0;
        }
    }

    return 1;
}

/* Provide the ensure ai ids operation used by this module and its client applications. */
static void ensure_ai_ids(UmiIdeCommandRouter *router)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (router->context.ai_task_id[0] == '\0') {
        (void)snprintf(
            router->context.ai_task_id,
            sizeof(router->context.ai_task_id),
            "ide.ai.task.%llu",
            (unsigned long long)router->sequence);
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (router->context.ai_request_id[0] == '\0') {
        (void)snprintf(
            router->context.ai_request_id,
            sizeof(router->context.ai_request_id),
            "ide.ai.request.%llu",
            (unsigned long long)router->sequence);
    }

    router->sequence += 1U;
}

/*
 * Provide the open target command operation used by this module and its client
 * applications.
 */
static UmiStatus open_target_command(
    UmiIdeCommandRouter *router,
    const char *command_id)
{
    UmiIdeCrossNavigation *nav = navigation(router);
    char view_id[128];
    size_t offset = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (nav == NULL) return UMI_STATUS_INVALID_STATE;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.navigate.back") == 0) {
        return umi_ide_cross_navigation_back(
            nav, view_id, sizeof(view_id), &offset);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.navigate.forward") == 0) {
        return umi_ide_cross_navigation_forward(
            nav, view_id, sizeof(view_id), &offset);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.open.problem") == 0) {
        return umi_ide_cross_navigation_problem(
            nav,
            router->context.problem_index,
            view_id,
            sizeof(view_id),
            &offset);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.open.test") == 0) {
        return umi_ide_cross_navigation_test(
            nav,
            router->context.test_item_id,
            view_id,
            sizeof(view_id),
            &offset);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.open.source-control") == 0) {
        return umi_ide_cross_navigation_source_control(
            nav,
            router->context.source_control_path,
            router->context.source_control_line,
            view_id,
            sizeof(view_id),
            &offset);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.open.symbol") == 0) {
        return umi_ide_cross_navigation_symbol(
            nav,
            router->context.symbol_id,
            view_id,
            sizeof(view_id),
            &offset);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.open.diagnostic") == 0) {
        return umi_ide_cross_navigation_diagnostic(
            nav,
            router->context.diagnostic_index,
            view_id,
            sizeof(view_id),
            &offset);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.open.debug-frame") == 0) {
        return umi_ide_cross_navigation_debug_frame(
            nav,
            view_id,
            sizeof(view_id),
            &offset);
    }

    return UMI_STATUS_NOT_IMPLEMENTED;
}

/* Provide the inline command operation used by this module and its client applications. */
static UmiStatus inline_command(
    UmiIdeCommandRouter *router,
    const char *command_id)
{
    UmiIdeInlineController *owner = inline_controller(router);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (owner == NULL) return UMI_STATUS_INVALID_STATE;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.inline.request") == 0) {
        return umi_ide_inline_controller_request(
            owner,
            &router->context.editor_selection);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.inline.accept") == 0) {
        return umi_ide_inline_controller_accept(owner);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.inline.reject") == 0) {
        return umi_ide_inline_controller_reject(owner);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.inline.cancel") == 0) {
        return umi_ide_inline_controller_cancel(owner);
    }

    return UMI_STATUS_NOT_IMPLEMENTED;
}

/* Provide the ai command operation used by this module and its client applications. */
static UmiStatus ai_command(
    UmiIdeCommandRouter *router,
    const char *command_id)
{
    UmiIdeIntegrationBindings *b = bindings(router);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (b == NULL || b->ai_developer == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.ai.open-overview") == 0) {
        return umi_ai_developer_experience_platform_select_pane(
            b->ai_developer,
            UMI_AI_DEVELOPER_PANE_OVERVIEW);
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.ai.open-review") == 0) {
        return umi_ai_developer_experience_platform_select_pane(
            b->ai_developer,
            UMI_AI_DEVELOPER_PANE_PATCH_REVIEW);
    }

    ensure_ai_ids(router);

    /* Use the stable identifier comparison to choose the matching record or policy. */
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

    /* Use the stable identifier comparison to choose the matching record or policy. */
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

    /* Use the stable identifier comparison to choose the matching record or policy. */
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

    /* Use the stable identifier comparison to choose the matching record or policy. */
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

/* Provide the workflow command operation used by this module and its client applications. */
static UmiStatus workflow_command(
    UmiIdeCommandRouter *router,
    const char *command_id)
{
    UmiIdeWorkflowService *service = workflow(router);
    UmiIdeWorkflowPolicy policy;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_STATE;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.workflow.refresh") == 0 ||
        strcmp(command_id, "ide.workflow.ready") == 0) {
        return umi_ide_workflow_service_refresh(service);
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.workflow.profile.development") == 0) {
        status = umi_ide_workflow_profile_development(&policy);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(command_id, "ide.workflow.profile.pre-commit") == 0) {
        status = umi_ide_workflow_profile_pre_commit(&policy);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(command_id, "ide.workflow.profile.self-host") == 0 ||
               strcmp(command_id, "ide.self-host.verify") == 0) {
        status = umi_ide_workflow_profile_self_host(&policy);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(command_id, "ide.workflow.profile.release") == 0) {
        status = umi_ide_workflow_profile_release(&policy);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(command_id, "ide.workflow.profile.autonomous-ai") == 0) {
        status = umi_ide_workflow_profile_autonomous_ai(&policy);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ide_workflow_service_set_policy(service, &policy);
}

/* Provide the document command operation used by this module and its client applications. */
static UmiStatus document_command(
    UmiIdeCommandRouter *router,
    const char *command_id,
    int *out_changed)
{
    UmiIdeIntegrationBindings *b = bindings(router);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (b == NULL || b->documents == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.document.save") == 0) {
        return umi_document_coordinator_save_active(b->documents);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.document.sync") == 0) {
        return umi_document_coordinator_sync_active(b->documents);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command_id, "ide.document.check-external") == 0) {
        return umi_document_coordinator_check_external_change(
            b->documents,
            out_changed);
    }

    return UMI_STATUS_NOT_IMPLEMENTED;
}

/*
 * Perform ide command router through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ide_command_router_execute(
    UmiIdeCommandRouter *router,
    const char *command_id,
    char *out_message,
    size_t message_capacity)
{
    UmiStatus status;
    int changed = 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL || command_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_ide_command_router_enabled(router, command_id)) {
        return UMI_STATUS_INVALID_STATE;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(command_id, "ide.navigate.", 13U) == 0 ||
        strncmp(command_id, "ide.open.", 9U) == 0) {
        status = open_target_command(router, command_id);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(command_id, "ide.inline.", 11U) == 0) {
        status = inline_command(router, command_id);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(command_id, "ide.ai.", 7U) == 0) {
        status = ai_command(router, command_id);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(command_id, "ide.workflow.", 13U) == 0 ||
               strcmp(command_id, "ide.self-host.verify") == 0) {
        status = workflow_command(router, command_id);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(command_id, "ide.document.", 13U) == 0) {
        status = document_command(router, command_id, &changed);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = UMI_STATUS_NOT_IMPLEMENTED;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message != NULL && message_capacity > 0U) {
        const UmiIdeCommandDescriptor *descriptor =
            umi_ide_command_find(command_id);

        /* Use the stable identifier comparison to choose the matching record or policy. */
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
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(
                       command_id,
                       "ide.document.check-external") == 0 &&
                   status == UMI_STATUS_OK) {
            (void)snprintf(
                out_message,
                message_capacity,
                "External document change: %s",
                changed ? "detected" : "not detected");
        } /* Use this fallback path when the earlier condition does not apply. */ else {
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
