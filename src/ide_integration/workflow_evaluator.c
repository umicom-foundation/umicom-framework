/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/workflow_evaluator.c
 *
 * PURPOSE:
 *   Evaluate workspace/document/problems/tests/source-control/debug/AI gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/workflow_evaluator.h"

#include <stdio.h>
#include <string.h>

static UmiStatus add_workspace(
    const UmiIdeActiveContext *context,
    const UmiIdeWorkflowPolicy *policy,
    UmiIdeWorkflowReport *report)
{
    const int available = context->workspace_root[0] != '\0';
    return umi_ide_workflow_report_add(
        report,
        "workspace",
        "Workspace",
        available ? context->workspace_root : "No workspace root is active.",
        available ? UMI_IDE_GATE_PASS : UMI_IDE_GATE_BLOCK,
        policy->require_workspace);
}

static UmiStatus add_document(
    const UmiIdeActiveContext *context,
    const UmiIdeWorkflowPolicy *policy,
    UmiIdeWorkflowReport *report)
{
    char detail[512];
    UmiIdeWorkflowGateState state;

    if (!context->has_document) {
        return umi_ide_workflow_report_add(
            report,
            "document",
            "Active Document",
            "No active document.",
            policy->require_document
                ? UMI_IDE_GATE_BLOCK
                : UMI_IDE_GATE_WARN,
            policy->require_document);
    }

    (void)snprintf(
        detail,
        sizeof(detail),
        "%.300s | language %.80s | revision %llu | dirty=%d",
        context->document.display_name,
        context->document.language_id,
        (unsigned long long)context->document.revision,
        context->document.dirty);

    state =
        context->document.dirty && !policy->allow_dirty_document
            ? UMI_IDE_GATE_BLOCK
            : context->document.dirty
                ? UMI_IDE_GATE_WARN
                : UMI_IDE_GATE_PASS;

    return umi_ide_workflow_report_add(
        report,
        "document",
        "Active Document",
        detail,
        state,
        policy->require_document || !policy->allow_dirty_document);
}

static UmiStatus add_problems(
    const UmiIdeActiveContext *context,
    const UmiIdeWorkflowPolicy *policy,
    UmiIdeWorkflowReport *report)
{
    char detail[256];
    UmiIdeWorkflowGateState state = UMI_IDE_GATE_PASS;

    if (!context->has_problems) {
        return umi_ide_workflow_report_add(
            report,
            "problems",
            "Problems",
            "Problems service not bound.",
            UMI_IDE_GATE_WARN,
            0);
    }

    (void)snprintf(
        detail,
        sizeof(detail),
        "errors=%zu fatals=%zu warnings=%zu total=%zu",
        context->problems.errors,
        context->problems.fatals,
        context->problems.warnings,
        context->problems.count);

    if ((policy->require_no_fatals && context->problems.fatals > 0U) ||
        (policy->require_no_errors && context->problems.errors > 0U)) {
        state = UMI_IDE_GATE_BLOCK;
    } else if (context->problems.warnings > 0U) {
        state = UMI_IDE_GATE_WARN;
    }

    return umi_ide_workflow_report_add(
        report,
        "problems",
        "Problems",
        detail,
        state,
        policy->require_no_errors || policy->require_no_fatals);
}

static UmiStatus add_tests(
    const UmiIdeIntegrationBindings *bindings,
    const UmiIdeActiveContext *context,
    const UmiIdeWorkflowPolicy *policy,
    UmiIdeWorkflowReport *report)
{
    UmiTestPlatformResultRegistry *results;
    size_t index;
    size_t failed = 0U;
    size_t passed = 0U;
    char detail[256];
    UmiIdeWorkflowGateState state;

    if (!context->has_tests || bindings->tests == NULL) {
        return umi_ide_workflow_report_add(
            report,
            "tests",
            "Tests",
            "Test Platform service not bound.",
            policy->require_tests ? UMI_IDE_GATE_BLOCK : UMI_IDE_GATE_WARN,
            policy->require_tests);
    }

    results = umi_test_platform_service_result(bindings->tests);

    if (results != NULL) {
        for (index = 0U;
             index < umi_test_platform_result_registry_count(results);
             ++index) {
            UmiTestPlatformResultSnapshot item;

            if (umi_test_platform_result_registry_at(
                    results, index, &item) != UMI_STATUS_OK) {
                continue;
            }

            if (item.outcome == UMI_TEST_PLATFORM_OUTCOME_PASSED) {
                passed += 1U;
            } else if (item.outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED ||
                       item.outcome == UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT) {
                failed += 1U;
            }
        }
    }

    (void)snprintf(
        detail,
        sizeof(detail),
        "items=%zu results=%zu passed=%zu failed=%zu running=%d",
        context->tests.item_count,
        context->tests.result_count,
        passed,
        failed,
        context->tests.operation_running);

    if (policy->require_tests && context->tests.result_count == 0U) {
        state = UMI_IDE_GATE_BLOCK;
    } else if (policy->require_all_tests_passed && failed > 0U) {
        state = UMI_IDE_GATE_BLOCK;
    } else if (context->tests.operation_running) {
        state = UMI_IDE_GATE_WARN;
    } else {
        state = UMI_IDE_GATE_PASS;
    }

    return umi_ide_workflow_report_add(
        report,
        "tests",
        "Tests",
        detail,
        state,
        policy->require_tests || policy->require_all_tests_passed);
}

