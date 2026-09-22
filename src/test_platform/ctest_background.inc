/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/ctest_background.inc
 * PURPOSE:
 *   Supervise the existing JSON provider with task ownership, Stop, and
 *   owner-thread publication fenced by workspace and registry generations.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/threading.h"

struct UmiCtestDiscoveryJob {
    UmiTask *task;
    UmiTestPlatformCtestImportOptions options;
    UmiTestPlatformCtestDiscoveryOptions execution;
    UmiTestPlatformItemRegistry *items, *stagedItems;
    UmiTestPlatformSuiteRegistry *suites, *stagedSuites;
    UmiTestPlatformDiscoveryRegistry *discoveries, *stagedDiscoveries;
    uint64_t ownerThread, generation, itemRevision, suiteRevision, discoveryRevision;
    UmiTestPlatformCtestImportSummary summary;
    UmiStatus workerStatus, finalStatus;
    int consumed, stopRequested;
    char diagnostics[4096];
};

static void UmiCtestDiscoveryDispose(UmiCtestDiscoveryJob *job)
{
    if (job == NULL) return;
    umi_task_destroy(job->task);
    umi_test_platform_item_registry_destroy(job->stagedItems);
    umi_test_platform_suite_registry_destroy(job->stagedSuites);
    umi_test_platform_discovery_registry_destroy(job->stagedDiscoveries);
    free(job);
}

static UmiStatus UmiCtestDiscoveryWorker(UmiTaskContext *context, void *payload)
{
    UmiCtestDiscoveryJob *job = payload;
    UmiTestPlatformCtestDiscoveryOptions execution = job->execution;
    execution.cancellation = UmiTaskContextCancellation(context);
    job->workerStatus = UmiTestPlatformCtestDiscoverConfigured(&job->options,
        &execution, job->stagedItems, job->stagedSuites, job->stagedDiscoveries,
        &job->summary, job->diagnostics, sizeof(job->diagnostics));
    /* Task completion's mutex publishes these writes to an owner that observes
     * a terminal state. The callback has no live-registry or GTK access. */
    return job->workerStatus;
}

