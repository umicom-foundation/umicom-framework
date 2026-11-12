/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/delivery_observation/main.c
 *
 * PURPOSE:
 *   Observe Framework task completion, failure and cooperative cancellation without launching tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/pipeline.h"
#include <stdio.h>
#include <string.h>

/* The application owns registries and forwards outcomes. Framework owns
 * dependency readiness and delivery phases; these command descriptions are
 * never sent to a shell by this teaching program. */
static UmiStatus Add(UmiBuildTaskRegistry *registry, const char *id, const char *dependency)
{
    UmiBuildTaskSnapshot task;
    umi_build_task_init(&task, id, id, UMI_BUILD_TASK_BUILD);
    UmiStatus status = umi_build_task_set_command(&task, "example-worker", ".");
    if (status == UMI_STATUS_OK && dependency != NULL) status = umi_build_task_add_dependency(&task, dependency);
    return status == UMI_STATUS_OK ? umi_build_task_registry_upsert(registry, &task) : status;
}
static UmiStatus Show(UmiBuildDeliveryPipeline *pipeline, UmiBuildDeliveryPipelineSnapshot *display)
{
    UmiStatus status = umi_build_delivery_pipeline_snapshot(pipeline, display);
    if (status == UMI_STATUS_OK) printf("Phase: %s; active: %zu; stop requested: %d\n",
        umi_build_pipeline_phase_text(display->phase), display->tasks.active_count, display->stop_requested);
    return status;
}
int main(int argc, char **argv)
{
    if (argc != 2 || (strcmp(argv[1], "complete") && strcmp(argv[1], "cancel") && strcmp(argv[1], "fail"))) {
        fputs("Usage: delivery-observation complete|cancel|fail\n", stderr); return 2;
    }
    UmiBuildTaskRegistry *registry = NULL;
    UmiBuildDeploymentTargetRegistry *targets = NULL;
    UmiBuildDeliveryPipeline *pipeline = NULL;
    UmiBuildDeliveryPipelineSnapshot display = {0};
    UmiStatus status = umi_build_task_registry_create(&registry);
    if (status == UMI_STATUS_OK) status = Add(registry, "compile", NULL);
    if (status == UMI_STATUS_OK) status = Add(registry, "package", "compile");
    if (status == UMI_STATUS_OK) status = umi_build_deployment_target_registry_create(&targets);
    if (status == UMI_STATUS_OK) status = umi_build_delivery_pipeline_create("learning.delivery", registry, targets, 1, &pipeline);
    if (status == UMI_STATUS_OK) status = umi_build_delivery_pipeline_plan_tasks(pipeline, "learning.operation", "package");
    if (status == UMI_STATUS_OK) status = umi_build_delivery_pipeline_begin_tasks(pipeline);
    if (status == UMI_STATUS_OK) status = umi_build_delivery_pipeline_start_task(pipeline, "compile");
    if (status != UMI_STATUS_OK) goto done;
    puts("Umicom delivery model - no external command is executed");
    status = Show(pipeline, &display);
    if (status != UMI_STATUS_OK) goto done;
    if (strcmp(argv[1], "cancel") == 0) {
        status = umi_build_delivery_pipeline_request_stop(pipeline);
        if (status == UMI_STATUS_OK) status = Show(pipeline, &display);
        if (status != UMI_STATUS_OK) goto done;
        if (display.phase != UMI_BUILD_PIPELINE_TASKS || display.tasks.active_count != 1 || !display.stop_requested) {
            status = UMI_STATUS_INTERNAL_ERROR; goto done;
        }
        puts("The compile observation is still active. A real worker must report its actual outcome before retirement.");
        /* Simulate a worker completing after a request. No OS process exists. */
        status = umi_build_delivery_pipeline_complete_task(pipeline, "compile", 0);
    } else if (strcmp(argv[1], "fail") == 0) {
        status = umi_build_delivery_pipeline_fail_task(pipeline, "compile", UMI_STATUS_INTERNAL_ERROR, 7);
    } else {
        status = umi_build_delivery_pipeline_complete_task(pipeline, "compile", 0);
        UmiBuildTaskSnapshot ready;
        if (status == UMI_STATUS_OK) status = umi_build_delivery_pipeline_next_task(pipeline, &ready);
        if (status == UMI_STATUS_OK) status = umi_build_delivery_pipeline_start_task(pipeline, ready.task_id);
        if (status == UMI_STATUS_OK) status = umi_build_delivery_pipeline_complete_task(pipeline, ready.task_id, 0);
    }
    if (status == UMI_STATUS_OK) status = Show(pipeline, &display);
    if (status == UMI_STATUS_OK) {
        UmiBuildPipelinePhase expected = strcmp(argv[1], "cancel") == 0 ? UMI_BUILD_PIPELINE_CANCELLED :
            strcmp(argv[1], "fail") == 0 ? UMI_BUILD_PIPELINE_FAILED : UMI_BUILD_PIPELINE_ARTIFACTS;
        if (display.phase != expected || display.tasks.active_count != 0) status = UMI_STATUS_INTERNAL_ERROR;
    }
    if (status == UMI_STATUS_OK) puts("Model outcome observed. No package, installation or deployment was produced.");
done:
    umi_build_delivery_pipeline_destroy(pipeline);
    umi_build_deployment_target_registry_destroy(targets);
    umi_build_task_registry_destroy(registry);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Delivery observation refused with status %d.\n", (int)status);
    return status == UMI_STATUS_OK ? 0 : 1;
}
