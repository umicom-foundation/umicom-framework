/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_pipeline.c
 *
 * PURPOSE:
 *   Verify dependency scheduling, cycle rejection, blocking, retry and lifecycle
 *   state transitions in the reusable developer pipeline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "umicom/developer/pipeline.h"

/*
 * Exercise operation and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiDeveloperOperationSnapshot operation(
    const char *id,
    UmiDeveloperOperationKind kind,
    const char *title)
{
    UmiDeveloperOperationSnapshot result;
    assert(umi_developer_operation_init(&result, id, kind, title) ==
           UMI_STATUS_OK);
    result.max_attempts = 2U;
    return result;
}

/* A bounded dense DAG shares ancestors through many paths. Cycle admission
 * must count distinct queued operations and leave state unchanged on refusal. */
static void DenseDependencies(void)
{
    UmiDeveloperPipeline *pipeline=NULL;
    assert(umi_developer_pipeline_create(&pipeline)==UMI_STATUS_OK);
    for(size_t i=0U;i<48U;++i){
        char id[32];snprintf(id,sizeof id,"dense.%zu",i);
        UmiDeveloperOperationSnapshot item=operation(id,UMI_DEVELOPER_OPERATION_CUSTOM,id);
        assert(umi_developer_pipeline_submit(pipeline,&item)==UMI_STATUS_OK);
    }
    size_t edges=0U;
    for(size_t i=1U;i<48U && edges<UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY;++i){
        for(size_t j=0U;j<i && edges<UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY;++j){
            char a[32],b[32];snprintf(a,sizeof a,"dense.%zu",i);snprintf(b,sizeof b,"dense.%zu",j);
            assert(umi_developer_pipeline_add_dependency(pipeline,a,b)==UMI_STATUS_OK);
            ++edges;
            UmiDeveloperPipelineSnapshot before,after;
            assert(umi_developer_pipeline_snapshot(pipeline,&before)==UMI_STATUS_OK);
            if(edges<UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY){
                assert(umi_developer_pipeline_add_dependency(pipeline,b,a)==UMI_STATUS_INVALID_STATE);
                assert(umi_developer_pipeline_snapshot(pipeline,&after)==UMI_STATUS_OK);
                assert(after.dependency_count==before.dependency_count && after.revision==before.revision);
            }
        }
    }
    assert(edges==UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY);
    UmiDeveloperPipelineSnapshot snapshot;
    assert(umi_developer_pipeline_snapshot(pipeline,&snapshot)==UMI_STATUS_OK);
    assert(snapshot.dependency_count==edges);
    assert(umi_developer_pipeline_add_dependency(pipeline,"dense.47","dense.0")==UMI_STATUS_CAPACITY_EXCEEDED);
    umi_developer_pipeline_destroy(pipeline);
}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperPipeline *pipeline = NULL;
    UmiDeveloperOperationSnapshot configure;
    UmiDeveloperOperationSnapshot build;
    UmiDeveloperOperationSnapshot test;
    UmiDeveloperOperationSnapshot next;
    UmiDeveloperOperationSnapshot item;
    UmiDeveloperPipelineSnapshot snapshot;

    assert(umi_developer_pipeline_create(&pipeline) == UMI_STATUS_OK);

    configure = operation("plan.configure",
                          UMI_DEVELOPER_OPERATION_CONFIGURE,
                          "Configure");
    build = operation("plan.build",
                      UMI_DEVELOPER_OPERATION_BUILD,
                      "Build");
    test = operation("plan.test",
                     UMI_DEVELOPER_OPERATION_TEST,
                     "Test");

    assert(umi_developer_pipeline_submit(pipeline, &configure) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_submit(pipeline, &build) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_submit(pipeline, &test) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_add_dependency(
        pipeline, "plan.build", "plan.configure") == UMI_STATUS_OK);
    assert(umi_developer_pipeline_add_dependency(
        pipeline, "plan.test", "plan.build") == UMI_STATUS_OK);

    assert(umi_developer_pipeline_add_dependency(
        pipeline, "plan.configure", "plan.test") == UMI_STATUS_INVALID_STATE);

    assert(umi_developer_pipeline_next_ready(pipeline, &next) == UMI_STATUS_OK);
    assert(strcmp(next.id, "plan.configure") == 0);

    assert(umi_developer_pipeline_start(pipeline, next.id) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_set_progress(
        pipeline, next.id, 5000U, "halfway") == UMI_STATUS_OK);
    assert(umi_developer_pipeline_fail(
        pipeline, next.id, 2, "configure failed") == UMI_STATUS_OK);

    assert(umi_developer_pipeline_snapshot(pipeline, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.failed_count == 1U);
    assert(snapshot.blocked_count == 2U);
    assert(snapshot.ready_count == 0U);

    assert(umi_developer_pipeline_retry(pipeline, "plan.configure") ==
           UMI_STATUS_OK);
    assert(umi_developer_pipeline_snapshot(pipeline, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.blocked_count == 0U);
    assert(snapshot.ready_count == 1U);

    assert(umi_developer_pipeline_start(pipeline, "plan.configure") ==
           UMI_STATUS_OK);
    assert(umi_developer_pipeline_complete(
        pipeline, "plan.configure", 0, "configured") == UMI_STATUS_OK);
    assert(umi_developer_pipeline_next_ready(pipeline, &next) == UMI_STATUS_OK);
    assert(strcmp(next.id, "plan.build") == 0);

    assert(umi_developer_pipeline_start(pipeline, "plan.build") == UMI_STATUS_OK);
    assert(umi_developer_pipeline_complete(
        pipeline, "plan.build", 0, "built") == UMI_STATUS_OK);
    assert(umi_developer_pipeline_next_ready(pipeline, &next) == UMI_STATUS_OK);
    assert(strcmp(next.id, "plan.test") == 0);

    assert(umi_developer_pipeline_start(pipeline, "plan.test") == UMI_STATUS_OK);
    assert(umi_developer_pipeline_cancel(
        pipeline, "plan.test", "user cancelled") == UMI_STATUS_OK);
    assert(umi_developer_pipeline_find(pipeline, "plan.test", &item) ==
           UMI_STATUS_OK);
    assert(item.state == UMI_DEVELOPER_OPERATION_CANCELLED);

    umi_developer_pipeline_destroy(pipeline);
    DenseDependencies();
    return 0;
}