UmiStatus UmiCtestDiscoveryJobStart(UmiTaskQueue *queue,
    const UmiTestPlatformCtestImportOptions *options,
    const UmiTestPlatformCtestDiscoveryOptions *execution,
    UmiTestPlatformItemRegistry *items, UmiTestPlatformSuiteRegistry *suites,
    UmiTestPlatformDiscoveryRegistry *discoveries, uint64_t generation,
    UmiCtestDiscoveryJob **outJob)
{
    UmiStatus status;
    UmiCtestDiscoveryJob *job;
    UmiTaskConfig config = {0};
    if (outJob == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outJob = NULL;
    if (queue == NULL || items == NULL || suites == NULL || discoveries == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiCtestOptionsValidate(options);
    if (status != UMI_STATUS_OK) return status;
    if (execution != NULL && execution->cancellation != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (execution != NULL &&
        (execution->maximum_output_bytes > UMI_TEST_PLATFORM_CTEST_MAX_BYTES ||
         execution->maximum_tests >= UMI_TEST_PLATFORM_ITEM_MAX_CAPACITY))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    job = calloc(1U, sizeof(*job));
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    job->options = *options;
    if (execution != NULL) job->execution = *execution;
    job->items = items; job->suites = suites; job->discoveries = discoveries;
    job->generation = generation; job->ownerThread = umi_thread_current_id();
    job->itemRevision = umi_test_platform_item_registry_revision(items);
    job->suiteRevision = umi_test_platform_suite_registry_revision(suites);
    job->discoveryRevision = umi_test_platform_discovery_registry_revision(discoveries);
    job->workerStatus = UMI_STATUS_BUSY; job->finalStatus = UMI_STATUS_BUSY;
    status = UmiTestPlatformItemRegistryClone(items, &job->stagedItems);
    if (status == UMI_STATUS_OK)
        status = UmiTestPlatformSuiteRegistryClone(suites, &job->stagedSuites);
    if (status == UMI_STATUS_OK)
        status = UmiTestPlatformDiscoveryRegistryClone(discoveries, &job->stagedDiscoveries);
    config.label = "CTest metadata discovery";
    config.function = UmiCtestDiscoveryWorker; config.user_data = job;
    if (status == UMI_STATUS_OK) status = umi_task_create(&config, &job->task);
    if (status == UMI_STATUS_OK) status = umi_task_queue_submit(queue, job->task);
    if (status != UMI_STATUS_OK) {
        UmiCtestDiscoveryDispose(job);
        return status;
    }
    *outJob = job;
    return UMI_STATUS_OK;
}

UmiStatus UmiCtestDiscoveryJobPoll(UmiCtestDiscoveryJob *job,
    uint64_t currentGeneration, UmiTestPlatformCtestImportSummary *outSummary,
    int *outPublished, char *outDiagnostics, size_t diagnosticsCapacity)
{
    UmiTaskState state;
    if (outSummary != NULL) (void)memset(outSummary, 0, sizeof(*outSummary));
    if (outPublished != NULL) *outPublished = 0;
    if (outDiagnostics != NULL && diagnosticsCapacity != 0U) outDiagnostics[0] = '\0';
    if (job == NULL || (outDiagnostics == NULL && diagnosticsCapacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (job->ownerThread != umi_thread_current_id()) return UMI_STATUS_INVALID_STATE;
    state = umi_task_state(job->task);
    if (state == UMI_TASK_CREATED || state == UMI_TASK_QUEUED || state == UMI_TASK_RUNNING)
        return UMI_STATUS_BUSY;
    if (!job->consumed) {
        job->finalStatus = umi_task_result(job->task);
        if (job->stopRequested) job->finalStatus = UMI_STATUS_CANCELLED;
        if (job->finalStatus == UMI_STATUS_OK &&
            (currentGeneration != job->generation ||
             umi_test_platform_item_registry_revision(job->items) != job->itemRevision ||
             umi_test_platform_suite_registry_revision(job->suites) != job->suiteRevision ||
             umi_test_platform_discovery_registry_revision(job->discoveries) != job->discoveryRevision)) {
            job->finalStatus = UMI_STATUS_INVALID_STATE;
            (void)snprintf(job->diagnostics, sizeof(job->diagnostics),
                "Discarded stale discovery: workspace or catalogue changed while the worker ran.");
        }
        if (job->finalStatus == UMI_STATUS_OK) {
            UmiTestPlatformItemRegistrySwap(job->items, job->stagedItems);
            UmiTestPlatformSuiteRegistrySwap(job->suites, job->stagedSuites);
            UmiTestPlatformDiscoveryRegistrySwap(job->discoveries, job->stagedDiscoveries);
            if (outPublished != NULL) *outPublished = 1;
        } else if (job->finalStatus == UMI_STATUS_CANCELLED) {
            (void)snprintf(job->diagnostics, sizeof(job->diagnostics),
                "Discovery stopped; the previous catalogue was retained.");
        }
        job->consumed = 1;
    }
    if (job->finalStatus == UMI_STATUS_OK && outSummary != NULL) *outSummary = job->summary;
    if (outDiagnostics != NULL && diagnosticsCapacity != 0U)
        (void)snprintf(outDiagnostics, diagnosticsCapacity, "%s", job->diagnostics);
    return job->finalStatus;
}

UmiStatus UmiCtestDiscoveryJobCancel(UmiCtestDiscoveryJob *job)
{
    if (job == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (job->ownerThread != umi_thread_current_id() || job->consumed)
        return UMI_STATUS_INVALID_STATE;
    job->stopRequested = 1;
    return umi_task_cancel(job->task);
}

UmiStatus UmiCtestDiscoveryJobDestroy(UmiCtestDiscoveryJob *job)
{
    UmiTaskState state;
    if (job == NULL) return UMI_STATUS_OK;
    if (job->ownerThread != umi_thread_current_id()) return UMI_STATUS_INVALID_STATE;
    state = umi_task_state(job->task);
    if (state == UMI_TASK_CREATED || state == UMI_TASK_QUEUED || state == UMI_TASK_RUNNING)
        return UMI_STATUS_BUSY;
    UmiCtestDiscoveryDispose(job);
    return UMI_STATUS_OK;
}
