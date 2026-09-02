/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_fixture.h
 *
 * PURPOSE:
 *   Verify the terminal fixture behaviour and report a clear failure when its
 *   contract changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework terminal workbench test fixture.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TESTS_TERMINAL_FIXTURE_H
#define UMICOM_TESTS_TERMINAL_FIXTURE_H

#include <string.h>

#include "umicom/terminal_ui/terminal_ui.h"

/**
 * Represent the terminal test fixture data shared with callers of this public contract.
 */
typedef struct UmiTerminalTestFixture {
    UmiClock clock;
    UmiTerminalManager *manager;
    UmiProcessSupervisor *supervisor;
    UmiTaskQueue *tasks;
    UmiDiagnosticPipeline *pipeline;
    UmiBuildGraph *operations;
    UmiTerminalController *controller;
} UmiTerminalTestFixture;

/**
 * Initialise terminal fixture from caller-provided values so later operations receive a
 * known state.
 */
static UmiStatus terminal_fixture_create(UmiTerminalTestFixture *fixture)
{
    UmiTaskQueueConfig task_config;
    UmiTerminalControllerConfig controller_config;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fixture == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(fixture, 0, sizeof(*fixture));
    fixture->clock = umi_clock_system();
    status = umi_terminal_manager_create(&fixture->clock, &fixture->manager);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_process_supervisor_create(NULL, &fixture->supervisor);
    }
    task_config = umi_task_queue_config_default();
    task_config.worker_count = 1U;
    task_config.capacity = 8U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_task_queue_create(&task_config, &fixture->tasks);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_pipeline_create(NULL, &fixture->pipeline);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        UmiBuildGraphNodeSnapshot configure;
        UmiBuildGraphNodeSnapshot build;
        status = umi_build_graph_create(&fixture->operations);
        umi_build_graph_node_init(&configure, "configure", "Configure",
                                  UMI_BUILD_PHASE_CONFIGURE);
        umi_build_graph_node_init(&build, "build", "Build",
                                  UMI_BUILD_PHASE_BUILD);
        configure.maximum_attempts = 2U;
        build.maximum_attempts = 2U;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_build_graph_add_node(fixture->operations, &configure);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_build_graph_add_node(fixture->operations, &build);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_build_graph_add_dependency(
                fixture->operations, "build", "configure");
        }
    }
    controller_config = umi_terminal_controller_config_default();
    controller_config.manager = fixture->manager;
    controller_config.process_supervisor = fixture->supervisor;
    controller_config.task_queue = fixture->tasks;
    controller_config.diagnostic_pipeline = fixture->pipeline;
    controller_config.operation_graph = fixture->operations;
    controller_config.clock = &fixture->clock;
    controller_config.initial_session_id = "fixture.primary";
    controller_config.initial_title = "Fixture Terminal";
    controller_config.working_directory = ".";
    controller_config.history_capacity = 16U;
    controller_config.event_capacity = 32U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_controller_create(
            &controller_config, &fixture->controller);
    }
    return status;
}

/**
 * Release or reset state held by terminal fixture so the same storage can be reused
 * safely.
 */
static void terminal_fixture_destroy(UmiTerminalTestFixture *fixture)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fixture == NULL) return;
    umi_terminal_controller_destroy(fixture->controller);
    umi_build_graph_destroy(fixture->operations);
    umi_diagnostic_pipeline_destroy(fixture->pipeline);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fixture->tasks != NULL) {
        (void)umi_task_queue_shutdown(fixture->tasks, 1);
    }
    umi_task_queue_destroy(fixture->tasks);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fixture->supervisor != NULL) {
        (void)umi_process_supervisor_shutdown(fixture->supervisor);
    }
    umi_process_supervisor_destroy(fixture->supervisor);
    umi_terminal_manager_destroy(fixture->manager);
    umi_clock_dispose(&fixture->clock);
    (void)memset(fixture, 0, sizeof(*fixture));
}

#endif