static UmiStatus add_source_control(
    const UmiIdeActiveContext *context,
    const UmiIdeWorkflowPolicy *policy,
    UmiIdeWorkflowReport *report)
{
    char detail[256];
    UmiIdeWorkflowGateState state;

    if (!context->has_source_control) {
        return umi_ide_workflow_report_add(
            report,
            "source-control",
            "Source Control",
            "Source Control service not bound.",
            policy->require_source_control
                ? UMI_IDE_GATE_BLOCK
                : UMI_IDE_GATE_WARN,
            policy->require_source_control);
    }

    (void)snprintf(
        detail,
        sizeof(detail),
        "branch=%.120s changes=%zu staged=%zu available=%d",
        context->source_control.branch.name,
        context->source_control.change_count,
        context->source_control.staged_count,
        context->source_control.available);

    if (!context->source_control.available && policy->require_source_control) {
        state = UMI_IDE_GATE_BLOCK;
    } else if (policy->require_clean_source_control &&
               context->source_control.change_count > 0U) {
        state = UMI_IDE_GATE_BLOCK;
    } else if (context->source_control.change_count > 0U) {
        state = UMI_IDE_GATE_WARN;
    } else {
        state = UMI_IDE_GATE_PASS;
    }

    return umi_ide_workflow_report_add(
        report,
        "source-control",
        "Source Control",
        detail,
        state,
        policy->require_source_control ||
            policy->require_clean_source_control);
}

static UmiStatus add_debug(
    const UmiIdeActiveContext *context,
    const UmiIdeWorkflowPolicy *policy,
    UmiIdeWorkflowReport *report)
{
    char detail[256];
    UmiIdeWorkflowGateState state;

    if (!context->has_debug) {
        return umi_ide_workflow_report_add(
            report,
            "debugger",
            "Debugger",
            "Debug Runtime service not bound.",
            UMI_IDE_GATE_WARN,
            0);
    }

    (void)snprintf(
        detail,
        sizeof(detail),
        "active=%d paused=%d session=%.100s thread=%llu frame=%llu",
        context->debug.active,
        context->debug.paused,
        context->debug.active_session_id,
        (unsigned long long)context->debug.active_thread_id,
        (unsigned long long)context->debug.active_frame_id);

    state =
        context->debug.active && policy->require_debugger_idle
            ? UMI_IDE_GATE_BLOCK
            : context->debug.active
                ? UMI_IDE_GATE_WARN
                : UMI_IDE_GATE_PASS;

    return umi_ide_workflow_report_add(
        report,
        "debugger",
        "Debugger",
        detail,
        state,
        policy->require_debugger_idle);
}

static UmiStatus add_ai(
    const UmiIdeActiveContext *context,
    const UmiIdeWorkflowPolicy *policy,
    UmiIdeWorkflowReport *report)
{
    char detail[256];
    UmiIdeWorkflowGateState state;

    if (!context->has_ai) {
        return umi_ide_workflow_report_add(
            report,
            "ai-developer",
            "AI Developer",
            "AI Developer Experience service not bound.",
            UMI_IDE_GATE_WARN,
            0);
    }

    (void)snprintf(
        detail,
        sizeof(detail),
        "state=%s pendingApprovals=%zu reviewLoaded=%d",
        umi_ai_coding_runtime_state_text(context->ai.coding.agent.state),
        context->ai.pending_approval_count,
        context->ai.review_loaded);

    if (policy->require_no_pending_ai_approval &&
        context->ai.pending_approval_count > 0U) {
        state = UMI_IDE_GATE_BLOCK;
    } else if (policy->require_ai_idle &&
               context->ai.coding.agent.state != UMI_AI_CODING_RUNTIME_IDLE &&
               context->ai.coding.agent.state != UMI_AI_CODING_RUNTIME_COMPLETED) {
        state = UMI_IDE_GATE_BLOCK;
    } else if (context->ai.coding.agent.state == UMI_AI_CODING_RUNTIME_FAILED) {
        state = UMI_IDE_GATE_WARN;
    } else {
        state = UMI_IDE_GATE_PASS;
    }

    return umi_ide_workflow_report_add(
        report,
        "ai-developer",
        "AI Developer",
        detail,
        state,
        policy->require_no_pending_ai_approval || policy->require_ai_idle);
}

UmiStatus umi_ide_workflow_evaluate(
    const UmiIdeIntegrationBindings *bindings,
    const UmiIdeActiveContext *context,
    const UmiIdeWorkflowPolicy *policy,
    UmiIdeWorkflowReport *out_report)
{
    UmiStatus status;

    if (bindings == NULL || context == NULL ||
        policy == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_ide_workflow_report_init(out_report);

    status = add_workspace(context, policy, out_report);
    if (status == UMI_STATUS_OK)
        status = add_document(context, policy, out_report);
    if (status == UMI_STATUS_OK)
        status = add_problems(context, policy, out_report);
    if (status == UMI_STATUS_OK)
        status = add_tests(bindings, context, policy, out_report);
    if (status == UMI_STATUS_OK)
        status = add_source_control(context, policy, out_report);
    if (status == UMI_STATUS_OK)
        status = add_debug(context, policy, out_report);
    if (status == UMI_STATUS_OK)
        status = add_ai(context, policy, out_report);

    return status;
}
