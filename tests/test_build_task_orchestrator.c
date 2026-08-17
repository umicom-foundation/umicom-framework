/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_task_orchestrator.c
 *
 * PURPOSE:
 *   Verify dependency-closed planning, execution completion and cancellation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/build/task_orchestrator.h"

static void add_task(UmiBuildTaskRegistry *registry,
                     const char *id,
                     UmiBuildTaskKind kind,
                     const char *dependency)
{
    UmiBuildTaskSnapshot task;
    umi_build_task_init(&task, id, id, kind);
    assert(umi_build_task_set_command(&task, id, ".") == UMI_STATUS_OK);
    if (dependency != NULL) {
        assert(umi_build_task_add_dependency(&task, dependency) ==
               UMI_STATUS_OK);
    }
    assert(umi_build_task_registry_upsert(registry, &task) == UMI_STATUS_OK);
}

int main(void)
{
    UmiBuildTaskRegistry *registry = NULL;
    UmiBuildTaskOrchestrator *orchestrator = NULL;
    UmiBuildTaskSnapshot task;
    UmiBuildTaskOperationSnapshot snapshot;
    size_t completed = 0U;

    assert(umi_build_task_registry_create(&registry) == UMI_STATUS_OK);
    add_task(registry, "configure", UMI_BUILD_TASK_CONFIGURE, NULL);
    add_task(registry, "build", UMI_BUILD_TASK_BUILD, "configure");
    add_task(registry, "test", UMI_BUILD_TASK_TEST, "build");
    assert(umi_build_task_orchestrator_create(
               registry, 2U, &orchestrator) == UMI_STATUS_OK);
    assert(umi_build_task_orchestrator_plan(
               orchestrator, "verify.operation", "test") == UMI_STATUS_OK);
    assert(umi_build_task_orchestrator_begin(orchestrator) == UMI_STATUS_OK);
    while (umi_build_task_orchestrator_next_ready(
               orchestrator, &task) == UMI_STATUS_OK) {
        assert(umi_build_task_orchestrator_start(
                   orchestrator, task.task_id) == UMI_STATUS_OK);
        assert(umi_build_task_orchestrator_complete(
                   orchestrator, task.task_id, 0) == UMI_STATUS_OK);
        completed += 1U;
    }
    assert(completed == 3U);
    assert(umi_build_task_orchestrator_snapshot(
               orchestrator, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.state == UMI_BUILD_TASK_OPERATION_SUCCEEDED);
    assert(snapshot.planned_count == 3U);
    assert(snapshot.completed_count == 3U);

    assert(umi_build_task_orchestrator_plan(
               orchestrator, "cancel.operation", NULL) == UMI_STATUS_OK);
    assert(umi_build_task_orchestrator_begin(orchestrator) == UMI_STATUS_OK);
    assert(umi_build_task_orchestrator_request_stop(orchestrator) ==
           UMI_STATUS_OK);
    assert(umi_build_task_orchestrator_snapshot(
               orchestrator, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.state == UMI_BUILD_TASK_OPERATION_CANCELLED);
    assert(snapshot.stop_requested != 0);

    umi_build_task_orchestrator_destroy(orchestrator);
    umi_build_task_registry_destroy(registry);
    return 0;
}
